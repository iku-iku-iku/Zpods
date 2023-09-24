//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "archive.h"

TEST(ArchiveTest, test_archive_and_unarchive)
{
    zpods::archive(fmt::format("{}/tiny", zpods::test_data_path()).c_str() ,
                   zpods::temp_path());

//    zpods::archive(fmt::format("{}/single/man_pthreads.txt", zpods::test_data_path()).c_str() ,
//                   zpods::temp_path());

    zpods::unarchive(fmt::format("{}/archive", zpods::temp_path()).c_str() ,
                   zpods::temp_path());
}
