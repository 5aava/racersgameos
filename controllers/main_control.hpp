/**
 * 0xRacers Smart Contract
 *
 * @author 0xGames @s5aava
 */
#pragma once


class main_control {
private:

    void assert_true(bool test, const char* cstr) {
        eosio::check(test ? 1 : 0, cstr);
    }

public:

    asset string2asset(string sym, uint64_t quantity, uint8_t precision){
        auto symbolvalue = symbol(sym, precision);
        eosio::asset tosend;
        tosend.amount = quantity;
        tosend.symbol = symbolvalue;
        return tosend;
    }

};