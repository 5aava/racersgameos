# 0xGames EOS Contract #

RACERSGAMEOS dev version 2.0

# API #

## Admins actions
```cpp
/// @abi action init
ACTION init(    uint8_t ft, 
                asset lbpcom, 
                asset lbpplus, 
                asset lbpro, 
                asset lbpvip, 
                asset licp, 
                asset pilp )

/// @abi action clear tables
ACTION clrall()
```
---
## Auction actions

```cpp
/// @abi action sellnft
ACTION sellnft(	capi_name from,
                uint64_t nft_id,
                uint64_t startprice,
                uint64_t stopprice,
                uint8_t days,
                string memo )

/// @abi action removenft
ACTION removenft(   capi_name from, 
                    uint64_t nft_id, 
                    string memo)

/// @abi action giftnft
ACTION giftnft(     capi_name from, 
                    capi_name to_account,
                    uint64_t nft_id, 
                    string memo) 

/// @abi action transfer
ACTION transfer(    capi_name from, 
                    capi_name to, 
                    asset price, 
                    string memo)
```
---
## EOSIO NFT actions

```cpp
/// @abi action issue
ACTION issue(   capi_name to, 
                asset quantity, 
                std::vector<string> uris, 
                string name, 
                string memo)

/// @abi action nfttransfer
ACTION nfttransfer(   capi_name from, 
                      capi_name to_account, 
                      uint64_t nft_id, 
                      string memo)

/// @abi action create
ACTION create( capi_name issuer, string sym )

/// @abi action burn
ACTION burn( capi_name from, uint64_t nft_id )

/// @abi action cleartokens
ACTION cleartokens() 

/// @abi action clearsymbol
ACTION clearsymbol(asset value)
```
---
## For my tests

```cpp
/// @abi action test
ACTION test()
```

---

by @s5aava with love
