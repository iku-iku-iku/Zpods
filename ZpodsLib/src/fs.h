//
// Created by code4love on 23-9-24.
//

#ifndef ZPODS_FS_H
#define ZPODS_FS_H

#include "pch.h"
#include <filesystem>
#include <utility>

#ifdef __linux__

#include <sys/inotify.h>

#endif

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

            FileCollector(ref<std::string> path, FileType type = FileType::REGULAR_FILE) : type_(type) {
                scan_path(path);
            }

            void scan_path(ref<std::string> path) {
                if (!is_directory(path)) {
                    if (type_ & FileType::REGULAR_FILE) {
                        paths_.emplace_back(path);
                    }
                    return;
                } else {
                    if (type_ & FileType::DIRECTORY) {
                        paths_.emplace_back(path);
                    }
                }

                for (const auto &entry: directory_iterator(path)) {
                    scan_path(entry.path());
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
            FileType type_;
        };
    }

    class FsWatcher {
    public:
        struct Callback {
            std::function<void(const char *path)> on_file_create;
            std::function<void(const char *path)> on_file_delete;
            std::function<void(const char *path)> on_file_modify;
            std::function<void(const char *path)> on_dir_create;
            std::function<void(const char *path)> on_dir_delete;
        } callback_;

        [[noreturn]] FsWatcher(const char *path, Callback callback) : path_(path), callback_(std::move(callback)) {
#ifdef __linux__
            fd_ = inotify_init();
            ZPODS_ASSERT(fd_ >= 0);
            for (let_ref p: fs::FileCollector(path_, FileType::DIRECTORY)) {
                // add watch for all directories
                add_watch_for(p.c_str());
            }

            while (true) {
                size_t i = 0;
                let len = read(fd_, buf_, sizeof(buf_));

                while (i < len) {
                    let event = (struct inotify_event *) &buf_[i];
                    let dir_path = fs::path(path_) / (event->name);
                    let file_path = fs::path(path_) / (event->name);
                    if (event->len) {
                        if (event->mask & IN_CREATE) {
                            if (event->mask & IN_ISDIR) {
                                // new directory created, add watch for it!
                                spdlog::info("The directory {} was created.", dir_path.c_str());
                                add_watch_for(dir_path.c_str());
                                callback_.on_dir_create(dir_path.c_str());
                            } else {
                                spdlog::info("The file {} was created.", event->name);
                                callback_.on_file_create(file_path.c_str());
                            }
                        } else if (event->mask & IN_DELETE) {
                            if (event->mask & IN_ISDIR) {
                                // remove watch for the deleted directory!
                                let dir_path = fs::path(path_) / (event->name);
                                spdlog::info("The directory {} was deleted.", dir_path.c_str());
                                remove_watch_for(dir_path.c_str());
                                callback_.on_dir_delete(dir_path.c_str());
                            } else {
                                spdlog::info("The file {} was deleted.", event->name);
                                callback_.on_file_delete(file_path.c_str());
                            }
                        } else if (event->mask & IN_MODIFY) {
                            if (event->mask & IN_ISDIR) {
                                spdlog::info("The directory {} was modified.", event->name);
                            } else {
                                spdlog::info("The file {} was modified.", event->name);
                                callback_.on_file_modify(file_path.c_str());
                            }
                        }
                    }
                    i += sizeof(struct inotify_event) + event->len;
                }
            }
#else
#endif
        }

        void add_watch_for(const char *dir) {
            let path = absolute(fs::path(dir));
            ZPODS_ASSERT(path_wd_map_.find(path) == path_wd_map_.end());
#ifdef __linux__
            let wd = inotify_add_watch(fd_, path.c_str(), IN_ALL_EVENTS);
            ZPODS_ASSERT(wd >= 0);
            path_wd_map_.emplace(path, wd);
            spdlog::info("add watch for {}", path.c_str());
#endif
        }

        void remove_watch_for(const char *dir) {
            let path = absolute(fs::path(dir));
#ifdef __linux__
            let wd = path_wd_map_[path];
            inotify_rm_watch(fd_, wd);
            path_wd_map_.erase(path);
            spdlog::info("remove watch for {}", path.c_str());
#endif
        }

        ~FsWatcher() {
            for (let_ref[_, wd]: path_wd_map_) {
                inotify_rm_watch(fd_, wd);
            }
            close(fd_);
        }

    private:
        const char *path_;
        int fd_;
        std::unordered_map<fs::zpath, int> path_wd_map_;
        char buf_[1024];
    };
}

#endif //ZPODS_FS_H
