/**
 * 0xRacers Smart Contract
 *
 * @author 0xGames @s5aava
 */
#pragma once


class auction_control : public main_control {
private:
    name _self;
    uint64_t _scope = _self.value;
    map<string, string> data;

    tb_auction tbauction;
    token_index tokens;
    tb_prop_cars tbpropcars;

    admin_control &admin_controller;
    eosio_nft_control &eosio_nft_controller;
    lootbox_control &lootbox_controller;

    asset getPriceWithoutTax(asset price){
        asset send_price = price * (100 - TAX) / 100;
        return send_price;
    }

    void send_money(name to, asset price, string memo, uint64_t nft_id) {
        asset send_price = getPriceWithoutTax(price);

        action(permission_level{_self, "active"_n },
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, to, send_price, memo)
        ).send();
    }

    asset getCurrentPrice(uint64_t nft_id){
		auto nft = tbauction.find(nft_id);
        eosio::check(nft != tbauction.end(), "getCurrentPrice: Could not find token");

		asset sliceprice = nft->startprice - nft->stopprice;		       
        float slicetime = nft->endtime - nft->starttime;
        float pricetic = sliceprice.amount / slicetime; 
        float lasttime = nft->endtime - current_time(); 
        float currentprice = nft->stopprice.amount + (lasttime * pricetic);
        asset returnprice = string2asset("EOS", currentprice, 4);

        if(returnprice.amount < nft->stopprice.amount || returnprice.amount > 10000000000){
            return nft->stopprice;
        }

        return returnprice;
    }

    struct st_transfer {
        name from;
        name to;
        asset   quantity;
        string  memo;
    };

    struct st_memo {
        string type = "buy_nothing";
        uint64_t nft_id = 0;
        string name = "nothing";
    };


