#pragma once
#include "transform.h"

#include "archive.h"

namespace zpods
{
class ArchiveTransform : public Transform
{
  public:
    ArchiveTransform(const char* src_dir, const char* dest_dir,
                     const BackupConfig& config)
        : src_dir_(src_dir), dest_dir_(dest_dir), config_(config)
    {
    }

    auto execute(std::string&& bytes) -> Transform::ResType override
    {
        let_mut[status, buf] =
            make_archive(src_dir_.c_str(), dest_dir_.c_str(), config_);
        if (status == Status::NO_NEW_TO_ARCHIVE)
        {
            spdlog::info("no new pea to archive");
            status = Status::OK;
        }
        return {status, std::move(buf)};
    }

  private:
    std::string src_dir_;
    std::string dest_dir_;
    const BackupConfig& config_;
};

} // namespace zpods
