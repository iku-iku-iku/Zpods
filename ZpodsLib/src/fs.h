//
// Created by code4love on 23-9-24.
//

#ifndef ZPODS_FS_H
#define ZPODS_FS_H

#include "pch.h"
#include <filesystem>

namespace zpods {
    namespace fs {

        using path_type = std::filesystem::path;

        inline bool is_directory(ref<std::string> path) {
            return std::filesystem::is_directory(path);
        }

        inline auto directory_iterator(ref<std::string> path) {
            return std::filesystem::directory_iterator(path);
        }

        inline auto get_file_size(ref<path_type> path) {
            return std::filesystem::file_size(path);
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
