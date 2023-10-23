//
// Created by code4love on 23-9-24.
//

#ifndef ZPODS_FS_H
#define ZPODS_FS_H

#include "pch.h"
#include <filesystem>
#include <utility>
#include <regex>

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

        auto get_file_name(const char *path) -> const char *;

        auto get_base_name(const char *path) -> std::string;

        auto open_or_create_file_as_ofs(const char *path, openmode mode) -> std::ofstream;

        auto open_or_create_file_as_ifs(const char *path, openmode mode) -> std::ifstream;

        inline bool is_directory(ref<std::string> path) {
            return std::filesystem::is_directory(path);
        }

        inline auto directory_iterator(ref<std::string> path) {
            return std::filesystem::directory_iterator(path);
        }

        // given a file: xx/yy/zz, retrieve all files like: xx/yy/zz*
        inline auto get_file_family(const char* file_path) {
            std::vector<zpath> paths;
            let dir = path(file_path).parent_path();

            for (const auto &entry: directory_iterator(dir)) {
                if (strstr(entry.path().c_str(), file_path)) {
                    paths.push_back(entry.path());
                }
            }

            return paths;
        }

        inline auto get_file_size(ref<zpath> path) {
            return std::filesystem::file_size(path);
        }

        auto read(const char *path, std::string_view buf);

        auto write(const char *path, std::string_view buf);

        auto read_from_file(const char *path) -> std::string;

        auto relative(const char *path, const char *base) -> const char *;

        bool is_same_content(const char *path1, const char *path2);

        using FileType = std::filesystem::file_type;

        inline auto make_year_month_day(int year, int month, int day) {
            return std::chrono::year_month_day(std::chrono::year(year), std::chrono::month(month),
                                               std::chrono::day(day));
        }

        struct FilesFilter {
            std::vector<zpath> paths; ///< paths to backup
            std::unordered_set<FileType> types{FileType::regular};  ///< types to backup
            std::vector<std::string> re_list; // regular expressions
            std::chrono::year_month_day min_date = make_year_month_day(0, 1, 1);
            ///< only backup files that are modified after min_time
            std::chrono::year_month_day max_date = make_year_month_day(9999, 12, 31);
            ///< only backup files that are modified before max_time
            uintmax_t min_size = 0; ///< only backup files that are larger than min_size
            uintmax_t max_size = (uintmax_t) -1; ///< only backup files that are smaller than max_size
        };


        class FileCollector {
        public:
            using iterator = std::vector<zpath>::iterator;

            explicit FileCollector(FilesFilter filter) : filter_(std::move(filter)) {
                for (const auto &item: filter_.paths) {
                    base_map_.insert({item, item});
                    current_root_ = item;
                    scan_path(item);
                }
            }

            static inline auto file_time_to_date(std::filesystem::file_time_type time) {
                let sys_time = decltype(time)::clock::to_sys(time);
                let tt = std::chrono::system_clock::to_time_t(sys_time);
                let tm = std::localtime(&tt);

                let year = std::chrono::year(tm->tm_year + 1900);
                let month = std::chrono::month(tm->tm_mon + 1);
                let day = std::chrono::day(tm->tm_mday);

                let date = std::chrono::year_month_day(year, month, day);
                return date;
            }

            bool add_path(ref<std::string> path) {
                if (path_set_.contains(path)) {
                    return false;
                }
                let status = std::filesystem::symlink_status(path);
                FileType type = status.type();

                spdlog::debug("PATH: {}, TYPE: {}", path, (int) type);

                // filter with file type
                if (!filter_.types.contains(type)) {
                    return false;
                }

                // filter with size
                uintmax_t bytes_cnt = 0;

                if (status.type() != FileType::directory) {
                    bytes_cnt = std::filesystem::file_size(path);
                    if (bytes_cnt > filter_.max_size || bytes_cnt < filter_.min_size) {
                        return false;
                    }
                }

                // filter with date
                std::filesystem::file_time_type time = std::filesystem::last_write_time(path);
                let date = file_time_to_date(time);

                if (date > filter_.max_date || date < filter_.min_date) {
                    return false;
                }

                // filter with names
                if (!filter_.re_list.empty()) {
                    bool found = false;
                    for (const auto &item: filter_.re_list) {
                        if (std::regex_search(fs::path(path.c_str()).filename().c_str(), std::regex(item))) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        return false;
                    }
                }

                spdlog::info("{}: {}-{}-{} size: {}", path, (int) date.year(), (unsigned) date.month(),
                             (unsigned) date.day(), bytes_cnt);
                path_set_.insert(path);
                paths_.emplace_back(path);

                if (base_map_.count(path) == 0) {
                    base_map_.insert({path, current_root_});
                } else {
                    if (strlen(base_map_[path].c_str()) > strlen(current_root_.c_str())) {
                        base_map_[path] = current_root_;
                    }
                }

                return true;
            }

            auto get_relative_path(ref<zpath> path) const {
                let it = base_map_.find(path);
                ZPODS_ASSERT(it != base_map_.end());
                return fs::relative(path.c_str(), it->second.parent_path().c_str());
            }

            void scan_path(ref<std::string> path) {
                add_path(path);

                // for non dir, no need to dfs
                // for dir, continue to dfs
                if (is_directory(path)) {
                    for (const auto &entry: directory_iterator(path)) {
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
            std::unordered_set<zpath> path_set_;
            FilesFilter filter_;
            std::unordered_map<zpath, zpath> base_map_;
            zpath current_root_;
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
            fs::FilesFilter filter;
            filter.paths.emplace_back(path_);
            filter.types.insert(fs::FileType::directory);
            for (let_ref p: fs::FileCollector(std::move(filter))) {
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

// 定义字面量函数
constexpr unsigned long long operator "" _KB(unsigned long long x) {
    return x * 1024;
}

constexpr unsigned long long operator "" _KB(long double x) {
    return static_cast<unsigned long long>(x * 1024.0);
}

constexpr unsigned long long operator "" _MB(unsigned long long x) {
    return x * 1024 * 1024;
}

constexpr unsigned long long operator "" _MB(long double x) {
    return static_cast<unsigned long long>(x * 1024.0 * 1024.0);
}

constexpr unsigned long long operator "" _GB(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}

constexpr unsigned long long operator "" _GB(long double x) {
    return static_cast<unsigned long long>(x * 1024.0 * 1024.0 * 1024.0);
}

#endif //ZPODS_FS_H
