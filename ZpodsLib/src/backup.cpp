#include "backup.h"
#include "compress.h"
#include "archive.h"
#include "fs.h"

using namespace zpods;

using namespace zpods::fs;

Status zpods::backup(const char *src_path, const char *target_dir, const BackupConfig &config) {
    // check src exist
    let src = fs::path(src_path);
    let target = fs::path(target_dir);

    if (!fs::exists(src.c_str())) {
        return Status::ERROR;
    }

    // 1. archive files of src_path to a single file in target_dir
    zpods::archive(src_path, target_dir);
    let archive_path = fmt::format("{}/archive", target_dir);

    // 2. compress if needed
    compress_file(archive_path.c_str(), archive_path.c_str());
//    if (config.compress) {
//        let src_size = fs::get_file_size(archive_path);
//
//        std::vector<byte> buf(src_size);
//        fs::read(archive_path.c_str(), {reinterpret_cast<const char *>(buf.data()), buf.size()});
//
//        let [dest_size, dest] = zpods::compress(std::span(buf.data(), buf.size()));
//
//        spdlog::info("SIZE BEFORE: {} AFTER: {}", src_size, dest_size);
//        fs::write(archive_path.c_str(), {reinterpret_cast<const char *>(dest.get()), dest_size});
//    }
//
    // TODO: add header for metadata

//    let_mut backup = target / src.filename();
//    backup.replace_extension(".pods");

//    if (fs::exists(backup.c_str())) {
//        fs::remove_all(backup.c_str());
//    }

//    auto options = fs::copy_options::recursive | fs::copy_options::skip_symlinks;

//    fs::copy(archive_path.c_str(), backup.c_str(), options);
    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_dir) {
//    let src_size = fs::get_file_size(src_path);
    decompress_file(src_path, src_path);

//    std::vector<char> src(src_size);

//    fs::read(src_path, {src.data(), src.size()});
//
//    let [decompressed_size, decompressed] = zpods::decompress(reinterpret_cast<p_cbyte>(src.data()));
//    fs::write(src_path, {reinterpret_cast<const char *>(decompressed.get()), decompressed_size});
//
    zpods::unarchive(src_path, target_dir);

    return Status::OK;
}
