#include "spline.h"


// -----------------------  Spline ---------------------------------

/// Checks whether vector is zero-length or not.
bool isVectorNull(const glm::vec3& vect) {

    return !vect.x && !vect.y && !vect.z;
}

glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {

    glm::vec3 z = -glm::normalize(front);

    if (isVectorNull(z))
        z = glm::vec3(0.0, 0.0, 1.0);

    glm::vec3 x = glm::normalize(glm::cross(up, z));

    if (isVectorNull(x))
        x = glm::vec3(1.0, 0.0, 0.0);

    glm::vec3 y = glm::cross(z, x);
    //mat4 matrix = mat4(1.0f);
    glm::mat4 matrix = glm::mat4(
        x.x, x.y, x.z, 0.0,
        y.x, y.y, y.z, 0.0,
        z.x, z.y, z.z, 0.0,
        position.x, position.y, position.z, 1.0
    );

    return matrix;
}