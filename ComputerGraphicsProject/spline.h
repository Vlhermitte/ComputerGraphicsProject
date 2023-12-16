#pragma once

#ifndef __SPLINE_H
#define __SPLINE_H

#include "pgr.h"

bool isVectorNull(const glm::vec3& vect);
glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);


#endif // __SPLINE_H