//
// Created by Taras Shchybovyk on 6/11/18.
//

#pragma once

#include <eosio/chain/types.hpp>

#include <vector>
#include <string>
#include <type_traits>
#include <hidapi.h>

using namespace std;
using namespace eosio::chain;

namespace eosio {

typedef vector<unsigned char> databuf_t;

databuf_t hex_to_databuf(const std::string& hex);
std::string databuf_to_hex(const databuf_t &buf);

size_t pack_short_be(databuf_t &buffer, unsigned short value);
size_t pack_byte_be(databuf_t &buffer, unsigned char value);
size_t pack_buffer(databuf_t &buffer, const databuf_t &source, size_t from, size_t to);

unsigned short unpack_short_be(const databuf_t &buffer, size_t offset);
unsigned char unpack_byte_be(const databuf_t &buffer, size_t offset);

class hid {
public:

    hid(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number = 0);
    hid(const char *path);
    ~hid() { close(); }

    void close();
    void set_non_blocking(int message);
    int write(const databuf_t &message);
    databuf_t read(unsigned int size);
    databuf_t read_timeout(int timeout);
    databuf_t get_feature_report(unsigned char report_id);
    int send_feature_report(const databuf_t &message);

    struct device_info {
        unsigned short vendor_id;
        unsigned short product_id;
        wstring manufacturer;
        wstring product;
        wstring serial_number;
    };

    device_info get_device_info();

    static vector<device_info> devices(unsigned short vendor_id, unsigned short product_id);
    static void initialize();
    static void deinitialize();

private:
    hid_device *_hid_handle;
    unsigned short _vendor_id;
    unsigned short _product_id;
};

}