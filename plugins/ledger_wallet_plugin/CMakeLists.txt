file(GLOB HEADERS "include/eosio/ledger_wallet_plugin/*.hpp")
add_library( ledger_wallet_plugin
             hid.cpp
             ledger_wallet_manager.cpp
             ledger_wallet_plugin.cpp
             ${HEADERS} )

link_directories("/usr/local/lib")
target_link_libraries( ledger_wallet_plugin appbase eosio_chain hidapi )
target_include_directories( ledger_wallet_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( ledger_wallet_plugin PUBLIC "/usr/local/include/hidapi")