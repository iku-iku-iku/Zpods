//
// Created by code4love on 23-9-24.
//

#include "archive.h"
#include "fs.h"

using namespace zpods;

namespace {
    struct Header {
        size_t data_len;
        uint8_t path_len;
        char path[];

        static constexpr auto real_size() {
            return sizeof(Header::data_len) + sizeof(Header::path_len);
        }

        auto get_path() {
            let p = reinterpret_cast<const char *>(this) + real_size();
            return std::string_view(p, path_len);
        }

        [[nodiscard]] auto size() const {
            return real_size() + path_len;
        }

        [[nodiscard]] auto empty() const {
            return data_len == 0 && path_len == 0;
        }
    };

    auto as_header(auto *p) {
        return reinterpret_cast<Header *>(p);
    }
}

Status zpods::archive(const char *src_path, const char *target_dir, ref <BackupConfig> config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    size_t total_size = 0;

    let_mut iter = fs::FileCollector(src_path);
    const std::vector file_paths(iter.begin(), iter.end());
    std::vector<const char *> relative_paths;
    std::vector<size_t> data_sizes;
    std::vector<size_t> path_sizes;

    let file_cnt = file_paths.size();
    ZPODS_ASSERT(src_path[strlen(src_path) - 1] != '/');
    let base = fs::get_base_name(src_path);
    for (let_ref path: file_paths) {
        let rel = fs::relative(path.c_str(), base.c_str());
        relative_paths.push_back(rel);
        spdlog::info("archived file {}", rel);
        data_sizes.push_back(fs::get_file_size(path));
        path_sizes.push_back(strlen(rel));
    }

    constexpr let header_size = Header::real_size();
    total_size += std::accumulate(data_sizes.begin(), data_sizes.end(), 0ul);
    total_size += std::accumulate(path_sizes.begin(), path_sizes.end(), 0ul);
    total_size += (file_cnt + 1) * header_size;
    // align total size to 16 bytes
    total_size = (total_size + 15) & ~15ul;

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

    ZPODS_ASSERT(config.backup_filename.has_value());
    let ofs_path = fs::path(target_dir) / *config.backup_filename;
    let_mut ofs = fs::open_or_create_file_as_ofs(ofs_path.c_str(), fs::ios::binary);
    ofs.write((char *) buffer.data(), (long) total_size);

    return Status::OK;
}

Status zpods::unarchive(const char *src_path, const char *target_dir) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));
    ZPODS_ASSERT(!fs::is_directory(src_path));

    let content = fs::read_from_file(src_path);

    return unarchive({(p_byte) content.data(), content.size()}, target_dir);
}

Status zpods::unarchive(std::span<byte> src_bytes, const char *target_dir) {
    let_mut p = src_bytes.data();
    Header *header;
    while (!(header = as_header(p))->empty()) {
        p += header->size();
        let file_size = header->data_len;
        let file_name = header->get_path();
        let path_name = fmt::format("{}/{}", target_dir, file_name);
        spdlog::info("unarchived file {}", file_name);

        let base_name = fs::get_base_name(path_name.c_str());

        fs::create_directory_if_not_exist(base_name.c_str());

        std::ofstream ofs(path_name);

        ZPODS_ASSERT(ofs.is_open());
        ofs.write((char *) p, (long) file_size);
        p += header->data_len;
    }

    return Status::OK;
}
