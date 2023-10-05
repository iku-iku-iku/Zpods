//
// Created by code4love on 23-9-24.
//

#ifndef ZPODS_FS_H
#define ZPODS_FS_H

#include "pch.h"
#include <filesystem>

namespace zpods {
    namespace fs {

        namespace ios {
            constexpr auto binary = std::ios::binary;
        }

        using openmode = std::ios::openmode;

        using copy_options = std::filesystem::copy_options;

        using zpath = std::filesystem::path;

        inline auto copy(const char *src, const char *target, copy_options options) {
            std::filesystem::copy(src, target, options);
        }

        inline auto path(const char *path) {
            return std::filesystem::path(path);
        }

        inline auto exists(const char *path) {
            return std::filesystem::exists(path);
        }

        inline auto create_directory_if_not_exist(const char *path) {
            if (!exists(path)) {
                std::filesystem::create_directories(path);
            }
        }

        inline auto remove_all(const char *path) {
            std::filesystem::remove_all(path);
        }

        auto get_base_name(const char *path) -> std::string;

        auto open_or_create_file_as_ofs(const char *path, openmode mode) -> std::ofstream;

        auto open_or_create_file_as_ifs(const char *path, openmode mode) -> std::ifstream;

        inline bool is_directory(ref<std::string> path) {
            return std::filesystem::is_directory(path);
        }

        inline auto directory_iterator(ref<std::string> path) {
            return std::filesystem::directory_iterator(path);
        }

        inline auto get_file_size(ref<zpath> path) {
            return std::filesystem::file_size(path);
        }

        auto read(const char *path, std::string_view buf);

        auto write(const char *path, std::string_view buf);

        auto read_from_file(const char *path) -> std::string;

        auto relative(const char *path, const char *base) -> const char *;

        bool is_same_content(const char *path1, const char *path2);

        class FileCollector {
        public:
            using iterator = std::vector<zpath>::iterator;

            FileCollector(ref<std::string> path) {
                scan_path(path);
            }

            void scan_path(ref<std::string> path) {
                if (!is_directory(path)) {
                    paths_.push_back(path);
                    return;
                }

                for (const auto &entry: directory_iterator(path)) {
                    if (!entry.is_directory()) {
                        paths_.push_back(entry.path());
                    } else {
                        scan_path(entry.path());
                    }
                }
            }

            iterator begin() {
                return paths_.begin();
            }

            iterator end() {
                return paths_.end();
            }

        private:
            std::vector<zpath> paths_;
        };

    }
}

#endif //ZPODS_FS_H
