struct [[eosio::table]] st_prop_cars {
    uint64_t nft_id;
    uint8_t rarity;
    uint8_t tier;
    uint8_t color;
    uint8_t vinyl;
    uint64_t experince;
    string licence_number;
    string pilot_name;

    uint64_t primary_key() const { return nft_id; }
};

typedef eosio::multi_index<"propcars"_n, st_prop_cars> tb_prop_cars;