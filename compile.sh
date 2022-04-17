eosio-cpp racersgameos.cpp -o racersgameos.wasm
eosio-abigen racersgameos.cpp --contract=racers -output=racersgameos.abi

zip cdt.zip ./racersgameos.abi ./racersgameos.wasm