/**
 * 0xRacers Smart Contract
 *
 * @author 0xGames @s5aava
 */
#pragma once


class admin_control : public main_control {
private:

	void issue_gift_tokens(name to, string tkn_name, uint8_t lootbox_type, uint8_t count){
        std::vector<string> uris = {};

        for (uint8_t l = 0; l < count; l++){
            uris.push_back("lootbox " + to_string(lootbox_type) + " " + to_string(l));
        }

        action(permission_level{_self, "active"_n }, _self, "issue"_n,
            make_tuple(to, string2asset(tkn_name, count, 0), uris, tkn_name, string(""), lootbox_type)
        ).send();

    	createEventGiftBox(lootbox_type, from);
    }

    void createEventGiftBox(uint8_t lootbox_type, name from){
        map<string, string> data;
        data.insert({
            { "lootbox_type", to_string(lootbox_type) },
            { "from", from.to_string() }            
        });
        logEvents("gift_lootbox", data);
    }

public:

    name _self;

	tb_config tbconfig;
    tb_auction tbauction;
	tb_prop_cars tbpropcars;
	tb_prop_parts tbpropparts;
	tb_events tbevents;
	token_index tokens;

	
    admin_control( name _self)
        : _self(_self)
		, tbconfig(_self, _self.value)
        , tbauction(_self, _self.value)
		, tbpropcars(_self, _self.value)
		, tbpropparts(_self, _self.value)
		, tbevents(_self, _self.value)
		, tokens(_self, _self.value)
		{}


	void giftlootbox(name to_account, uint8_t lootbox_type){
		require_auth(_self);

        if(lootbox_type >= 1 && lootbox_type <= 4){
            issue_gift_tokens(to_account, string("CAR"), lootbox_type, 1);      // 1 CAR

        }else if(lootbox_type >= 5 && lootbox_type <= 8){
            issue_gift_tokens(to_account, string("PART"), lootbox_type, 3);      // 3 PART

        }else{
            check(false, "lootbox type is wrong");
        }
	}

	void initdiscount( uint8_t discount ) {
		require_auth(_self);

        auto itr_config = tbconfig.begin();
        if (itr_config == tbconfig.end()) {
			print("emplace");
            itr_config = tbconfig.emplace( _self, [&]( auto& row ) { 
				row.config_id = 1;
				row.discount = 0;
			});
        }else{
			print("modify");
			tbconfig.modify( itr_config, _self, [&]( auto& row ) {
				row.discount = discount;
			});
		}
	}

    void clearLogEvents(std::vector <uint64_t> ids){
		require_auth(_self);

		if (ids.empty()) {
			for (const auto &event : tbevents) {
				ids.push_back(event.event_id);
			}
		}

		for (auto &id : ids) {
			auto it = tbevents.find(id);
			if (it != tbevents.end()) {
				tbevents.erase(it);
			}
		}

    }

	void clrall() {
		require_auth(_self);

		auto iter = tbauction.begin();
		while (iter != tbauction.cend()) {
			iter = tbauction.erase(iter);
		}

		auto iter2 = tbpropcars.begin();
		while (iter2 != tbpropcars.cend()) {
			iter2 = tbpropcars.erase(iter2);
		}

		auto iter3 = tbpropparts.begin();
		while (iter3 != tbpropparts.cend()) {
			iter3 = tbpropparts.erase(iter3);
		}
	
		auto iter4 = tbevents.begin();
		while (iter4 != tbevents.cend()) {
			iter4 = tbevents.erase(iter4);
		}

		auto iter5 = tbconfig.begin();
		while (iter5 != tbconfig.cend()) {
			iter5 = tbconfig.erase(iter5);
		}

		auto iter6 = tokens.begin();
		while (iter6 != tokens.cend()) {
			iter6 = tokens.erase(iter6);
		}
		
	}

    void logEvents(string event_type, map<string, string> data){
        tbevents.emplace( _self, [&]( auto& row ) {
			row.event_id = tbevents.available_primary_key();
			row.event_type = event_type;		
			row.contract_address = _self;
			row.date = current_time();
			row.trx_hash = getTransactionHash();
		 	row.data = data;
        });
    }

	checksum256 getTransactionHash(){
        auto size = transaction_size();
        char buf[size];
        uint32_t read = read_transaction( buf, size );
        check( size == read, "read_transaction failed");
        checksum256 h = sha256(buf, read);
		return h;
	}

};