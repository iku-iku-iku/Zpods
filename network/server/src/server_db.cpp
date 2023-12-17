#include "ZpodsLib/src/base/fs.h"
#include "server_db.h"

zpods::DbHandle::DbHandle()
{
    rocksdb::Options options;
    options.create_if_missing = true;
    zpods::fs::create_directory_if_not_exist(get_zpods_storage_path().c_str());
    rocksdb::Status status =
        rocksdb::DB::Open(options, MAKE_STORE_PATH("zpods_server_db"), &db);
    assert(status.ok());
}
