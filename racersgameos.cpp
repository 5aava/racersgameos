/**
 * 0xRacers Smart Contract
 *
 * @author 0xGames @s5aava
 */
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.hpp>
#include <eosiolib/print.hpp>
#include <cmath>
#include <cstdlib>
#include <stdlib.h>
#include <map>

using namespace eosio;

using std::string;
using std::to_string;
using std::vector;
using std::map;


#define CONTRACT_ACCOUNT_NAME "racersgameos"

#define EXCHANGE_RATE_CONTRACT_ACCOUNT "exchangedata"
#define EXCHANGE_RATE_WORK_ID 1

#define TOKEN_PART_NAME "PART"
#define TOKEN_CAR_NAME "CAR"
#define TAX 5

#define LOOTBOX_PRICE_CAR_COMMON 1000
#define LOOTBOX_PRICE_CAR_PLUS 2500
#define LOOTBOX_PRICE_CAR_PRO 5000
#define LOOTBOX_PRICE_CAR_VIP 10000

#define LOOTBOX_PRICE_PART_COMMON 1000
#define LOOTBOX_PRICE_PART_PLUS 2500
#define LOOTBOX_PRICE_PART_PRO 5000
#define LOOTBOX_PRICE_PART_VIP 10000

#define LICENCE_NUMBER_PRICE 1000
#define LICENCE_NUMBER_LENGHT 6

#define PILOT_NAME_PRICE 1000
#define PILOT_NAME_LENGHT 12


#include "controllers/main_control.hpp"

class [[eosio::contract]]
	racers : 	public eosio::contract,
				public main_control {

private:

	/* tables */
	#include "tables/tb_config.hpp"
	#include "tables/tb_events.hpp"
	#include "tables/tb_eosio_nft.hpp"
	#include "tables/tb_prop_car.hpp"
	#include "tables/tb_prop_part.hpp"
	#include "tables/tb_auction.hpp"
	#include "tables/tb_seed.hpp"
	#include "tables/tb_usdoracle.hpp"

	/* controllers */
	#include "controllers/admin_control.hpp"
	#include "controllers/properties_control.hpp"
	#include "controllers/eosio_nft_control.hpp"
	#include "controllers/lootbox_control.hpp"
	#include "controllers/auction_control.hpp"

	admin_control admin_controller;
	properties_control properties_controller;
	eosio_nft_control eosio_nft_controller;
	lootbox_control lootbox_controller;
    auction_control auction_controller;
	

public:
	racers( name self
		   , name code
		   , eosio::datastream<const char*> ds)
		: contract(self, code, ds)
		, admin_controller(self)
		, eosio_nft_controller(self, self.value)		
		, lootbox_controller(self, self.value, admin_controller)
		, properties_controller(self, self.value, admin_controller)
		, auction_controller(self, self.value, admin_controller, eosio_nft_controller, lootbox_controller)
		{}

	/* ========= Admins actions ========= */
	[[eosio::action]]
	void initdiscount( uint8_t discount ) {
		admin_controller.initdiscount( discount );
	}

	[[eosio::action]]
	void clearevt(vector <uint64_t> ids) {
		admin_controller.clearLogEvents(ids);
	}

	[[eosio::action]]
	void giftBox(uint8_t _boxType, name _to) {
		admin_controller.giftlootbox(_to, _boxType);
	}

	[[eosio::action]]
	void clrall() {
		admin_controller.clrall();
	}


	/* ========= Auction actions ========= */
	[[eosio::action]]
	void sellnft( name from,
					uint64_t nft_id,
					uint64_t startprice,
					uint64_t stopprice,
					uint8_t days ) {
		auction_controller.sellnft( from, nft_id, startprice, stopprice, days );		
	}

	[[eosio::action]]
	void removenft(name from, uint64_t nft_id) {
		auction_controller.removenft( from, nft_id );
	}

	[[eosio::action]]
	void giftnft(name from, name to_account, uint64_t nft_id) {
		auction_controller.giftnft( from, to_account, nft_id );
	}

	[[eosio::action]]
	void transfer(name from, name to, asset price, string memo){
		auction_controller.transfer( from, to, price, memo );
	}

	/* ========= Eosio.nft ========= */
	[[eosio::action]]
	void create( name issuer, string sym ) {
		eosio_nft_controller.create( issuer, sym );
	}

	[[eosio::action]]
	void issue( name to, asset quantity, vector<string> uris, string name, string memo, uint8_t lootbox_type){
		properties_controller.issue( to, lootbox_type, name);
		eosio_nft_controller.issue( to, quantity, uris, name, memo );		
	}

	[[eosio::action]]
	void transferid(name from, name to_account, uint64_t nft_id, string memo) {		
		eosio_nft_controller.transferid( from, to_account, nft_id, memo );		
	}

	[[eosio::action]]
	void burn( name from, uint64_t nft_id ) {
		eosio_nft_controller.burn( from, nft_id );
	}


	/* ========= for tests ========= */
	[[eosio::action]]
	void test(){
		properties_controller.test();
    }

};

/* ================================================================= */

#undef EOSIO_DISPATCH

#define EOSIO_DISPATCH( TYPE, MEMBERS ) \
extern "C" {  \
	void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
		if( code == receiver && action != name("transfer").value) { \
			switch( action ) { \
				EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
			} \
		} else if (code == name("eosio.token").value && action == name("transfer").value ) {\
			execute_action( name(receiver), name(code), &racers::transfer );\
		}\
   } \
}

// @abi action 
EOSIO_DISPATCH( racers, (initdiscount)(clearevt)(giftBox)(clrall) (sellnft)(removenft)(giftnft)(transfer) (create)(issue)(transferid)(burn) (test));