public:
    auction_control( name _self
                    , uint64_t _scope
                    , admin_control &_admin_control
                    , eosio_nft_control &_eosio_nft_control
                    , lootbox_control &_lootbox_control )
        : _self(_self)
        , tbauction(_self, _scope)
        , tokens(_self, _scope)
        , tbpropcars(_self, _scope)
        , admin_controller(_admin_control)
        , eosio_nft_controller(_eosio_nft_control)
        , lootbox_controller(_lootbox_control)
        {}

    void sellnft(   name from,
                    uint64_t nft_id,
                    uint64_t startprice,
                    uint64_t stopprice,
                    uint8_t days ) {
        require_auth(from);
        eosio::check(startprice > stopprice, "sellnft: startprice > stopprice");
        eosio::check(days > 0, "sellnft: days > 0");

        uint64_t starttime = current_time();
        uint64_t endtime = starttime + ( days * 24 * 60 * 60 );        

        // log event
        data.insert({
            { "from", from.to_string() },
            { "to", _self.to_string() },
            { "nft_id", to_string(nft_id) },
            { "startprice", to_string(startprice) },
            { "stopprice", to_string(stopprice) },
            { "starttime", to_string(starttime) },
            { "endtime", to_string(endtime) }
        });
        admin_controller.logEvents("sellnft", data);

        // transfer token to auction
        eosio_nft_controller.transferid(from, _self, nft_id, string("") ); 

        tbauction.emplace(from, [&](auto & row) {
            row.nft_id = nft_id;
            row.owner = from;
            row.startprice = string2asset("EOS", startprice, 4);
            row.stopprice = string2asset("EOS", stopprice, 4);
            row.starttime = starttime;
            row.endtime = endtime;
        });
    }

    // removenft *
    void removenft(name from, uint64_t nft_id) {
        require_auth(from);

        auto removeid = tbauction.find(nft_id);
        eosio::check(removeid != tbauction.end(), "removenft: could not find item");
        eosio::check(removeid->owner == from, "removenft: You are not owner token nft");
        
        tbauction.erase(removeid);
        
        // transfer token from auction
        action(permission_level{_self, "active"_n },
            _self, "transferid"_n,
            make_tuple(_self, removeid->owner, nft_id, string("") )
        ).send();

        // log event
        data.insert({
            { "from", from.to_string() },
            { "to", _self.to_string() },
            { "nft_id", to_string(nft_id) }
        });
        admin_controller.logEvents("removenft", data);
    }


    // giftnft *
    void giftnft(name from, name to, uint64_t nft_id) {
        require_auth(from);

        auto sender_token = tokens.find( nft_id );
        eosio::check( sender_token != tokens.end(), "giftnft: token with specified ID does not exist" );
        eosio::check( sender_token->owner == from, "giftnft: sender does not own token with specified ID.");
        
        // Gift transfer token
        eosio_nft_controller.transferid(from, to, nft_id, string(""));

        // log event
        data.insert({
            { "from", from.to_string() },
            { "to", to.to_string() },
            { "nft_id", to_string(nft_id) }
        });
        admin_controller.logEvents("giftnft", data);
    }
    
    
    template<typename T>
    void parse_transfer(T func) {
        
        auto t_data = eosio::unpack_action_data<st_transfer>();

        eosio::check(t_data.quantity.symbol == symbol("EOS", 4), "transfer: only accepts EOS for deposits");
        eosio::check(t_data.quantity.is_valid(), "transfer: Invalid token transfer");
        eosio::check(t_data.quantity.amount > 0, "transfer: Quantity must be positive");

        st_memo res;
        const char dltr[2] = ":";
        const char dltr2[2] = ",";

        size_t b_type = t_data.memo.find(dltr);
        size_t e_type = t_data.memo.find(dltr2, b_type);
        res.type = t_data.memo.substr( b_type + 1, e_type - 5);

        if(e_type != string::npos){
            size_t b_nft_id = t_data.memo.find(dltr, e_type + 1);
            size_t e_nft_id = t_data.memo.find(dltr2, b_nft_id);
            res.nft_id = stoi(t_data.memo.substr( b_nft_id + 1, e_nft_id - 5));

            if(e_nft_id != string::npos){
                size_t b_name = t_data.memo.find(dltr, e_nft_id + 1);
                size_t e_name = t_data.memo.find(dltr2, b_name);
                res.name = t_data.memo.substr( b_name + 1, e_name - 5);
            }
        }

        func(res);
    }

    void transfer(name from, name to, asset price, string memo){
        require_auth(from);

        parse_transfer([&](const auto &pto){

            if(pto.type == "buy_token"){
                auto salenft = tbauction.find(pto.nft_id);
                eosio::check(salenft != tbauction.end(), "buytoken: could not find nft");

                asset cur_price = getCurrentPrice(pto.nft_id);

                print("gcp: ", cur_price.amount, ", price: ", price.amount);
                eosio::check(price.amount >= cur_price.amount, "buytoken: you Send wrong price");                

                eosio::check(from != salenft->owner, "buytoken: You are owner, you cant`t buy yours token");
                tbauction.erase(salenft);
                                
                // transfer token from auction
                action(permission_level{_self, "active"_n },
                    _self, "transferid"_n,
                    make_tuple( _self, from, pto.nft_id, memo)
                ).send();
                
                send_money(salenft->owner, price, "send_money", pto.nft_id );

                // log event
                data.insert({
                    { "from", from.to_string() },
                    { "to", to.to_string() },
                    { "nft_id", to_string(pto.nft_id) },
                    { "price", to_string(price.amount) },
                });
                admin_controller.logEvents("buy_token", data);

            // buy lootbox
            }else if(pto.type == "buy_lootbox"){

                lootbox_controller.buy_lootbox(from, stoi(pto.name), price);

            // buy licence_number
            }else if(pto.type == "buy_number"){
             
                asset mustprice = string2asset("EOS", LICENCE_NUMBER_PRICE, 4);
                eosio::check(price == mustprice, "buynumber: You send wrong price ");
                eosio::check(pto.name.length() == LICENCE_NUMBER_LENGHT, "Number must be 6 chars");

                auto iter = tbpropcars.find(pto.nft_id);
                eosio::check(iter != tbpropcars.end(), "buynumber: could not find nft");

                tbpropcars.modify(iter, _self, [&](auto &s) {
                    s.licence_number = pto.name; 
                });

                // log event
                data.insert({
                    { "from", from.to_string() },
                    { "to", to.to_string() },
                    { "nft_id", to_string(pto.nft_id) },
                    { "price", to_string(price.amount) },
                    { "licence_number", pto.name }
                });
                admin_controller.logEvents("buy_number", data);

            // buy pilot_name
            }else if(pto.type == "buy_pilotname"){
                    
                asset mustprice = string2asset("EOS", PILOT_NAME_PRICE, 4);
                eosio::check(price == mustprice, "buypilotname: You send wrong price");
                eosio::check(pto.name.length() == PILOT_NAME_LENGHT, "Pilot name must be 6 chars");

                auto iter = tbpropcars.find(pto.nft_id);
                eosio::check(iter != tbpropcars.end(), "buypilotname: could not find nft");

                tbpropcars.modify(iter, _self, [&](auto &s) {
                    s.pilot_name = pto.name;
                });

                // log event
                data.insert({
                    { "from", from.to_string() },
                    { "to", to.to_string() },
                    { "nft_id", to_string(pto.nft_id) },
                    { "price", to_string(price.amount) },
                    { "pilot_name", pto.name }
                });
                admin_controller.logEvents("buy_pilotname", data);
            };

        });

    };
};