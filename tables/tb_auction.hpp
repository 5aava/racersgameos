struct [[eosio::table]] st_auction {
    uint64_t nft_id;
    name owner;
    asset startprice;
    asset stopprice;
    uint64_t starttime;
    uint64_t endtime;

    uint64_t primary_key() const { return nft_id; }
};

typedef eosio::multi_index<"auction"_n, st_auction> tb_auction;