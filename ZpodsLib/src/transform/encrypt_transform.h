#pragma once

#include "base/crypto.h"
#include "transform.h"

namespace zpods
{
class EncryptTransform : public Transform
{
  public:
    EncryptTransform(std::string_view key, std::string_view iv)
        : key_(key), iv_(iv)
    {
    }
    auto execute(std::string&& bytes) -> Transform::ResType override
    {
        auto ciphertext = zpods::encrypt(bytes, key_, iv_);
        if (!ciphertext)
        {
            spdlog::error("encrypt failed!");
            return {zpods::Status::ERROR, {}};
        }
        return {zpods::Status::OK, std::move(*ciphertext)};
    }

  private:
    std::string key_;
    std::string iv_;
};
} // namespace zpods
