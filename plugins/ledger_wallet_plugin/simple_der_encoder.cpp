//
// Created by Taras Shchybovyk on 6/27/18.
//

#include <eosio/ledger_wallet_plugin/simple_der_encoder.hpp>

namespace eosio {

simple_der_encoder::~simple_der_encoder() {}

simple_der_encoder::simple_der_encoder() {
    reset();
}

void simple_der_encoder::write(const char *d, uint32_t dlen) {
    const uint8_t type = 0x00;
    const uint8_t octet_string_number = 0x04;
    const uint8_t class_type = 0x00;
    const uint8_t tag = type | class_type | octet_string_number;

    _buffer.push_back(tag);

    if (dlen < 128) {
        _buffer.push_back((uint8_t)dlen);
    } else {
        uint32_t length = dlen;
        databuf_t values;
        while (length != 0) {
            values.push_back(uint8_t(length) & 0xff);
            length >>= 8;
        }
        std::reverse(values.begin(), values.end());
        assert(values.size() < 127);
        uint8_t head = 0x80 | values.size();
        _buffer.push_back(head);
        for (auto val : values) {
            _buffer.push_back(val);
        }
    }

    for (uint32_t i = 0; i < dlen; ++i) {
        _buffer.push_back(d[i]);
    }
}

databuf_t simple_der_encoder::result() {

    return _buffer;
}

void simple_der_encoder::reset() {
    _buffer.clear();
}

}