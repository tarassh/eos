//
// Created by Taras Shchybovyk on 6/11/18.
//

#include <eosio/ledger_wallet_plugin/ledger_wallet_manager.hpp>

namespace eosio {

chain::signed_transaction ledger_wallet_manager::sign_transaction(const chain::signed_transaction& txn, const flat_set<public_key_type>& keys,
                                           const chain::chain_id_type& id) {

    return chain::signed_transaction();
}

chain::signature_type ledger_wallet_manager::sign_digest(const chain::digest_type& digest, const public_key_type& key) {
    return chain::signature_type();
}

}
