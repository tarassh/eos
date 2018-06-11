/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <eosio/ledger_wallet_plugin/ledger_wallet_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

namespace eosio {

using namespace appbase;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
class ledger_wallet_api_plugin : public appbase::plugin<ledger_wallet_api_plugin> {
public:
    ledger_wallet_api_plugin() = default;
    ledger_wallet_api_plugin(const ledger_wallet_api_plugin&) = delete;
    ledger_wallet_api_plugin(ledger_wallet_api_plugin&&) = delete;
    ledger_wallet_api_plugin& operator=(const ledger_wallet_api_plugin&) = delete;
    ledger_wallet_api_plugin& operator=(ledger_wallet_api_plugin&&) = delete;
    virtual ~ledger_wallet_api_plugin() override = default;
 
   APPBASE_PLUGIN_REQUIRES((ledger_wallet_plugin) (http_plugin))
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
};

}
