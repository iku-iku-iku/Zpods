#pragma once

#include "transform.h"

namespace zpods
{

class TransformPipeline : public Transform
{
  public:
    auto execute(std::string&& _bytes) -> Transform::ResType override
    {
        Status status;
        std::string bytes = std::move(_bytes);
        for (auto& transform : transforms_)
        {
            std::tie(status, bytes) = transform->execute(std::move(bytes));
            if (status != Status::OK)
            {
                return {status, {}};
            }
        }
        return {Status::OK, std::move(bytes)};
    }

    template <typename TransType, typename... Param>
        requires std::derived_from<TransType, Transform>
    void add_transform(Param&&... param)
    {
        transforms_.push_back(new TransType(std::forward<Param>(param)...));
    }

    ~TransformPipeline() override
    {
        for (auto& transform : transforms_)
        {
            delete transform;
            transform = nullptr;
        }
    }

  private:
    std::vector<Transform*> transforms_;
};
} // namespace zpods
