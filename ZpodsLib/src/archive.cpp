//
// Created by code4love on 23-9-24.
//

#include "archive.h"
#include "fs.h"
#include "zpods_core.h"

using namespace zpods;

Status zpods::archive(const char *target_dir, const BackupConfig &config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    size_t total_size = 0;

    if (config.filter.paths.empty()) {
        return Status::EMPTY;
    }

    std::vector<fs::zpath> file_paths;
    let_mut collector = zpods::fs::FileCollector{config.filter};
    {
        file_paths = collector.paths();
    }
//    const std::vector file_paths(collector.begin(), collector.end());
    std::vector<std::string> relative_paths;
    std::vector<size_t> data_sizes;
    std::vector<size_t> path_sizes;

    let file_cnt = file_paths.size();
    for (const auto &item: config.filter.paths) {
        ZPODS_ASSERT(item.c_str()[strlen(item.c_str()) - 1] != '/');
    }

    for (let_ref path: file_paths) {
        let rel = collector.get_relative_path(path.c_str());
        relative_paths.emplace_back(rel);
        spdlog::info("archived file {}", rel);
        data_sizes.push_back(fs::get_file_size(path));
        path_sizes.push_back(strlen(rel));
    }

    constexpr let header_size = PodHeader::compact_size();
    total_size += std::accumulate(data_sizes.begin(), data_sizes.end(), 0ul);
    total_size += std::accumulate(path_sizes.begin(), path_sizes.end(), 0ul);
    total_size += (file_cnt + 1) * header_size;
    // align total size to 16 bytes
    total_size = (total_size + 15) & ~15ul;

    let buffer = std::unique_ptr<byte[]>(new byte[total_size]);
//    std::vector<byte> buffer(total_size);
    p_byte p = buffer.get();

    for (size_t i = 0; i < file_cnt; i++) {
        let_mut_ref header = *PodHeader::as_header(p);
        header.set_data_len(data_sizes[i]);
        ZPODS_ASSERT(path_sizes[i] < 256); // since we use uint8_t to store path_len
        header.path_len = static_cast<uint8_t>(path_sizes[i]);
        header.set_last_modified_ts(fs::last_modified_timestamp(file_paths[i].c_str()));

        memcpy(p + header_size, relative_paths[i].c_str(), path_sizes[i]);
        p += PodHeader::as_header(p)->size();
        std::ifstream ifs(file_paths[i]);
        ifs.read((char *) p, (long) data_sizes[i]);
        p += data_sizes[i];

        ZPODS_ASSERT(p - buffer.get() <= total_size);
    }

    memset(p, 0, header_size);
    ZPODS_ASSERT(config.backup_filename.has_value());
    let ofs_path = fs::path(target_dir) / *config.backup_filename;

    let_mut ofs = fs::open_or_create_file_as_ofs(ofs_path.c_str(), fs::ios::binary);
    ofs.write((char *) buffer.get(), (long) total_size);

    return Status::OK;
}

Status zpods::unarchive(const char *src_path, const char *target_dir) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    if (!fs::exists(src_path)) {
        return Status::PATH_NOT_EXIST;
    }
    ZPODS_ASSERT(!fs::is_directory(src_path));

    let content = fs::read_from_file(src_path);

    return unarchive({(p_byte) content.data(), content.size()}, target_dir);
}

Status zpods::unarchive(std::span<byte> src_bytes, const char *target_dir) {
    return foreach_file_in_zpods_bytes(src_bytes.data(), [&](const PodHeader &header) {
        let path = header.get_path();
        let full_path = fs::path(target_dir) / path;
        spdlog::info("unarchived file {}", full_path.c_str());

        let base_name = fs::get_base_name(full_path.c_str());

        fs::create_directory_if_not_exist(base_name.c_str());

        std::ofstream ofs(full_path);
        ZPODS_ASSERT(ofs.is_open());

        let bytes = header.get_path();
        ofs.write((char *) bytes.data(), bytes.size());
        return Status::OK;
    });
}
