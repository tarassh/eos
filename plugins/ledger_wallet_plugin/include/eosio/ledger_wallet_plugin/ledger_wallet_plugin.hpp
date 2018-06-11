/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <appbase/application.hpp>

namespace eosio {

    using namespace appbase;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
    class ledger_wallet_plugin : public appbase::plugin<ledger_wallet_plugin> {
    public:
        ledger_wallet_plugin();

        virtual ~ledger_wallet_plugin();

        APPBASE_PLUGIN_REQUIRES()

        virtual void set_program_options(options_description &, options_description &cfg) override;

        void plugin_initialize(const variables_map &options);

        void plugin_startup();

        void plugin_shutdown();

    private:
        std::unique_ptr<class ledger_wallet_manager> my;
    };

}
