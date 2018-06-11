//
// Created by Taras Shchybovyk on 6/11/18.
//

#include <eosio/ledger_wallet_plugin/hid.hpp>
#include <eosio/chain/exceptions.hpp>

namespace eosio {

#define READ_BUFF_MAXSIZE 2048

hid::hid(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number)
        : _vendor_id(vendor_id), _product_id(product_id) {
    _hid_handle = hid_open(vendor_id, product_id, serial_number);

    if (_hid_handle == nullptr) {
        // EOS_THROW(chain::wallet_nonexistent_exception, "Device not found: ${v} ${p}", ("v", vendor_id)("p", product_id));
    }
}

hid::hid(const char *path)
        : _vendor_id(0), _product_id(0) {
    _hid_handle = hid_open_path(path);
    if (_hid_handle == nullptr) {
        //EOS_THROW(chain::wallet_nonexistent_exception, "Device not found: ${p}", ("p", path));
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

hid::device_info hid::get_device_info() {
    const size_t max_len = 255;
    wchar_t wstr[max_len];
    device_info info;

    info.vendor_id = _vendor_id;
    info.product_id = _product_id;
    hid_get_manufacturer_string(_hid_handle, wstr, max_len);
    info.manufacturer = wstr;

    hid_get_product_string(_hid_handle, wstr, max_len);
    info.product = wstr;

    hid_get_serial_number_string(_hid_handle, wstr, max_len);
    info.serial_number = wstr;

    return info;
}

vector<hid::device_info> hid::devices(unsigned short vendor_id, unsigned short product_id) {
    hid_device_info *devs = hid_enumerate(vendor_id, product_id);
    vector<device_info> infos;

    hid_device_info *cur_dev = devs;
    while (cur_dev) {
        device_info info;

        info.vendor_id = cur_dev->vendor_id;
        info.product_id = cur_dev->product_id;
        info.manufacturer = cur_dev->manufacturer_string;
        info.product = cur_dev->product_string;
        info.serial_number = cur_dev->serial_number;
        infos.push_back(info);

        cur_dev = cur_dev->next;
    }

    hid_free_enumeration(devs);
    return infos;
}

void hid::deinitialize() {
    if (hid_exit()) {
        // throw error
    }
}

}