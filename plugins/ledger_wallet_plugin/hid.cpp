//
// Created by Taras Shchybovyk on 6/11/18.
//

#include <eosio/ledger_wallet_plugin/hid.hpp>
#include <eosio/chain/exceptions.hpp>

namespace eosio {

#define READ_BUFF_MAXSIZE 2048

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

databuf_t hex_to_databuf(const std::string &hex) {
    databuf_t bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byte_string = hex.substr(i, 2);
        auto byte = (unsigned char) strtol(byte_string.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }

    return bytes;
}

std::string databuf_to_hex(const databuf_t &buf) {
    std::string s(buf.size() * 2, ' ');
    for (int i = 0; i < buf.size(); ++i) {
        s[2 * i]     = hexmap[(buf[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[buf[i] & 0x0F];
    }
    return s;
}

size_t pack_back_buffer(databuf_t &buffer, const databuf_t &source, size_t from, size_t to) {
    auto size = buffer.size();
    buffer.insert(buffer.end(), source.cbegin() + from, source.cbegin() + to);
    return buffer.size() - size;
}

size_t pack_back_short_be(databuf_t &buffer, unsigned short value) {
    unsigned char byte = (value >> 8) & 0xFF;
    buffer.push_back(byte);
    byte = (unsigned char) (value & 0xFF);
    buffer.push_back(byte);

    return sizeof(value);
}

size_t pack_back_byte_be(databuf_t &buffer, unsigned char value) {
    unsigned char byte = (unsigned char) (value & 0xFF);
    buffer.push_back(byte);

    return sizeof(value);
}


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

int hid::write(const databuf_t &message) {
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

databuf_t hid::read(unsigned int size) {
    unsigned char *buf = new unsigned char[size];
    int len = hid_read(_hid_handle, buf, size);
    if (len < 0) {
        delete[] buf;
        // THROW
    }
    auto data = databuf_t(buf, buf + len);
    delete[] buf;

    return data;
}

databuf_t hid::read_timeout(int timeout) {
    unsigned char buff_read[READ_BUFF_MAXSIZE];
    int returned_length = hid_read_timeout(_hid_handle, buff_read, sizeof(buff_read), timeout);

    if (returned_length == -1) {
        // THROW could not read
    }

    return databuf_t(buff_read, buff_read + returned_length);
}

databuf_t hid::get_feature_report(unsigned char report_id) {
    unsigned char buff_read[READ_BUFF_MAXSIZE];
    buff_read[0] = report_id;

    int returned_length = hid_get_feature_report(_hid_handle, buff_read, sizeof(buff_read));

    if (returned_length == -1) {
        // THROW could not read
    }

    return databuf_t(buff_read, buff_read + returned_length);
}

int hid::send_feature_report(const databuf_t &message) {
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

void hid::initialize() {
    if (hid_init()) {
        // throw error
    }
}

void hid::deinitialize() {
    if (hid_exit()) {
        // throw error
    }
}

}