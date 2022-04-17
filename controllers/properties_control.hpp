/**
 * 0xRacers Smart Contract
 *
 * @author 0xGames @s5aava
 */
#pragma once 


class properties_control : public main_control {
private:
    name _self;
    uint64_t _scope = _self.value;

	tb_prop_cars tbpropcars;
	tb_prop_parts tbpropparts;
    token_index tokens;
    tb_seed tbseed;

    admin_control &admin_controller;

    uint16_t random(uint16_t num) {
        auto mixedBlock = tapos_block_prefix() * tapos_block_num();

        const char *mixedChar = reinterpret_cast<const char *>(&mixedBlock);
        checksum256 result = sha256( (char *)mixedChar, sizeof(mixedChar));
        const char *p64 = reinterpret_cast<const char *>(&result);

        auto itr_seed = tbseed.begin();
        if (itr_seed == tbseed.end()) {
            itr_seed = tbseed.emplace( _self, [&]( auto& r ) { });
        }

        auto new_seed = (itr_seed->last + current_time() + abs((int)p64[9]) ) % 65537;
        tbseed.modify( itr_seed, _self, [&]( auto& s ) {
            s.last = new_seed;
        });
        return new_seed % num;
    }

    uint16_t randomRange(uint16_t min, uint16_t max){
        eosio::check(min < max, "min < max");
        uint16_t i = max -min;
        uint16_t r = random(i);
        return min + r;
    }

public:
    properties_control( name _self
                       , uint64_t _scope
                       , admin_control &_admin_control )
        : _self(_self)
        , tbpropcars(_self, _scope)
        , tbpropparts(_self, _scope)
        , tokens(_self, _scope)
        , tbseed(_self, _scope)
        , admin_controller(_admin_control)
        {}

    // ================== issue
    void issue(name to, uint8_t lootbox_type, string token_name){
        require_auth( _self );

        std::vector<uint8_t> rarities = generate_rarities(lootbox_type);
        uint8_t tier = generate_tier(lootbox_type);
        auto start_id = tokens.available_primary_key();

        for (int i = 0; i < rarities.size(); i++) {
            auto id = start_id + i;
            auto rarity = rarities[i];
            add_properties(to, token_name, rarity, tier, id);
        }

    }

    // ================== add_properties
    void add_properties(name to, string token_name, uint8_t rarity, uint8_t tier, uint64_t id){
        
        // log event
        map<string, string> data;
        data.insert({
            { "from", _self.to_string() },
            { "to", to.to_string() },
            { "nft_id", to_string(id) },
            { "token_name", string(token_name) },
            { "rarity", to_string(rarity) },
            { "tier", to_string(tier) }
        });

        // car
        if(token_name == TOKEN_CAR_NAME){
            uint8_t color = random(10);

            tbpropcars.emplace( _self, [&]( auto& prop ) {
                prop.nft_id = id; 
                prop.rarity = rarity;
                prop.tier = tier;
                prop.color = color;
                prop.vinyl = 0;
                prop.experince = 0;
                prop.licence_number = "";
                prop.pilot_name = "";
            });

            // log event
            data.insert( 
                { "color", to_string(color) }
            );
        }

        // part
        if(token_name == TOKEN_PART_NAME){
            tbpropparts.emplace( _self, [&]( auto& prop ) {
                prop.nft_id = id;
                prop.rarity = rarity;
                prop.tier = tier;
            });
        }

        // log event
        admin_controller.logEvents("issue", data);
    }

    // ================== generate rarities
    std::vector<uint8_t> generate_rarities(uint8_t &lootbox_type) {
        std::vector<uint8_t> rarities;

        if (lootbox_type >= 1 && lootbox_type <= 4){
            rarities = generate_rarity_chance({750, 200, 48, 2}, 1);

        } else if (lootbox_type >= 5 && lootbox_type <= 8) {
            rarities = generate_rarity_chance({750, 200, 48, 2}, 3);

        } else {
            eosio::check(0, "break");
        }
        return rarities;
    }

    // ================== generate tiers
    uint8_t generate_tier(uint8_t &lootbox_type) {
        uint8_t tier;

        if (lootbox_type == 1 ){            tier = 0;
        } else if (lootbox_type == 2 ){     tier = 1;
        } else if (lootbox_type == 3 ){     tier = 2;
        } else if (lootbox_type == 4 ){     tier = 3;
        } else if (lootbox_type == 5 ){     tier = 0;
        } else if (lootbox_type == 6 ){     tier = 1;
        } else if (lootbox_type == 7 ){     tier = 2;
        } else if (lootbox_type == 8 ){     tier = 3;            
        } else {
            eosio::check(0, "break");
        }
        return tier;
    }

    // ================== generate_rarity_chance
    std::vector<uint8_t> generate_rarity_chance(std::vector<uint16_t> chances, uint8_t count) {        
        std::vector<uint8_t> result;

        for (uint8_t i = 0; i < count; i++) { 
            uint16_t rand = random(1000);
            uint16_t chance = 0;

            for (uint8_t j = 0; j < chances.size(); j++) {
                chance += chances[j];
                if (rand < chance) {
                    result.push_back(j);
                    break;
                }
            }
        }
        return result;
    }

};