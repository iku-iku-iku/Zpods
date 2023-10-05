//
// Created by code4love on 23-9-24.
//

#include "fs.h"

bool zpods::fs::is_same_content(const char *path1, const char *path2) {
    if (!exists(path1)) {
        return false;
    }
    if (!exists(path2)) {
        return false;
    }
    let_mut ifs1 = std::ifstream(path1, ios::binary);
    let_mut ifs2 = std::ifstream(path2, ios::binary);

    let_mut size1 = get_file_size(path1);
    let_mut size2 = get_file_size(path2);

    if (size1 != size2) {
        return false;
    }

    std::vector<char> buf1(size1);
    std::vector<char> buf2(size2);

    ifs1.read(buf1.data(), size1);
    ifs2.read(buf2.data(), size2);

    return memcmp(buf1.data(), buf2.data(), size1) == 0;
}

auto zpods::fs::relative(const char *path, const char *base) -> const char * {
    let_mut len = strlen(base);
    if (base[len - 1] == '/') {
        len--;
    }

    ZPODS_ASSERT(is_directory(base));

//            if (strlen(path) == len && !is_directory(base)) {
//                let_mut ret = path;
//                while (*path) {
//                    if (*path == '/') ret = path + 1;
//                    path++;
//                }
//                return ret;
//            }

    if (strncmp(path, base, len) == 0) {
        return path + len + 1;
    }
    return nullptr;
}

auto zpods::fs::write(const char *path, std::string_view buf) {
    std::ofstream ofs(path);
    ZPODS_ASSERT(ofs.is_open());
    ofs.write(buf.data(), (long) buf.length());
}

auto zpods::fs::read(const char *path, std::string_view buf) {
    std::ifstream ifs(path);
    ZPODS_ASSERT(ifs.is_open());
    ifs.read(const_cast<char *>(buf.data()), (long) buf.length());
}

auto zpods::fs::open_or_create_file_as_ifs(const char *path, zpods::fs::openmode mode) -> std::ifstream {
    let base = get_base_name(path);
    if (!exists(base.c_str())) {
        create_directory_if_not_exist(base.c_str());
    }
    std::ifstream ifs(path, mode);
    ZPODS_ASSERT(ifs.is_open());
    return ifs;
}

auto zpods::fs::open_or_create_file_as_ofs(const char *path, zpods::fs::openmode mode) -> std::ofstream {
    let base = get_base_name(path);
    if (!exists(base.c_str())) {
        create_directory_if_not_exist(base.c_str());
    }
    std::ofstream ofs(path, mode);
    ZPODS_ASSERT(ofs.is_open());
    return ofs;
}

auto zpods::fs::get_base_name(const char *path) -> std::string {
    size_t len = 0;
    let_mut p = path;
    while (*p) {
        if (*p == '/') {
            len = p - path;
        }
        p++;
    }
    return {path, len};
}

auto zpods::fs::read_from_file(const char *path) -> std::string {
    ZPODS_ASSERT(exists(path));
    let_mut ifs = open_or_create_file_as_ifs(path, ios::binary);
    let_mut size = get_file_size(path);
    std::vector<char> buf(size);
    ifs.read(buf.data(), (long) size);
    return {buf.begin(), buf.end()};
}
