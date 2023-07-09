#pragma once

#define CEREAL_SERIALIZE_FUNCTION_NAME Serialize
#define CEREAL_LOAD_FUNCTION_NAME Load
#define CEREAL_SAVE_FUNCTION_NAME Save
#define CEREAL_LOAD_MINIMAL_FUNCTION_NAME LoadMinimal
#define CEREAL_SAVE_MINIMAL_FUNCTION_NAME SaveMinimal

#include <cereal/macros.hpp>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <filesystem>

namespace cereal
{
    template <typename Archive>
    void Serialize(Archive &outArchive, glm::vec2 &outVec)
    {
        outArchive(outVec.x, outVec.y);
    }

    template <typename Archive>
    void Serialize(Archive &outArchive, glm::vec3 &outVec)
    {
        outArchive(outVec.x, outVec.y, outVec.z);
    }

    template <typename Archive>
    void Serialize(Archive &outArchive, glm::vec4 &outVec)
    {
        outArchive(outVec.x, outVec.y, outVec.z, outVec.w);
    }

    template <typename Archive>
    void Serialize(Archive &outArchive, glm::quat &outQuat)
    {
        outArchive(outQuat.x, outQuat.y, outQuat.z, outQuat.w);
    }
}