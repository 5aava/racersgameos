/**
 * 0xRacers Smart Contract
 *
 * @author 0xGames @s5aava
 */
#pragma once


class lootbox_control : public main_control {

private:
    name _self;
    uint64_t _scope = _self.value;
    tb_config tbconfig;
    admin_control &admin_controller;


    void check_lootbox_price(asset price, int64_t lootbox_usd_price ){
        auto itr = tbconfig.find(1);
        eosio::check(itr != tbconfig.end(), "It is not define discount");
        int8_t discount = itr->discount;

        usdtable table(name(EXCHANGE_RATE_CONTRACT_ACCOUNT), name(EXCHANGE_RATE_CONTRACT_ACCOUNT).value);
        auto rate = table.get(EXCHANGE_RATE_WORK_ID).value;
        auto eos_price = lootbox_usd_price * rate * (100 - discount) / 100;

        asset mustprice = string2asset("EOS", eos_price, 4);
        eosio::check(price >= mustprice, "You send wrong price lootbox");
    }


    void issue_action_tokens(asset price, name from, string tkn_name, uint8_t lootbox_type, uint8_t count){
        std::vector<string> uris = {};
        for (uint8_t l = 0; l < count; l++){
            uris.push_back("lootbox " + to_string(lootbox_type) + " " + to_string(l));
        }

        action(permission_level{_self, "active"_n }, _self, "issue"_n,
            make_tuple(from, string2asset(tkn_name, count, 0), uris, tkn_name, string(""), lootbox_type)
        ).send();

        createEventLoobtox(price, lootbox_type, from, tkn_name);
    }


    void createEventLoobtox(asset price, uint8_t lootbox_type, name from, string tkn_name){
        map<string, string> data;
        data.insert({
            { "price", to_string(price.amount) },
            { "lootbox_type", to_string(lootbox_type) },
            { "from", from.to_string() },
            { "tkn_name", tkn_name }
        });
        admin_controller.logEvents("buy_lootbox", data);
    }

public:
    lootbox_control(  name _self
                    , uint64_t _scope
                    , admin_control &_admin_control )
        : _self(_self)
        , tbconfig(_self, _scope)
        , admin_controller(_admin_control)
        {}

    void buy_lootbox(name from, uint8_t lootbox_type, asset price){
        if(lootbox_type == 1){
            check_lootbox_price(price, LOOTBOX_PRICE_CAR_COMMON);
            issue_action_tokens(price, from, string("CAR"), lootbox_type, 1);

        }else if(lootbox_type == 2){
            check_lootbox_price(price, LOOTBOX_PRICE_CAR_PLUS);
            issue_action_tokens(price, from, string("CAR"), lootbox_type, 1);

        }else if(lootbox_type == 3){
            check_lootbox_price(price, LOOTBOX_PRICE_CAR_PRO);
            issue_action_tokens(price, from, string("CAR"), lootbox_type, 1);

        }else if(lootbox_type == 4){
            check_lootbox_price(price, LOOTBOX_PRICE_CAR_VIP);
            issue_action_tokens(price, from, string("CAR"), lootbox_type, 1);

        }else if(lootbox_type == 5){
            check_lootbox_price(price, LOOTBOX_PRICE_PART_COMMON);
            issue_action_tokens(price, from, string("PART"), lootbox_type, 3);

        }else if(lootbox_type == 6){
            check_lootbox_price(price, LOOTBOX_PRICE_PART_PLUS);
            issue_action_tokens(price, from, string("PART"), lootbox_type, 3);

        }else if(lootbox_type == 7){
            check_lootbox_price(price, LOOTBOX_PRICE_PART_PRO);
            issue_action_tokens(price, from, string("PART"), lootbox_type, 3);

        }else if(lootbox_type == 8){
            check_lootbox_price(price, LOOTBOX_PRICE_PART_VIP);
            issue_action_tokens(price, from, string("PART"), lootbox_type, 3);

        }else{
            eosio::check(false, "lootbox type is wrong");
        }
    }

};
