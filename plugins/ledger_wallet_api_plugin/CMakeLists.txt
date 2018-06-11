file(GLOB HEADERS "include/eosio/ledger_wallet_api_plugin/*.hpp")
add_library( ledger_wallet_api_plugin
             ledger_wallet_api_plugin.cpp
             ${HEADERS} )

target_link_libraries( ledger_wallet_api_plugin appbase ledger_wallet_plugin http_plugin )
target_include_directories( ledger_wallet_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
