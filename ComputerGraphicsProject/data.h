#pragma once

#ifndef __DATA_H
#define __DATA_H

#include "pgr.h"

#define SCENE_WIDTH  1.0f
#define SCENE_HEIGHT 1.0f
#define SCENE_DEPTH  1.0f

#define PLAYER_SIZE   0.05f
#define TERRAIN_SIZE  1.0f
#define FLOOR_SIZE	1.0f

#define CAMERA_ELEVATION_MAX 70.0f

#define PLAYER_SPEED_INCREMENT 0.2f
#define PLAYER_UP_SPEED_INCREMENT 0.1f
#define PLAYER_SPEED_MAX 0.4f
#define PLAYER_SPEED_MIN -0.2f
#define PLAYER_VIEW_ANGLE_DELTA 2.0f

#define MAX_HEIGHT 0.8f
#define MIN_HEIGHT -0.2f


enum { 
	KEY_LEFT_ARROW, 
	KEY_RIGHT_ARROW, 
	KEY_UP_ARROW, 
	KEY_DOWN_ARROW, 
	KEY_SPACE,
	KEY_SHIFT_L,
	KEYS_COUNT
};

//const float floorVertices[] = {
//	 -0.5f, -0.5f, 0.5f,
//	  0.5f, -0.5f, 0.5f,
//	  0.5f,  0.5f, 0.5f,
//	 -0.5f,  0.5f, 0.5f,
//
//};
//
//
//const unsigned int floorIndices[] = {
//	0, 1, 2,
//	2, 3, 0
//};

const float floorH = 0.25f;

// FIX THIS - normals are not correct
const float floorVertices[] = {
	// position (x,y,z)   color (r,g,b)      normals (x,y,z)
	// x     y      z     r     g     b     nx    ny    nz
	// Front face
	-0.5f, -0.5f,  0.5f, 
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	// Back face
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
};

const unsigned int floorIndices[] = {
	// Front face
	0, 1, 2,
	2, 3, 0,

	// Back face
	4, 5, 6,
	6, 7, 4,

	// Left face
	0, 3, 7,
	7, 4, 0,

	// Right face
	1, 5, 6,
	6, 2, 1,

	// Top face
	3, 2, 6,
	6, 7, 3,

	// Bottom face
	0, 1, 5,
	5, 4, 0,
};

#endif // __DATA_H