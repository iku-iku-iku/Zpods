//
// Created by code4love on 23-9-24.
//

#include "archive.h"
#include "fs.h"

namespace {
    struct Header {
        size_t data_len;
        uint8_t path_len;
        char path[];

        static constexpr auto real_size() {
            return sizeof(Header::data_len) + sizeof(Header::path_len);
        }

        auto get_path() {
            let p= reinterpret_cast<const char*>(this) + real_size();
            return std::string_view(p, path_len);
        }

        [[nodiscard]] auto size() const {
            return real_size() + path_len;
        }

        [[nodiscard]] auto empty() const {
            return data_len == 0 && path_len == 0;
        }
    };

    auto as_header(auto* p) {
        return reinterpret_cast<Header*>(p);
    }
}

zpods::Status zpods::archive(const char *src_path, const char *target_dir) {
    if (!zpods::fs::is_directory(target_dir)) {
        return zpods::Status::ERROR;
    }

    size_t total_size = 0;

    auto iter = zpods::fs::FileCollector(src_path);
    const std::vector file_paths(iter.begin(), iter.end());
    std::vector<const char *> relative_paths;
    std::vector<size_t> data_sizes;
    std::vector<size_t> path_sizes;

    let file_cnt = file_paths.size();
    for (const auto &path: file_paths) {
        let rel = zpods::fs::relative(path.c_str(), src_path);
        relative_paths.push_back(rel);
        data_sizes.push_back(zpods::fs::get_file_size(path));
        path_sizes.push_back(strlen(rel));
    }

    constexpr auto header_size = Header::real_size();
    total_size += std::accumulate(data_sizes.begin(), data_sizes.end(), 0ul);
    total_size += std::accumulate(path_sizes.begin(), path_sizes.end(), 0ul);
    total_size += (file_cnt + 1) * header_size;

    std::vector<byte> buffer(total_size);
    p_byte p = buffer.data();

    for (size_t i = 0; i < file_cnt; i++) {
        *as_header(p) = {data_sizes[i], static_cast<uint8_t>(path_sizes[i])};
        memcpy(p + header_size, relative_paths[i], path_sizes[i]);
        p += as_header(p)->size();
        std::ifstream ifs(file_paths[i]);
        ifs.read((char *) p, (long) data_sizes[i]);
        p += data_sizes[i];
    }

    memset(p, 0, header_size);

    std::ofstream ofs(fmt::format("{}/{}", target_dir, "archive").c_str());
    ofs.write((char *) buffer.data(), (long) total_size);

    return zpods::Status::OK;
}

zpods::Status zpods::unarchive(const char *src_path, const char *target_dir) {
    if (!zpods::fs::is_directory(target_dir)) {
        return zpods::Status::ERROR;
    }
    if (zpods::fs::is_directory(src_path)) {
        return zpods::Status::ERROR;
    }

    let total_size = zpods::fs::get_file_size(src_path);
    std::vector<byte> buffer(total_size);

    {
        std::ifstream ifs(src_path);
        ifs.rdbuf()->sgetn((char *) buffer.data(), (long) total_size);
    }

    let_mut p = buffer.data();
    Header *header;
    while (!(header = as_header(p))->empty()) {
        p += header->size();
        let file_size = header->data_len;
        let file_name = header->get_path();
        let path_name = fmt::format("{}/{}", target_dir, file_name);
        spdlog::info("unarchived file {}", file_name);

        let base_name = zpods::fs::get_base_name(path_name.c_str());
        if (!zpods::fs::exists(base_name.c_str())) {
            zpods::fs::create_directory(base_name.c_str());
        }

        std::ofstream ofs(path_name);

        ZPODS_ASSERT(ofs.is_open());
        ofs.write((char *) p, (long) file_size);
        p += header->data_len;
    }

    return zpods::Status::OK;
}
