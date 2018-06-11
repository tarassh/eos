file(GLOB HEADERS "include/eosio/ledger_wallet_plugin/*.hpp")
add_library( ledger_wallet_plugin
             ledger_wallet_plugin.cpp
             ${HEADERS} )

target_link_libraries( ledger_wallet_plugin appbase fc )
target_include_directories( ledger_wallet_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
