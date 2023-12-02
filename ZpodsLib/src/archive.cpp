//
// Created by code4love on 23-9-24.
//

#include "archive.h"
#include "fs.h"
#include "zpods_core.h"
#include "manager.h"

using namespace zpods;

Status zpods::archive(const char *target_dir, const BackupConfig &config) {
    ZPODS_ASSERT(fs::is_directory(target_dir));

    size_t total_size = 0;

    let_mut collector = zpods::fs::FileCollector{config.dir_to_backup, config.filter};
    let file_paths = collector.paths();

    std::unordered_set<Pea> pods;

    for (const auto &item: config.filter.paths) {
        ZPODS_ASSERT(item.c_str()[strlen(item.c_str()) - 1] != '/');
    }

    for (let_ref path: file_paths) {
        let rel = fs::relative(path.c_str(), config.dir_to_backup.c_str());
        Pea pod{
                .last_modified_ts = fs::last_modified_timestamp(path.c_str()),
                .rel_path = rel,
                .abs_path = path,
        };
        pods.emplace(std::move(pod));
    }

    for (const auto &item: pods) {
        spdlog::info("file collected before filtered: {}", item);
    }

    pods = PodsManager::Instance()->filter_archived_pods(target_dir, pods);

    let file_cnt = pods.size();
    for (let_ref pod: pods) {
        spdlog::info("archived file {}", pod);

        total_size += fs::get_file_size(pod.abs_path);
        total_size += pod.rel_path.size();
    }

    constexpr let header_size = PeaHeader::compact_size();
    total_size += (file_cnt + 1) * header_size;
    // align total size to 16 bytes
    total_size = (total_size + 15) & ~15ul;

    let buffer = std::unique_ptr<byte[]>(new byte[total_size]);
    p_byte p = buffer.get();

    for (let_ref pod: pods) {
        let_mut_ref header = *PeaHeader::as_header(p);
        let data_size = fs::get_file_size(pod.abs_path);
        header.set_data_len(data_size);
        header.set_normal();
        header.set_path_len(pod.rel_path.size());
        header.set_last_modified_ts(pod.last_modified_ts);

        memcpy(p + header_size, pod.rel_path.c_str(), pod.rel_path.size());
        p += PeaHeader::as_header(p)->size();
        std::ifstream ifs(pod.abs_path);
        ifs.read((char *) p, (long) data_size);
        p += data_size;

        ZPODS_ASSERT(p - buffer.get() <= total_size);
    }

    memset(p, 0, header_size);
    let_mut ofs = fs::open_or_create_file_as_ofs(config.archive_path.c_str(), fs::ios::binary);
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
    return foreach_pea_in_pod_bytes(src_bytes.data(), [&](const PeaHeader &header) {
        let path = header.get_path();
        let full_path = fs::path(target_dir) / path;
        spdlog::info("unarchived file {}", full_path.c_str());

        let base_name = fs::get_base_name(full_path.c_str());

        fs::create_directory_if_not_exist(base_name.c_str());

        std::ofstream ofs(full_path);
        ZPODS_ASSERT(ofs.is_open());

        let bytes = header.get_data();
        ofs.write((char *) bytes.data(), bytes.size());
        return Status::OK;
    });
}
