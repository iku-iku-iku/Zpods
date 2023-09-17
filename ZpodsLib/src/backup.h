#include "enum.h"

namespace zpods {
    ///
    /// \param src_path is the path to backup
    /// \param target_dir is the path to put the backup file
    /// \return Status::OK if succeeded
    Status backup(const char* src_path, const char* target_dir);
    Status restore(const char* src_path, const char* target_path);
}