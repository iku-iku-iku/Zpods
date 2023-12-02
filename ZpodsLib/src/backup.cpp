#include "backup.h"
#include "compress.h"
#include "archive.h"
#include "fs.h"
#include "crypto.h"
#include "zpods_core.h"
#include "manager.h"

using namespace zpods;

using namespace zpods::fs;

namespace {
    // 获取当前的Unix时间戳
    long get_current_timestamp() {
        // 获取当前时间点
        let now = std::chrono::system_clock::now();

        // 转换为时间戳
        let duration = now.time_since_epoch();

        // 将时间戳转换为秒
        let seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

        // 返回秒数
        return seconds.count();
    }
}

static Status backup_one(const char *target_dir, const BackupConfig &config) {
    // if not specified backup file name, deduce from path
    // TODO: deduce is compulsion
//    if (!config.backup_filename.has_value()) {
//        config.backup_filename = fmt::format("{}{}", config.dir_to_backup.filename().c_str(),
//                                             POD_FILE_SUFFIX);
//    }
    PodsManager::Instance()->load_pods(target_dir);

    do {
        config.archive_path = fs::path(target_dir) / fmt::format("{}{}", get_current_timestamp(), POD_FILE_SUFFIX);
    } while (fs::exists(config.archive_path.c_str()));


//    if (fs::exists(archive_path.c_str())) {
//        spdlog::info("existed pods {}", archive_path.c_str());
//        PodsManager::Instance()->load_pods(archive_path);
//        for (const auto &item: PodsManager::Instance()->current_pods()) {
//            spdlog::info("detected archived file {}", item);
//        }
//    }

    // if there exist the pods file
//    if (fs::exists(archive_path.c_str())) {
//        // need delta backup
//        if (config.delta_backup) {
//            // get paths from backup file and all delta backup files
//            PodsManager::Instance()->load_pods(archive_path);
//        } else {
//            // if not enabled delta backup, just remove it.
//            fs::remove_file(archive_path.c_str());
//        }
//    }

    // 1. archive files of src_path to a single file in target_dir
    zpods::archive(target_dir, config);

    let_mut bytes = fs::read_from_file(config.archive_path.c_str());
    std::unique_ptr<byte[]> buf;
    size_t buf_len;
    // 2. compress if needed
    if (config.compress) {
        std::tie(buf_len, buf) = compress({(p_byte) bytes.data(), bytes.size()});
        bytes = {as_c_str(buf.get()), buf_len};
        spdlog::info("file compression succeeded : {}", config.archive_path.c_str());
    }

    // 3. encrypt if needed
    if (config.crypto_config) {
        let_ref conf = config.crypto_config;
        let cipher = encrypt({as_c_str(bytes.data()), bytes.size()}, conf->key_, conf->iv_);
        if (!cipher.has_value()) {
            spdlog::error("file encryption failed: {}", config.archive_path.c_str());
            return Status::ERROR;
        }
        bytes = {cipher->data(), cipher->size()};
        spdlog::info("file encryption succeeded : {}", config.archive_path.c_str());
    }

    // output to target file
    {
        let_mut ofs = fs::open_or_create_file_as_ofs(config.archive_path.c_str(), fs::ios::binary);

        let_mut header = PodHeader::from(config);
        calculate_checksum(header.checksum, {(p_byte) bytes.data(), bytes.size()});
        if (config.crypto_config) {
            calculate_password_verify_token(header, config.crypto_config->key_);
        }
        ofs.write(as_c_str(header), sizeof(header));
        ofs << bytes;
    }

    return Status::OK;
}

Status zpods::backup(const char *target_dir, const BackupConfig &config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    // we must have something to back up!
    if (config.filter.paths.empty()) {
        return Status::EMPTY;
    }

    // what we back up must really exist!
    for (const auto &item: config.filter.paths) {
        if (!fs::exists(item.c_str())) {
            return Status::PATH_NOT_EXIST;
        }
    }

    for (const auto &dir_to_backup: config.filter.paths) {
        let pods_dir = fs::path(target_dir) / fmt::format("{}-PODS", dir_to_backup.c_str());

        if (!fs::exists(pods_dir.c_str())) {
            PodsManager::Instance()->create_pods(pods_dir);
        }
        PodsManager::Instance()->record_mapping(dir_to_backup, pods_dir);

        config.dir_to_backup = dir_to_backup;
        let st = backup_one(pods_dir.c_str(), config);
        if (st != Status::OK) {
            return st;
        }
    }

    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_dir, BackupConfig config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    return zpods::process_origin_zpods_bytes(src_path, config, [target_dir](auto &bytes) {
        return zpods::unarchive({(p_byte) bytes.data(), bytes.size()}, target_dir);
    });
}

Status zpods::sync_backup(const char *target_dir, const BackupConfig &config) {
    // make sure the backup file is up-to-date
    zpods::backup(target_dir, config);

    // backup callback if the src_path is modified
    let callback = [&](const char *path) {
        zpods::backup(target_dir, config);
    };

    // create a watcher in a thread for each path
    std::vector<std::thread> threads;
    threads.reserve(config.filter.paths.size());
    for (let_ref path: config.filter.paths) {
        threads.emplace_back([=] {
            FsWatcher watcher(path.c_str(), {
                    .on_file_create = callback,
                    .on_file_delete = callback,
                    .on_file_modify = callback,
                    .on_dir_create = callback,
                    .on_dir_delete = callback,
            });
        });
    }
    for (let_mut_ref item: threads) { item.join(); }

    return Status::OK;
}
