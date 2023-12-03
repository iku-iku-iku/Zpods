//
// Created by code4love on 23-9-24.
//

#include "archive.h"
#include "ZpodsLib/src/base/fs.h"
#include "ZpodsLib/src/core/zpods_core.h"
#include "manager.h"

using namespace zpods;

Status zpods::archive(const char *src_dir, const char *dest_dir, const BackupConfig &config) {
    ZPODS_ASSERT(fs::is_directory(dest_dir));

    size_t pod_total_size = 0;

    let_mut collector = zpods::fs::FileCollector{src_dir, config.filter};
    let pea_paths = collector.paths();

    std::unordered_set<Pea> pod;

//    for (const auto &item: config.filter.paths) {
//        ZPODS_ASSERT(item.c_str()[strlen(item.c_str()) - 1] != '/');
//    }
//
    for (let_ref path: pea_paths) {
        let rel = fs::relative(path.c_str(), src_dir);
        Pea pea{
                .last_modified_ts = fs::last_modified_timestamp(path.c_str()),
                .rel_path = rel,
                .abs_path = path,
        };
        pod.emplace(std::move(pea));
    }

    for (const auto &pea: pod) {
        spdlog::info("pea collected before filtered: {}", pea);
    }

    pod = PodsManager::Instance()->filter_archived_peas(dest_dir, pod);
    let pea_cnt = pod.size();
    if (pea_cnt == 0) {
        spdlog::info("no new pea to archive");
        return Status::NO_NEW_TO_ARCHIVE;
    }

    for (let_ref pea: pod) {
        pod_total_size += pea.rel_path.size();
        if (pea.deleted) {
            spdlog::info("file {} is deleted", pea);
        } else {
            spdlog::info("archived file {}", pea);
            pod_total_size += fs::get_file_size(pea.abs_path);
        }
    }

    constexpr let header_size = PeaHeader::compact_size();
    pod_total_size += (pea_cnt + 1) * header_size;
    // align total size to 16 bytes
    pod_total_size = (pod_total_size + 15) & ~15ul;

    let buffer = std::unique_ptr<byte[]>(new byte[pod_total_size]);
    p_byte p = buffer.get();

    for (let_ref pea: pod) {
        let_mut_ref pea_header = *PeaHeader::as_header(p);
        let data_size = pea.deleted ? 0 : fs::get_file_size(pea.abs_path);
        pea_header.set_data_len(data_size);
        if (pea.deleted) {
            pea_header.set_delete();
        } else {
            pea_header.set_normal();
        }
        pea_header.set_path_len(pea.rel_path.size());
        pea_header.set_last_modified_ts(pea.last_modified_ts);

        memcpy(p + header_size, pea.rel_path.c_str(), pea.rel_path.size());
        p += PeaHeader::as_header(p)->size();
        if (!pea.deleted) {
            std::ifstream ifs(pea.abs_path);
            ifs.read((char *) p, (long) data_size);
            p += data_size;
        }

        ZPODS_ASSERT(p - buffer.get() <= pod_total_size);
    }

    memset(p, 0, header_size);
    let_mut ofs = fs::open_or_create_file_as_ofs(config.current_pod_path.c_str(), fs::ios::binary);
    ofs.write((char *) buffer.get(), (long) pod_total_size);

    return Status::OK;
}

Status zpods::unarchive(const char *src_path, const char *dest_dir) {
    fs::create_directory_if_not_exist(dest_dir);
    ZPODS_ASSERT(fs::is_directory(dest_dir));

    if (!fs::exists(src_path)) {
        return Status::PATH_NOT_EXIST;
    }
    ZPODS_ASSERT(!fs::is_directory(src_path));

    let content = fs::read_from_file(src_path);

    return unarchive({(p_byte) content.data(), content.size()}, dest_dir);
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
