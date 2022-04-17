struct [[eosio::table]] st_seed {
    uint64_t pk = 1;
    uint32_t last = 1;

    uint64_t primary_key() const { return pk; }
};

typedef eosio::multi_index<"seed"_n, st_seed> tb_seed;