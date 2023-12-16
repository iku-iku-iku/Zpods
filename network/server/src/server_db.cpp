#include "ZpodsLib/src/base/fs.h"
#include "server_db.h"

zpods::DbHandle::DbHandle()
{
    rocksdb::Options options;
    options.create_if_missing = true;
    zpods::fs::create_directory_if_not_exist(ZPODS_STORAGE);
    rocksdb::Status status =
        rocksdb::DB::Open(options, ZPODS_STORAGE "/zpods_server_db", &db);
    assert(status.ok());
}
