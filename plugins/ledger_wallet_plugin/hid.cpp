//
// Created by Taras Shchybovyk on 6/11/18.
//

#include <eosio/ledger_wallet_plugin/hid.h>
#include <eosio/chain/exceptions.hpp>

namespace eosio {

#define READ_BUFF_MAXSIZE 2048

hid::hid(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number) {
    _hid_handle = hid_open(vendor_id, product_id, serial_number);

    if (_hid_handle == nullptr) {
        EOS_THROW(chain::wallet_nonexistent_exception, "Device not found: ${v} ${p}", ("v", vendor_id)("p", product_id));
    }
}

hid::hid(const char *path) {
    _hid_handle = hid_open_path(path);
    if (_hid_handle == nullptr) {
        EOS_THROW(chain::wallet_nonexistent_exception, "Device not found: ${p}", ("p", path));
    }
}

void hid::close() {
    if (_hid_handle) {
        hid_close(_hid_handle);
        _hid_handle = nullptr;
    }
}

void hid::set_non_blocking(int message) {
    int res = hid_set_nonblocking(_hid_handle, message);
    if (res < 0) {
        //        EOS_THROW(chain::wallet_nonexistent_exception, "Wallet not found: ${w}", ("w", name));
    }
}

int hid::write(const eosio::hid::databuf_t &message) {
    unsigned char *buf = new unsigned char[message.size()];
    unsigned char *p = buf;
    for (const auto &i : message) {
        *p++ = i;
    }
    int res = hid_write(_hid_handle, buf, message.size());
    delete[] buf;
    if (res < 0) {
        // THOROW
    }

    return res; // return actual number of bytes written
}

hid::databuf_t hid::read() {
    unsigned char buf[READ_BUFF_MAXSIZE];
    int len = hid_read(_hid_handle, buf, sizeof(buf));
    if (len < 0) {
        // THROW
    }

    return databuf_t(buf, buf + len);
}

hid::databuf_t hid::read_timeout(int timeout) {
    unsigned char buff_read[READ_BUFF_MAXSIZE];
    int returned_length = hid_read_timeout(_hid_handle, buff_read, sizeof(buff_read), timeout);

    if (returned_length == -1) {
        // THROW could not read
    }

    return databuf_t(buff_read, buff_read + returned_length);
}

hid::databuf_t hid::get_feature_report(unsigned char report_id) {
    unsigned char buff_read[READ_BUFF_MAXSIZE];
    buff_read[0] = report_id;

    int returned_length = hid_get_feature_report(_hid_handle, buff_read, sizeof(buff_read));

    if (returned_length == -1) {
        // THROW could not read
    }

    return databuf_t(buff_read, buff_read + returned_length);
}

int hid::send_feature_report(const eosio::hid::databuf_t &message) {
    unsigned char *buf = new unsigned char[message.size()];
    unsigned char *p = buf;
    for (const auto &i : message) {
        *p++ = i;
    }

    int returned_length = hid_send_feature_report(_hid_handle, buf, message.size());
    delete[] buf;
    if (returned_length == -1) {
        // THROW Error
    }

    return returned_length;
}

}