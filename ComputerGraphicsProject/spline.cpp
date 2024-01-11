/*
* \file spline.cpp
* \author Valentin Lhermitte
* \date 2023-2024
* \brief Spline calculation
*/

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




/// Control points of the animation curve. (Foxbat)
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
/// Number of control points of the animation curve. (Foxbat)
const size_t curveSize = sizeof(curveData) / sizeof(glm::vec3);


/// Control points of the animation curve. (Camera)
glm::vec3 curveDataCamera[] = {
     glm::vec3(-0.7f, 0.0f, 0.1f),
     glm::vec3(-0.35f, -0.35f, 0.1f),
     glm::vec3(0.0f, -0.7f, 0.1f),
     glm::vec3(0.35f, -0.35f, 0.1f),
     glm::vec3(0.7f, 0.0f, 0.1f),
     glm::vec3(0.35f, 0.35f, 0.1f),
     glm::vec3(0.0f, 0.7f, 0.1f),
     glm::vec3(-0.35f, 0.35f, 0.1f)
};
/// Number of control points of the animation curve. (Camera)
const size_t curveSizeCamera = sizeof(curveDataCamera) / sizeof(glm::vec3);

//**************************************************************************************************
/// Curve validity test points.
glm::vec3 curveTestPoints[] = {
  glm::vec3(-9.0f, 0.0f, -5.0f),
  glm::vec3(-3.0f, 0.0f,  5.0f),
  glm::vec3(3.0f, 4.0f, -5.0f),
  glm::vec3(9.0f, 0.0f,  0.0f),
};

/// Correct result for curve position in range [0, 1] with step 0.05.
glm::vec3 curveTestGoldfile[] = {
  glm::vec3(-3.000000, 0.000000,  5.000000),
  glm::vec3(-2.700000, 0.119250,  4.933437),
  glm::vec3(-2.400000, 0.274000,  4.742500),
  glm::vec3(-2.100000, 0.459750,  4.440312),
  glm::vec3(-1.800000, 0.672000,  4.040000),
  glm::vec3(-1.500000, 0.906250,  3.554688),
  glm::vec3(-1.200000, 1.158000,  2.997500),
  glm::vec3(-0.900000, 1.422750,  2.381562),
  glm::vec3(-0.600000, 1.696000,  1.720000),
  glm::vec3(-0.300000, 1.973250,  1.025937),
  glm::vec3(0.000000, 2.250000,  0.312500),
  glm::vec3(0.300000, 2.521750, -0.407188),
  glm::vec3(0.600000, 2.784000, -1.120000),
  glm::vec3(0.900000, 3.032250, -1.812812),
  glm::vec3(1.200000, 3.262000, -2.472499),
  glm::vec3(1.500000, 3.468750, -3.085938),
  glm::vec3(1.800000, 3.648000, -3.640001),
  glm::vec3(2.100000, 3.795250, -4.121563),
  glm::vec3(2.400000, 3.906000, -4.517500),
  glm::vec3(2.700000, 3.975750, -4.814687),
  glm::vec3(3.000000, 4.000000, -5.000000)
};
/// Correct result for curve 1st derivative in range [0, 1] with step 0.05.
glm::vec3 curveTestGoldfile_1stDerivative[] = {
  glm::vec3(6.000000, 2.000000,   0.000000),
  glm::vec3(6.000000, 2.755000, -2.618750),
  glm::vec3(6.000000, 3.420000, -4.975000),
  glm::vec3(6.000000, 3.995000, -7.068750),
  glm::vec3(6.000000, 4.480000, -8.900000),
  glm::vec3(6.000000, 4.875000, -10.468750),
  glm::vec3(6.000000, 5.180000, -11.775001),
  glm::vec3(6.000000, 5.395000, -12.818750),
  glm::vec3(6.000000, 5.520000, -13.599999),
  glm::vec3(6.000000, 5.555000, -14.118751),
  glm::vec3(6.000000, 5.500000, -14.375000),
  glm::vec3(6.000000, 5.355000, -14.368751),
  glm::vec3(6.000000, 5.120000, -14.100000),
  glm::vec3(6.000000, 4.795001, -13.568751),
  glm::vec3(6.000000, 4.380000, -12.775001),
  glm::vec3(6.000000, 3.875000, -11.718750),
  glm::vec3(6.000000, 3.279999, -10.399999),
  glm::vec3(6.000000, 2.595000, -8.818749),
  glm::vec3(6.000000, 1.820001, -6.975001),
  glm::vec3(6.000000, 0.955000, -4.868751),
  glm::vec3(6.000000, 0.000000, -2.500000)
};



void testSpline(glm::vec3 *curveTestPoints, glm::vec3 *curveTestGoldfile, glm::vec3 *curveTestGoldfile_1stDerivative) {
    bool curveValid;
    bool curve1stDerivativeValid;

    const float marginOfError = 1e-5f;
    int numTest = 21;
    for (int i = 0; i < numTest; i++) {
        float t = i / 20.f;

        glm::vec3 computedPoint = evaluateCurveSegment(
            curveTestPoints[0],
            curveTestPoints[1],
            curveTestPoints[2],
            curveTestPoints[3],
            t
        );

        glm::vec3 computedPoint_1stDerivative = evaluateCurveSegment_1stDerivative(
            curveTestPoints[0],
            curveTestPoints[1],
            curveTestPoints[2],
            curveTestPoints[3],
            t
        );

        curveValid = glm::distance(curveTestGoldfile[i], computedPoint) < marginOfError;
        curve1stDerivativeValid = glm::distance(curveTestGoldfile_1stDerivative[i], computedPoint_1stDerivative) < marginOfError;

        if (!curveValid || !curveValid)
            // As soon as we fail a test we stop
            break;
    }
    std::cout << "Curve test: " << (curveValid ? "OK" : "FAIL") << std::endl;
    std::cout << "Curve 1st derivative test: " << (curve1stDerivativeValid ? "OK" : "FAIL") << std::endl;
}