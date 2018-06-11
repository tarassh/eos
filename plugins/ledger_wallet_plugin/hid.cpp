//
// Created by Taras Shchybovyk on 6/11/18.
//

#include <eosio/ledger_wallet_plugin/hid.h>
#include <eosio/chain/exceptions.hpp>

namespace eosio {

hid::hid(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number) {
    _hid_handle = hid_open(vendor_id, product_id, serial_number);

    if (_hid_handle == nullptr) {
//        EOS_THROW(chain::wallet_nonexistent_exception, "Wallet not found: ${w}", ("w", name));
    }
}

hid::hid(const char *path)
{
    _hid_handle = hid_open_path(path);
    if (_hid_handle == nullptr) {
//        EOS_THROW(chain::wallet_nonexistent_exception, "Wallet not found: ${w}", ("w", name));
    }
}

void hid::close()
{
    if (_hid_handle)
    {
        hid_close(_hid_handle);
        _hid_handle = nullptr;
    }
}

}