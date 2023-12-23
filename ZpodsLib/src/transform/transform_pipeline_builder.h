#pragma once

#include "transform/archive_transform.h"
#include "transform/compress_transform.h"
#include "transform/encrypt_transform.h"
#include "transform_pipeline.h"

namespace zpods
{
class TransformPipelineBuilder
{
  public:
    TransformPipelineBuilder()
        : pipeline_(std::make_unique<TransformPipeline>())
    {
    }

    TransformPipelineBuilder& enable_archive(const char* src_dir,
                                             const char* dest_dir,
                                             const BackupConfig& config)
    {
        pipeline_->add_transform<ArchiveTransform>(src_dir, dest_dir, config);
        return *this;
    }
    TransformPipelineBuilder& enable_compress()
    {
        pipeline_->add_transform<CompressTransform>();
        return *this;
    }
    TransformPipelineBuilder& enable_encrypt(std::string_view key,
                                             std::string_view iv)
    {
        pipeline_->add_transform<EncryptTransform>(key, iv);
        return *this;
    }

    auto build() -> std::unique_ptr<TransformPipeline>
    {
        return std::move(pipeline_);
    }

  private:
    std::unique_ptr<TransformPipeline> pipeline_;
};
} // namespace zpods
