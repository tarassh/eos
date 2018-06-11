//
// Created by Taras Shchybovyk on 6/11/18.
//

#pragma once

#include <eosio/chain/types.hpp>

#include <vector>
#include <hidapi.h>

using namespace std;
using namespace eosio::chain;

namespace eosio {

class hid {
public:
    typedef vector<unsigned char> databuf_t;

    hid(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number = 0);
    hid(const char *path);
    ~hid() { close(); }

    void close();

private:
    hid_device *_hid_handle;
};

}