#pragma once

#ifndef __SPLINE_H
#define __SPLINE_H

#include "pgr.h"
#include <iostream>

extern glm::vec3 curveData[];
extern const size_t  curveSize;

extern glm::vec3 curveDataCamera[];
extern const size_t  curveSizeCamera;



bool isVectorNull(const glm::vec3& vect);
glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);

glm::vec3 evaluateCurveSegment(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3, const float t);
glm::vec3 evaluateClosedCurve(const glm::vec3 points[], const size_t count, const float t);

glm::vec3 evaluateCurveSegment_1stDerivative(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3, const float t);
glm::vec3 evaluateClosedCurve_1stDerivative(const glm::vec3 points[], const size_t count, const float t);


void testSpline(glm::vec3* curveTestPoints, glm::vec3* curveTestGoldfile, glm::vec3* curveTestGoldfile_1stDerivative);

extern glm::vec3 curveTestPoints[];
extern glm::vec3 curveTestGoldfile[];
extern glm::vec3 curveTestGoldfile_1stDerivative[];

#endif // __SPLINE_H