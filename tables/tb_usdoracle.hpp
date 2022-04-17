struct [[eosio::table]] usd {
    uint64_t id;
    uint64_t value;
    uint64_t timestamp;

    uint64_t primary_key() const {return id;}
};

typedef eosio::multi_index<"usd"_n, usd> usdtable;