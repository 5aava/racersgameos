struct [[eosio::table]] st_prop_part {
    uint64_t nft_id;
    uint8_t rarity;
    uint8_t tier;

    uint64_t primary_key() const { return nft_id; }
};

typedef eosio::multi_index<"propparts"_n, st_prop_part> tb_prop_parts;