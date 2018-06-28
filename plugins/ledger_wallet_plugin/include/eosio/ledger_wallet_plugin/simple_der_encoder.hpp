//
// Created by Taras Shchybovyk on 6/27/18.
//

#ifndef EOSIO_SIMPLE_DER_ENCODER_H
#define EOSIO_SIMPLE_DER_ENCODER_H

#include <eosio/chain/types.hpp>
#include <eosio/ledger_wallet_plugin/hid.hpp>

namespace eosio {

class simple_der_encoder {
public:
    simple_der_encoder();

    ~simple_der_encoder();

    void write(const char *d, uint32_t dlen);

    void put(char c) { write(&c, 1); }

    void reset();

    databuf_t result();

private:
    databuf_t _buffer;
};

}


#endif //EOSIO_SIMPLE_DER_ENCODER_H
