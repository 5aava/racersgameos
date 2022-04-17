struct [[eosio::table]] config {
    uint8_t config_id = 1;
    int8_t discount = 0;
    
    uint64_t primary_key() const { return config_id; }
};

typedef eosio::multi_index<"config"_n, config> tb_config;