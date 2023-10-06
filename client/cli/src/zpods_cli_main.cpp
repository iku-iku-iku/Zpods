//
// Created by code4love on 23-9-30.
//

#include "pch.h"
#include "CLI11.hpp"
#include "zpods_lib.h"

int main(int argc, char **argv) {
    CLI::App app{"zpods client"};
    app.require_subcommand(1);

    CLI::App *backup = app.add_subcommand("backup", "backup a directory (or a file) to a single file");
    CLI::App *restore = app.add_subcommand("restore", "restore a archive file to a directory");
//    app.parse(argc, argv);
//    try {
//        app.parse(argc, argv /* only parse up to first non-option argument */);
//    } catch (const CLI::CallForHelp &e) {
//        // Handle help call here if necessary
//    } catch (const CLI::ParseError &e) {
//        std::cout << "A subcommand is required." << std::endl;
//        std::cout << "Run with --help for more information." << std::endl;
//        return 1;
//    }

    std::string src_path;
    std::string target_dir;
    std::string password;

    zpods::BackupConfig config;

    // backup
    backup->add_option("-s,--src", src_path, "source path")->required();
    backup->add_option("-t,--target", target_dir, "target directory")->required();

    let compress = backup->add_flag("-c,--compress", "compress the backup file");
    let sync = backup->add_flag("-y,--sync", "synchronously backup the src_path to target_dir");
    let encrypt_password = backup->add_option("-p,--password", password, "password for encryption/decryption");

    backup->callback([&] {
        if (*encrypt_password) {
            config.crypto_config = zpods::CryptoConfig(password);
        }
        if (*compress) {
            config.compress = true;
        }
        if (*sync) {
            zpods::sync_backup(src_path.c_str(), target_dir.c_str(), config);
        } else {
            zpods::backup(src_path.c_str(), target_dir.c_str(), config);
        }
    });

    // restore
    restore->add_option("-s,--src", src_path, "source path")->required();
    restore->add_option("-t,--target", target_dir, "target directory")->required();
    let decrypt_password = restore->add_option("-p,--password", password, "password for encryption/decryption");

    restore->callback([&] {
        if (*decrypt_password) {
            config.crypto_config = zpods::CryptoConfig(password);
        }
        zpods::restore(src_path.c_str(), target_dir.c_str(), config);
    });

    CLI11_PARSE(app, argc, argv);

//    if (backup->parsed()) {
//        if (*compress) {
//            config.compress = true;
//        }
//        zpods::backup(src_path.c_str(), target_dir.c_str(), config);
//    } else if (restore->parsed()) {
//        zpods::restore(src_path.c_str(), target_dir.c_str(), config);
//    } else {
//        std::cout << "You must specify a subcommand" << std::endl;
//        std::cout << "Run with --help for more information." << std::endl;
//        return 1;
//    }
    return 0;
}