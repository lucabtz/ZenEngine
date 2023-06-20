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

namespace cereal
{
    template <typename Archive>
    void Serialize(Archive &inArchive, glm::vec2 &inVec)
    {
        inArchive(inVec.x, inVec.y);
    }

    template <typename Archive>
    void Serialize(Archive &inArchive, glm::vec3 &inVec)
    {
        inArchive(inVec.x, inVec.y, inVec.z);
    }

    template <typename Archive>
    void Serialize(Archive &inArchive, glm::vec4 &inVec)
    {
        inArchive(inVec.x, inVec.y, inVec.z, inVec.w);
    }
}