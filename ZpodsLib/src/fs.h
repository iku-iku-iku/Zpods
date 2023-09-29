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

        using path_type = std::filesystem::path;

        inline auto copy(const char *src, const char *target, copy_options options) {
            std::filesystem::copy(src, target, options);
        }

        inline auto path(const char *path) {
            return std::filesystem::path(path);
        }

        inline auto exists(const char *path) {
            return std::filesystem::exists(path);
        }

        inline auto create_directory(const char *path) {
            std::filesystem::create_directory(path);
        }

        inline auto remove_all(const char *path) {
            std::filesystem::remove_all(path);
        }

        inline auto get_base_name(const char *path) -> std::string {
            size_t len = 0;
            let_mut p = path;
            while (*p) {
                if (*p == '/') {
                    len = p - path;
                }
                p++;
            }
            return {path, len};
        }

        inline auto open_or_create_file_as_ofs(const char *path, openmode mode) -> std::ofstream {
            let base = get_base_name(path);
            if (!exists(base.c_str())) {
                create_directory(base.c_str());
            }
            std::ofstream ofs(path, mode);
            ZPODS_ASSERT(ofs.is_open());
            return ofs;
        }

        inline auto open_or_create_file_as_ifs(const char *path, openmode mode) -> std::ifstream {
            let base = get_base_name(path);
            if (!exists(base.c_str())) {
                create_directory(base.c_str());
            }
            std::ifstream ifs(path, mode);
            ZPODS_ASSERT(ifs.is_open());
            return ifs;
        }

        inline bool is_directory(ref<std::string> path) {
            return std::filesystem::is_directory(path);
        }

        inline auto directory_iterator(ref<std::string> path) {
            return std::filesystem::directory_iterator(path);
        }

        inline auto get_file_size(ref<path_type> path) {
            return std::filesystem::file_size(path);
        }

        inline auto read(const char *path, std::string_view buf) {
            std::ifstream ifs(path);
            ZPODS_ASSERT(ifs.is_open());
            ifs.read(const_cast<char *>(buf.data()), (long) buf.length());
        }

        inline auto write(const char *path, std::string_view buf) {
            std::ofstream ofs(path);
            ZPODS_ASSERT(ofs.is_open());
            ofs.write(buf.data(), (long) buf.length());
        }


        inline auto relative(const char *path, const char *base) -> const char * {
            let_mut len = strlen(base);
            if (base[len - 1] == '/') {
                len--;
            }

            if (strlen(path) == len && !is_directory(base)) {
                let_mut ret = path;
                while (*path) {
                    if (*path == '/') ret = path + 1;
                    path++;
                }
                return ret;
            }

            if (strncmp(path, base, len) == 0) {
                return path + len + 1;
            }
            return nullptr;
        }

        class FileCollector {
        public:
            using iterator = std::vector<path_type>::iterator;

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
            std::vector<path_type> paths_;
        };

    }
}

#endif //ZPODS_FS_H
