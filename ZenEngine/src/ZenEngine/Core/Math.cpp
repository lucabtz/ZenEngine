#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace ZenEngine
{

    void Math::DecomposeMatrix(const glm::mat4 &inMatrix, glm::vec3 &outTranslation, glm::vec3 &outRotation, glm::vec3 &outScale)
    {
        outScale.x = glm::length(inMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
        outScale.y = glm::length(inMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
        outScale.z = glm::length(inMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

        glm::mat4 orthonormal = glm::scale(glm::mat4(1.0f), { 1.0f / outScale.x, 1.0f / outScale.y, 1.0f / outScale.z }) 
            * inMatrix * glm::scale(glm::mat4(1.0f), outScale);

        outRotation.x = atan2f(orthonormal[1][2], orthonormal[2][2]);
        outRotation.y = atan2f(-orthonormal[0][2], sqrtf(orthonormal[1][2] * orthonormal[1][2] + orthonormal[2][2] * orthonormal[2][2]));
        outRotation.z = atan2f(orthonormal[0][1], orthonormal[0][0]);

        outTranslation.x = orthonormal[3].x;
        outTranslation.y = orthonormal[3].y;
        outTranslation.z = orthonormal[3].z;
    }

    void Math::DecomposeMatrix(const glm::mat4 & inMatrix, glm::vec3 & outTranslation, glm::quat & outRotation, glm::vec3 & outScale)
    {
        outScale.x = glm::length(inMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
        outScale.y = glm::length(inMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
        outScale.z = glm::length(inMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

        glm::mat4 orthonormal = glm::scale(glm::mat4(1.0f), { 1.0f / outScale.x, 1.0f / outScale.y, 1.0f / outScale.z }) 
            * inMatrix * glm::scale(glm::mat4(1.0f), outScale);

        outRotation = glm::toQuat(glm::mat3(orthonormal));

        outTranslation.x = orthonormal[3].x;
        outTranslation.y = orthonormal[3].y;
        outTranslation.z = orthonormal[3].z;
    }
}