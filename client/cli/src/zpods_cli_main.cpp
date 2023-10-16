//
// Created by code4love on 23-9-30.
//

#include <network.h>
#include "pch.h"
#include "CLI11.hpp"
#include "zpods_lib.h"
#include "termios.h"

std::string get_password() {
    std::cout << "Password: ";

    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::string password;
    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    std::cout << std::endl;
    return password;
}

std::string get_username() {
    std::cout << "Username: ";
    std::string username;
    std::getline(std::cin, username);
    return username;
}

int main(int argc, char **argv) {
    CLI::App app{"zpods client"};
    app.require_subcommand(1);

    CLI::App *register_ = app.add_subcommand("register", "register a user");
    CLI::App *backup = app.add_subcommand("backup", "backup a directory (or a file) to a single file");
    CLI::App *restore = app.add_subcommand("restore", "restore a archive file to a directory");

    std::string src_path;
    std::string target_dir;
    std::string password;
    zpods::BackupConfig config;
    zpods::User user;

    // register
    register_->callback([&] {
        user.username = get_username();
        user.password = get_password();
        let status = user.register_();
        if (status == zpods::Status::USER_ALREADY_EXISTS) {
            spdlog::info("user already exists!");
        } else if (status == zpods::Status::OK) {
            spdlog::info("register succeeded!");
        } else {
            spdlog::info("register failed!");
        }
    });

    // backup
    backup->add_option("-s,--src", src_path, "source path")->required();
    backup->add_option("-t,--target", target_dir, "target directory")->required();

    let compress = backup->add_flag("-c,--compress", "compress the backup file");
    let sync = backup->add_flag("-y,--sync", "synchronously backup the src_path to target_dir");
    let remote = backup->add_flag("-r,--remote", "backup to remote server");
    let encrypt_password = backup->add_option("-p,--password", password, "password for encryption/decryption");

    backup->callback([&] {
        if (*remote) {
            user.username = get_username();
            user.password = get_password();
            let status = user.login();
            switch (status) {
                case zpods::Status::WRONG_PASSWORD:
                    spdlog::info("wrong password!");
                    return;
                case zpods::Status::USER_NOT_EXISTS:
                    spdlog::info("user not exist");
                    return;
                case zpods::Status::ERROR:
                    spdlog::info("network error");
                    return;
                case zpods::Status::OK:
                    spdlog::info("login succeeded");
                    break;
                default:
                    spdlog::info("unknown error");
            }
        }
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
        let backup_file_path = fmt::format("{}/{}", target_dir.c_str(), config.backup_filename->c_str());

        if (*remote) {
            let status = user.upload_file(backup_file_path.c_str());
            if (status == zpods::Status::OK) {
                spdlog::info("upload successfully!");
            } else {
                spdlog::info("fail to upload");
            }
        }
    });

    // restore
    restore->add_option("-s,--src", src_path, "source path")->required();

    restore->add_option("-t,--target", target_dir, "target directory")->required();

    let decrypt_password = restore->add_option("-p,--password", password, "password for encryption/decryption");

    restore->callback([&] {
        if (*decrypt_password) {
            config.
                    crypto_config = zpods::CryptoConfig(password);
        }
        let status = zpods::restore(src_path.c_str(), target_dir.c_str(), config);

        switch (status) {
            case zpods::Status::OK:
                spdlog::info("restore succeeded");
                break;
            case zpods::Status::NOT_ZPODS_FILE:
                spdlog::error("restore failed: not a zpods file");
                break;
            case zpods::Status::ERROR:
                spdlog::error("restore failed: unknown error");
                break;
            case zpods::Status::PASSWORD_NEEDED:
                spdlog::error("restore failed: password needed");
                break;
            case zpods::Status::WRONG_PASSWORD:
                spdlog::error("restore failed: wrong password");
                break;
            case zpods::Status::CHECKSUM_ERROR:
                spdlog::error("restore failed: checksum error");
                break;
            default:
                spdlog::error("restore failed: unknown error");
        }
    });

    CLI11_PARSE(app, argc, argv);
    return 0;
}