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
    try {
        app.parse(argc, argv /* only parse up to first non-option argument */);
    } catch (const CLI::CallForHelp &e) {
        // Handle help call here if necessary
    } catch (const CLI::ParseError &e) {
        std::cout << "A subcommand is required." << std::endl;
        std::cout << "Run with --help for more information." << std::endl;
        return 1;
    }

    std::string src_path;
    std::string target_dir;
    {
        app.add_option("-s,--src", src_path, "source path")->required();
        app.add_option("-t,--target", target_dir, "target directory")->required();
    }

    backup->fallthrough();
    restore->fallthrough();

    CLI11_PARSE(app, argc, argv);

    if (*backup) {
        zpods::backup(src_path.c_str(), target_dir.c_str());
    }

    if (*restore) {
        zpods::restore(src_path.c_str(), target_dir.c_str());
    }
    return 0;
}