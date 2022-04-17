struct [[eosio::table]] st_events {
    uint64_t event_id;
    string event_type;
    name contract_address;
    uint64_t date;
    checksum256 trx_hash;
    std::map<string, string> data;

    uint64_t primary_key() const { return event_id; }
};

typedef eosio::multi_index<"events"_n, st_events> tb_events;
