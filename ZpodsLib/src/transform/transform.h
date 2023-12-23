#pragma once

#include "pch.h"

namespace zpods
{

class Transform
{
  public:
    using ResType = std::pair<zpods::Status, std::string>;
    virtual auto execute(std::string&& bytes) -> ResType = 0;
    virtual ~Transform() {}
};
} // namespace zpods
