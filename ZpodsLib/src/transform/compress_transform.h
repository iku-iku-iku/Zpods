#pragma once
#include "compress.h"
#include "transform.h"

namespace zpods
{
class CompressTransform : public Transform
{
  public:
    auto execute(std::string&& bytes) -> Transform::ResType override
    {
        let[size, ptr] = zpods::compress({(byte*)bytes.data(), bytes.size()});
        return {Status::OK, std::string{(const char*)ptr.get(), size}};
    }

  private:
}; // namespace zpods
} // namespace zpods
