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

flat_set<public_key_type> ledger_wallet_manager::get_public_keys() {
    hid *device = open_device();

    close_device(device);
    return flat_set<public_key_type>();
}

hid *ledger_wallet_manager::open_device() {
    hid *_hid = new hid(0x2c97, 0x01, nullptr);
    return _hid;
}

void ledger_wallet_manager::close_device(eosio::hid *device) {
    delete device;
}

}

