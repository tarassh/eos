/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/ledger_wallet_api_plugin/ledger_wallet_api_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _ledger_wallet_api_plugin = app().register_plugin<ledger_wallet_api_plugin>();

class ledger_wallet_api_plugin_impl {
   public:
};

ledger_wallet_api_plugin::ledger_wallet_api_plugin():my(new ledger_wallet_api_plugin_impl()){}
ledger_wallet_api_plugin::~ledger_wallet_api_plugin(){}

void ledger_wallet_api_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void ledger_wallet_api_plugin::plugin_initialize(const variables_map& options) {
   if(options.count("option-name")) {
      // Handle the option
   }
}

void ledger_wallet_api_plugin::plugin_startup() {
   // Make the magic happen
}

void ledger_wallet_api_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
