#include "spline.h"

/// Checks whether vector is zero-length or not.
bool isVectorNull(const glm::vec3& vect) {

    return !vect.x && !vect.y && !vect.z;
}

/**
 * @brief Aligns an object in 3D space based on its position and orientation.
 *
 * @param position The position of the object.
 * @param front The front direction of the object.
 * @param up The up direction of the object.
 * @return A 4x4 transformation matrix aligning the object.
 */
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


// -----------------------  Spline ---------------------------------

/**
 * @brief Evaluates a point on a curve segment defined by four control points using Catmull-Rom spline.
 *
 * @param P0 First control point.
 * @param P1 Second control point.
 * @param P2 Third control point.
 * @param P3 Fourth control point.
 * @param t The parameter t, ranging from 0 to 1, to evaluate the curve at.
 * @return The point on the curve segment at parameter t.
 */
glm::vec3 evaluateCurveSegment(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3, const float t) {
    glm::vec3 result(0.0, 0.0, 0.0);

    // evaluate point on a curve segment (defined by the control points P0...P3)
    // for the given value of parameter t
    const auto t0 = 1;
    const auto t1 = t0 * t;
    const auto t2 = t1 * t;
    const auto t3 = t2 * t;

    const auto B0 = 0.5f * ((-1) * t3 + 2 * t2 + (-1) * t1 + 0 * t0);
    const auto B1 = 0.5f * (3 * t3 + (-5) * t2 + 0 * t1 + 2 * t0);
    const auto B2 = 0.5f * ((-3) * t3 + 4 * t2 + 1 * t1 + 0 * t0);
    const auto B3 = 0.5f * (1 * t3 + (-1) * t2 + 0 * t1 + 0 * t0);

    result = B0 * P0 + B1 * P1 + B2 * P2 + B3 * P3;

    return result;
}

/**
 * @brief Evaluates a point on a closed curve defined by an array of control points using Catmull-Rom spline.
 *
 * @param points An array of control points defining the curve.
 * @param count The number of control points.
 * @param t The parameter t, ranging from 0 to 1, to evaluate the curve at.
 * @return The point on the closed curve at parameter t.
 */
glm::vec3 evaluateClosedCurve( const glm::vec3 points[], const size_t count, const float t) {
    glm::vec3 result(0.0, 0.0, 0.0);

    // based on the value of parameter t first find corresponding segment and its control points => i
    // and then call evaluateCurveSegment function with proper parameters to get a point on a closed curve

    size_t i = t;

    result = evaluateCurveSegment(
        points[((i - 1) + count) % count],
        points[(i + 0) % count],
        points[(i + 1) % count],
        points[(i + 2) % count],
        t - i
    );

    return result;
}


/**
 * @brief Evaluates the first derivative of a curve segment at a given parameter t. (Catmull-Rom spline)
 *
 * @param P0 First control point.
 * @param P1 Second control point.
 * @param P2 Third control point.
 * @param P3 Fourth control point.
 * @param t The parameter t, ranging from 0 to 1, to evaluate the derivative at.
 * @return The first derivative of the curve segment at parameter t.
 */
glm::vec3 evaluateCurveSegment_1stDerivative(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3, const float t) {
    glm::vec3 result(1.0, 0.0, 0.0);

    // evaluate first derivative for a point on a curve segment (defined by the control points P0...P3)
    // for the given value of parameter t
    const auto t0 = 0;
    const auto t1 = 1;
    const auto t2 = 2 * t;
    const auto t3 = 3 * t * t;

    const auto B0 = 0.5f * ((-1) * t3 + 2 * t2 + (-1) * t1 + 0 * t0);
    const auto B1 = 0.5f * (3 * t3 + (-5) * t2 + 0 * t1 + 2 * t0);
    const auto B2 = 0.5f * ((-3) * t3 + 4 * t2 + 1 * t1 + 0 * t0);
    const auto B3 = 0.5f * (1 * t3 + (-1) * t2 + 0 * t1 + 0 * t0);

    result = B0 * P0 + B1 * P1 + B2 * P2 + B3 * P3;

    return result;
}


/**
 * @brief Evaluates the first derivative of a closed curve at a given parameter t. (Catmull-Rom spline)
 *
 * @param points An array of control points defining the curve.
 * @param count The number of control points.
 * @param t The parameter t, ranging from 0 to 1, to evaluate the derivative at.
 * @return The first derivative of the closed curve at parameter t.
 */
glm::vec3 evaluateClosedCurve_1stDerivative(const glm::vec3 points[], const size_t count, const float t) {
    glm::vec3 result(0.0, 0.0, 0.0);

    size_t i = t;

    result = evaluateCurveSegment_1stDerivative(
        points[((i - 1) + count) % count],
        points[(i + 0) % count],
        points[(i + 1) % count],
        points[(i + 2) % count],
        t - i
    );

    return result;
}


/// Number of control points of the animation curve.
const size_t curveSize = 12;

/// Control points of the animation curve.
glm::vec3 curveData[] = {
  glm::vec3(0.0, 0.0,  0.0),

  glm::vec3(-0.33,  0.35, 0.0),
  glm::vec3(-0.66,  0.5, 0.0),
  glm::vec3(-0.85,  0.0, 0.0),
  glm::vec3(-0.66, -0.35, 0.0),
  glm::vec3(-0.33, -0.35, 0.0),

  glm::vec3(0.0,  0.0, 0.0),

  glm::vec3(0.33,  0.35, 0.0),
  glm::vec3(0.66,  0.35, 0.0),
  glm::vec3(0.85,  0.0, 0.0),
  glm::vec3(0.66, -0.35, 0.0),
  glm::vec3(0.33, -0.35, 0.0)
};