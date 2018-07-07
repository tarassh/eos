//// Created by Taras Shchybovyk on 6/11/18.//#include <eosio/ledger_wallet_plugin/ledger_wallet_manager.hpp>#include <eosio/ledger_wallet_plugin/simple_der_encoder.hpp>#include <eosio/ledger_wallet_plugin/httpc.hpp>#include <eosio/chain/exceptions.hpp>#include <eosio/chain/asset.hpp>#include <eosio/chain/abi_serializer.hpp>#include <fc/io/json.hpp>#include <thread>namespace eosio {    void ledger_wallet_manager::set_dir(const boost::filesystem::path &p) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    void ledger_wallet_manager::set_timeout(const std::chrono::seconds &t) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    void ledger_wallet_manager::set_timeout(int64_t secs) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    void ledger_wallet_manager::set_eosio_key(const std::string &key) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    std::string ledger_wallet_manager::create(const std::string &name) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");        return "";    }    void ledger_wallet_manager::open(const std::string &name) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    std::vector<std::string> ledger_wallet_manager::list_wallets() {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");        return vector<std::string>();    }    map<public_key_type, private_key_type> ledger_wallet_manager::list_keys(const string &name, const string &pw) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");        return map<public_key_type, private_key_type>();    }    void ledger_wallet_manager::lock_all() {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    void ledger_wallet_manager::lock(const std::string &name) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    void ledger_wallet_manager::unlock(const std::string &name, const std::string &password) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    void ledger_wallet_manager::import_key(const std::string &name, const std::string &wif_key) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");    }    string ledger_wallet_manager::create_key(const std::string &name, const std::string &key_type) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger does not support this operation");        return "";    }    vector<string> headers;    string url = "http://dev.cryptolions.io:38888/";    template<typename T>    fc::variant call(const std::string &url,                     const std::string &path,                     const T &v) {        try {            client::http::connection_param *cp = new client::http::connection_param((std::string &) url,                                                                                    (std::string &) path,                                                                                    true, headers);            return client::http::do_http_call(*cp, fc::variant(v), true, true);        }        catch (boost::system::system_error &e) {            throw client::http::connection_exception(fc::log_messages{FC_LOG_MESSAGE(error, e.what())});        }    }    template<typename T>    fc::variant call(const std::string &path,                     const T &v) { return call(url, path, fc::variant(v)); }    template<>    fc::variant call(const std::string &url,                     const std::string &path) { return call(url, path, fc::variant()); }    chain::signed_transaction    ledger_wallet_manager::sign_transaction(const chain::signed_transaction &txn, const flat_set<public_key_type> &keys,                                            const chain::chain_id_type &id) {        signed_transaction stxn(txn);        // get eos key        private_key_type eosio_priv(eosio_key);        public_key_type eosio_pub = eosio_priv.get_public_key();        public_key_type ledger_public_key = get_public_key();        for (const auto &pk : keys) {            if (pk == ledger_public_key) {                transaction &tx = static_cast<transaction &>(stxn);                transaction_header &tx_header = static_cast<transaction_header &>(stxn);                digest_type::encoder hash_enc;                simple_der_encoder enc;                fc::raw::pack(enc, id);                fc::raw::pack(hash_enc, id);                fc::raw::pack(enc, tx_header);                fc::raw::pack(hash_enc, tx_header);                fc::raw::pack(enc, tx.context_free_actions);                fc::raw::pack(hash_enc, tx.context_free_actions);                fc::raw::pack(enc, fc::unsigned_int(tx.actions.size()));                fc::raw::pack(hash_enc, fc::unsigned_int(tx.actions.size()));                for (auto &action: tx.actions) {                    fc::raw::pack(enc, action);                    fc::raw::pack(hash_enc, action);                    databuf_t types;                    auto result = call(client::http::get_abi_func,                                       fc::mutable_variant_object("account_name", action.account));                    auto structs = result["abi"]["structs"];                    for (const auto &st: structs.get_array()) {                        chain::name action_name;                        try {                            action_name = st["name"].as<chain::name>();                        } catch (...) {                            continue;                        }                        if (action.name == action_name) {                            ilog("${i}", ("i", st["name"]));                            uint8_t offset = 0;                            for (const auto &field: st["fields"].get_array()) {                                const auto &type = field["type"].get_string();                                ilog("${i}", ("i", type));                                if (type == "account_name") {                                    types.push_back(0);                                    types.push_back(offset);                                    offset += sizeof(name);                                } else if (type == "asset") {                                    types.push_back(1);                                    types.push_back(offset);                                    offset += sizeof(asset);                                } else if (type == "string") {                                    types.push_back(2);                                    types.push_back(offset);                                } else {                                    EOS_THROW(chain::wallet_missing_pub_key_exception, "UNSUPPORTED TYPE ${k}",                                              ("k", field["type"]));                                }                            }                            break;                        }                    }                    enc.write((char *) types.data(), types.size());                }                fc::raw::pack(enc, tx.transaction_extensions);                fc::raw::pack(hash_enc, tx.transaction_extensions);                if (stxn.context_free_data.size()) {                    fc::raw::pack(enc, digest_type::hash(stxn.context_free_data));                    fc::raw::pack(hash_enc, digest_type::hash(stxn.context_free_data));                } else {                    fc::raw::pack(enc, digest_type());                    fc::raw::pack(hash_enc, digest_type());                }                ilog("HASHHHHH ${h}", ("h", hash_enc.result()));                ilog("ORIGINAL ${o}", ("o", stxn.sig_digest(id, stxn.context_free_data)));                auto data = enc.result();                ilog("===> ${c}", ("c", databuf_to_hex(data)));                bool skip_review = false;                databuf_t raw_signature = sign_data(data, skip_review);                ilog("Signature (${l}) ${c}", ("c", databuf_to_hex(raw_signature))("l", raw_signature.size()));                auto signature = get_compressed_signature(raw_signature);                ilog("REAL Signature ${c}", ("c", signature));                stxn.signatures.push_back(signature);                ilog("${e}", ("e", stxn.get_signature_keys(id)));                break;            } else if (pk == eosio_pub) {                stxn.sign(eosio_priv, id);                ilog("${e}", ("e", stxn.get_signature_keys(id)));                break;            }            EOS_THROW(chain::wallet_missing_pub_key_exception, "Public key not found in unlocked wallets ${k}",                      ("k", pk));        }        return stxn;    }    chain::signature_type    ledger_wallet_manager::sign_digest(const chain::digest_type &digest, const public_key_type &key) {        EOS_THROW(chain::wallet_missing_pub_key_exception, "Ledger signing not realized");        return chain::signature_type();    }    bool ledger_wallet_manager::is_canonical(const databuf_t &data) {        return !(data[1] & 0x80)               && !(data[1] == 0 && !(data[2] & 0x80))               && !(data[33] & 0x80)               && !(data[33] == 0 && !(data[34] & 0x80));    }    databuf_t ledger_wallet_manager::sign_data(const databuf_t &data, bool skip_review) {        auto device = open_device();        databuf_t raw_signature;        unsigned int offset = 0;        auto first = true;        while (offset != data.size()) {            databuf_t chunk;            if (data.size() - offset > 200) {                chunk = databuf_t(data.begin() + offset, data.begin() + offset + 200);            } else {                chunk = databuf_t(data.begin() + offset, data.begin() + data.size());            }            databuf_t buffer;            if (first) {                uint32_t totalSize = 20 + 1 + chunk.size();                buffer = hex_to_databuf("e0040000");                pack_byte_be(buffer, totalSize);                pack_byte_be(buffer, 5);                auto path = hex_to_databuf("8000002c800000c2800000000000000000000000");                pack_buffer(buffer, path, 0, path.size());                first = false;            } else {                uint32_t totalSize = chunk.size();                buffer = hex_to_databuf("e0048000");                pack_byte_be(buffer, totalSize);            }            pack_buffer(buffer, chunk, 0, chunk.size());            ilog("===> ${c}", ("c", databuf_to_hex(buffer)));            raw_signature = exchange(device, buffer);            offset += chunk.size();        }        close_device(device);        return raw_signature;    }    flat_set<public_key_type> ledger_wallet_manager::get_public_keys() {        public_key_type pub_key = get_public_key();        private_key_type priv(eosio_key);        flat_set<public_key_type> result;        result.insert(priv.get_public_key());        result.insert(pub_key);        return result;    }    private_key_type ledger_wallet_manager::get_priv_ke() {        auto device = open_device();        auto buffer = hex_to_databuf("e008000115058000002c800000c2800000000000000000000000");        auto received_buffer = exchange(device, buffer);        close_device(device);        ilog("Received buffer ${c}", ("c", databuf_to_hex(received_buffer)));        private_key_type priv;        memcpy(&priv, buffer.data(), buffer.size());        return priv;    }    public_key_type ledger_wallet_manager::get_public_key() {        auto device = open_device();        auto buffer = hex_to_databuf("e002000115058000002c800000c2800000000000000000000000");        auto received_buffer = exchange(device, buffer);        close_device(device);        ilog("Received buffer ${c}", ("c", databuf_to_hex(received_buffer)));        auto key_length = received_buffer[0];        auto key = databuf_t(received_buffer.begin() + 1, received_buffer.begin() + key_length + 1);        ilog("Public Key ${c}", ("c", databuf_to_hex(key)));        public_key_data data;        if (key[0] == 0x4) {            data = get_compressed_pub_for_key(key);        } else {            const unsigned char *p = key.data();            memcpy(data.data, p, 33);        }        public_key_type pub_key;        memcpy(&pub_key, data.data, 33);        return pub_key;    }    fc::ecc::public_key_data ledger_wallet_manager::get_compressed_pub_for_key(databuf_t key) {        fc::ecc::public_key_data pub_key_data;        const unsigned char *data = key.data();        memcpy(pub_key_data.data + 1, data + 1, 32);        const unsigned char *y = data + 33;        pub_key_data.data[0] = (*y & 1) ? 0x02 : 0x03;        return pub_key_data;    }    signature_type ledger_wallet_manager::get_compressed_signature(databuf_t sig) {        signature_type signature;        memcpy(&signature, sig.data(), sig.size());        return signature;    }    std::unique_ptr<hid> ledger_wallet_manager::open_device() {        auto _hid = std::make_unique<hid>(0x2c97, 0x01, nullptr);        if (_hid)            _hid->set_non_blocking(1);        return _hid;    }    void ledger_wallet_manager::close_device(std::unique_ptr<hid> &device) {        device->close();    }    databuf_t ledger_wallet_manager::exchange(std::unique_ptr<hid> &device, eosio::databuf_t &apdu) {        auto apdu_buffer = wrap_apdu_command(0x0101, apdu, 64);        auto pad_size = apdu_buffer.size() % 64;        auto temp = apdu_buffer;        if (pad_size != 0) {            temp.insert(temp.end(), (64 - pad_size), 0);        }        unsigned int offset = 0;        while (offset != temp.size()) {            auto data = databuf_t(temp.cbegin() + offset, temp.cbegin() + offset + 64);            data.insert(data.begin(), 0x00);            ilog("===> ${c}", ("c", databuf_to_hex(data)));            device->write(data);            offset += 64;        }        size_t data_length = 0;        size_t data_start = 2;        size_t sw_offset = 0;        auto result = wait_first_response(device);        ilog("<=== ${c}", ("c", databuf_to_hex(result)));        device->set_non_blocking(0);        databuf_t response;        while (true) {            response = unwrap_apdu_response(0x0101, result, 64);            if (!response.empty()) {                result = response;                ilog("<=== ${c}", ("c", databuf_to_hex(result)));                data_start = 0;                sw_offset = response.size() - 2;                data_length = response.size() - 2;                device->set_non_blocking(1);                break;            }            auto new_data = device->read(65);            result.insert(result.end(), new_data.begin(), new_data.end());            ilog("<=== ${c}", ("c", databuf_to_hex(result)));        }        unsigned short sw = (((unsigned short) result[sw_offset]) << 8) + result[sw_offset + 1];        response = databuf_t(result.begin() + data_start, result.begin() + data_start + data_length);        if (sw != 0x9000) {            FC_THROW("Ledger Invalid status ${s}", ("s", sw));        }        return response;    }    databuf_t ledger_wallet_manager::wait_first_response(std::unique_ptr<hid> &device, std::chrono::seconds timeout) {        const auto &start = std::chrono::system_clock::now();        databuf_t data;        while (data.size() == 0) {            data = device->read(65);            if (data.size() == 0) {                const auto &now = std::chrono::system_clock::now();                if (now - start > timeout) {                    FC_THROW("Ledger read timeout");                }                std::this_thread::sleep_for(1ms);            }        }        return data;    }    databuf_t ledger_wallet_manager::wrap_apdu_command(unsigned short channel, eosio::databuf_t &command,                                                       unsigned int packet_size) {        if (packet_size < 3) {            FC_THROW("Can't handle Ledger framing with less than 3 bytes for the report");        }        unsigned short sequence_idx = 0;        unsigned int offset = 0;        unsigned short command_length = command.size();        size_t block_size = 0;        databuf_t buffer;        pack_short_be(buffer, channel);        size_t extra_header_size = buffer.size();        pack_byte_be(buffer, 0x05);        pack_short_be(buffer, sequence_idx);        pack_short_be(buffer, command_length);        ++sequence_idx;        if (command_length > packet_size - 5 - extra_header_size) {            block_size = packet_size - 5 - extra_header_size;        } else {            block_size = command_length;        }        pack_buffer(buffer, command, offset, offset + block_size);        offset += block_size;        while (offset != command_length) {            pack_short_be(buffer, channel);            pack_byte_be(buffer, 0x05);            pack_short_be(buffer, sequence_idx);            ++sequence_idx;            if (command_length - offset > packet_size - 3 - extra_header_size) {                block_size = packet_size - 3 - extra_header_size;            } else {                block_size = command_length - offset;            }            pack_buffer(buffer, command, offset, offset + block_size);            offset += block_size;        }        while (buffer.size() % packet_size != 0) {            pack_byte_be(buffer, (unsigned char) 0x00);        }        return buffer;    }    databuf_t ledger_wallet_manager::unwrap_apdu_response(unsigned short channel, eosio::databuf_t &data,                                                          unsigned int packet_size) {        unsigned short sequence_idx = 0;        size_t offset = 0;        size_t block_size = 0;        unsigned int extra_header_size = 2;        if (data.size() < 5 + extra_header_size + 5) {            return databuf_t();        }        if (unpack_short_be(data, offset) != channel) {            FC_THROW("Invalid channel ${c}", ("c", unpack_short_be(data, offset)));        }        offset += 2;        if (unpack_byte_be(data, offset) != 0x05) {            FC_THROW("Invalid tag ${c}", ("c", unpack_byte_be(data, offset)));        }        offset += 1;        if (unpack_short_be(data, offset) != sequence_idx) {            FC_THROW("Invalid sequence ${c}", ("c", unpack_short_be(data, offset)));        }        offset += 2;        unsigned long response_length = unpack_short_be(data, offset);        offset += 2;        if (data.size() < 5 + extra_header_size + response_length) {            return databuf_t();        }        if (response_length > packet_size - 5 - extra_header_size) {            block_size = packet_size - 5 - extra_header_size;        } else {            block_size = response_length;        }        auto result = databuf_t(data.cbegin() + offset, data.cbegin() + offset + block_size);        ilog("<=== ${c}", ("c", databuf_to_hex(result)));        offset += block_size;        while (result.size() != response_length) {            ++sequence_idx;            if (offset == data.size()) {                return databuf_t();            }            if (unpack_short_be(data, offset) != channel) {                FC_THROW("Invalid channel ${c}", ("c", unpack_short_be(data, offset)));            }            offset += 2;            if (unpack_byte_be(data, offset) != 0x05) {                FC_THROW("Invalid tag ${c}", ("c", unpack_byte_be(data, offset)));            }            offset += 1;            if (unpack_short_be(data, offset) != sequence_idx) {                FC_THROW("Invalid sequence ${c}", ("c", unpack_short_be(data, offset)));            }            offset += 2;            if (response_length - result.size() > packet_size - 3 - extra_header_size) {                block_size = packet_size - 3 - extra_header_size;            } else {                block_size = response_length - result.size();            }            result.insert(result.end(), data.begin() + offset, data.begin() + offset + block_size);            offset += block_size;        }        return result;    }}