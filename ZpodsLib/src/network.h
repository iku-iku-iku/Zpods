//
// Created by code4love on 23-10-9.
//

#ifndef ZPODS_NETWORK_H
#define ZPODS_NETWORK_H

#include "pch.h"
#include "httplib.h"
#include "fs.h"
#include "json.h"

namespace zpods {


    struct Client {

        httplib::Client client_;

        inline static auto json_header() {
            httplib::Headers headers = {
                    {"Content-Type",  "application/json"},
                    {"Authorization", "Bearer YOUR_ACCESS_TOKEN"}
            };

            return headers;
        }

        /*
         * @brief post a json string to server
         * @param json_str json string
         * @return response
         */
        auto post_with_json(const char *path, ref<std::string> json_str) {
            let headers = json_header();

            let_mut res = client_.Post(path, headers, json_str, "application/json");
            return res;
        }

        auto post_with_file(const char *path, ref<httplib::MultipartFormDataItems> items) {
            return client_.Post(path, items);
        }

        auto get(ref<std::string> path) {
            return client_.Get(path);
        }

        inline static auto connect_server() -> Client {
            httplib::Client client("http://localhost:8080");

            return Client{
                    .client_ = std::move(client)
            };
        }
    };


    struct User {
        int id = -1;
        std::string username;
        std::string password;

        auto to_json_str() const -> std::string;

        void dump_res(ref<httplib::Result> res, const char *path = "") const {
            if (res) {
                spdlog::info("[{}]: {}", path, res->body);
            }
        }

        auto request(const char *path) const;

        Status register_() const;

        Status login() const;

        Status unregister() const;

        Status upload_file(const char *file_path);

        Status download_file(const char *hdfs_path, const char *local_dir);
    };

    inline Status user_register(ref<User> user) {
        return user.register_();
    }

    inline Status user_login(ref<User> user) {
        return user.login();
    }

    Status upload_file();

    Status download_file(const char *hdfs_path, const char *local_path);
}

#endif //ZPODS_NETWORK_H
