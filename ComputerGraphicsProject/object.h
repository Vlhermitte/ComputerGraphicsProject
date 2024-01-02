#pragma once

#ifndef __OBJECT_H
#define __OBJECT_H

#include "pgr.h"

// Define a WARN_IF_NOT macro
#define WARN_IF(condition, message) \
    do { \
        if (condition) { \
            std::cerr << "\033[31mWarning: " << message << "\033[0m" << std::endl; \
        } \
    } while (0)

#define PLAYER_MODEL_NAME "data/ghoul/ghoul.obj"
#define TERRAIN_MODEL_NAME "data/terrainV3/terrain.obj"
#define FOXBAT_MODEL_NAME "data/MiG25_foxbat/MiG25_foxbat.obj"
#define F5ETIGERII_MODEL_NAME "data/F5ETigerII/F-5ETigerII.obj"
#define CAR_MODEL_NAME "data/RaceCar/RaceCar.obj"
#define TREE1_MODEL_NAME "data/tree1/Tree.obj"
#define TREE2_MODEL_NAME "data/tree2/Tree2.obj"

#define SKYBOX_PATH_NAME "data/skybox"
#define EXPLOSION_TEXTURE_NAME "data/explode.png";

/**
 * \brief Shader program related stuff (id, locations, ...).
 */
typedef struct _ShaderProgram {
	/// identifier for the shader program
	GLuint program;

	bool initialized;

	/**
	  * \brief Indices of the vertex shader inputs (locations)
	  */
	struct {
		// vertex attributes locations
		GLint position;
		GLint color;		// used only if no lighting 
		GLint normal;
		GLint texCoord;
		GLint time;

		// material locations
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint shininess;
		GLint useTexture;
		GLint texSampler;

		// uniforms locations
		GLint PVM;
		GLint ViewMatrix;
		GLint ModelMatrix;
		GLint NormalMatrix;

		GLint fogOn;

		GLint turnSunOn;
		GLint useSpotLight;
		GLint spotLightPosition;
		GLint spotLightDirection;
	} locations;


	_ShaderProgram() : program(0), initialized(false) {
		locations.position = -1;
		locations.color = -1;
		locations.normal = -1;
		locations.texCoord = -1;

		locations.ambient = -1;
		locations.diffuse = -1;
		locations.specular = -1;
		locations.shininess = -1;
		locations.useTexture = -1;
		locations.texSampler = -1;

		locations.PVM = -1;
		locations.ViewMatrix = -1;
		locations.ModelMatrix = -1;
		locations.NormalMatrix = -1;

		locations.fogOn = -1;

		locations.useSpotLight = -1;
		locations.spotLightPosition = -1;
		locations.spotLightDirection = -1;
	}

} ShaderProgram;

typedef struct _SkyboxShaderProgram {
	GLuint program;
	bool initialized;

	struct locations {
		GLint screenCoord;
		GLint inversePVmatrix;
		GLint skyboxSampler;
	} locations;

	_SkyboxShaderProgram() : program(0), initialized(false) {
		locations.screenCoord = -1;
		locations.inversePVmatrix = -1;
		locations.skyboxSampler = -1;
	}
} SkyboxShaderProgram;

typedef struct _ExplosionShaderProgram {
	GLuint program;
	bool initialized;

	struct locations {
		GLint position;
		GLint PVM;
		GLint ViewMatrix;
		GLint time;
		GLint texCoord;
		GLint texSampler;
		GLint frameDuration;
	} locations;

	_ExplosionShaderProgram() : program(0), initialized(false) {
		locations.position = -1;
		locations.PVM = -1;
		locations.ViewMatrix = -1;
		locations.time = -1;
		locations.texCoord = -1;
		locations.texSampler = -1;
		locations.frameDuration = -1;
	}
} ExplosionShaderProgram;


/**
 * \brief Material of an object (ambient, diffuse, specular, shininess).
 */
typedef struct _Material {
	glm::vec3	  ambient;
	glm::vec3	  diffuse;
	glm::vec3	  specular;
	float		  shininess;
	GLuint		  texture;
} Material;

/**
 * \brief Geometry of an object (vertices, triangles).
 */
typedef struct _ObjectGeometry {
	GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
	GLuint        elementBufferObject;  ///< identifier for the element buffer object
	GLuint        vertexArrayObject;    ///< identifier for the vertex array object
	unsigned int  numTriangles;         ///< number of triangles in the mesh
	Material      material;             ///< material of the object
} ObjectGeometry;


struct Mesh {
	GLuint VBO;
	GLuint EBO;
	GLuint VAO;

	GLuint texture;
	int numTriangles;

	glm::vec3 ambient = glm::vec3(0.4f);
	glm::vec3 diffuse = glm::vec3(0.4f);
	glm::vec3 specular = glm::vec3(0.4f);
	float shininess = 32.0f;
};

/**
 * \brief Object in the scene.
 */
typedef struct Object {
	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     verticalSpeed;
	float     size;
	glm::vec3 enable_spot;

	bool destroyed;

	float startTime;
	float currentTime;

	bool isInitialized;
} Object;

typedef struct _Terrain : public Object {

} Terrain;

typedef struct _Player : public Object {
	float viewAngle; // in degrees
	bool isAgainsAnObject; // false
} Player;

typedef struct _Foxbat : public Object {
	bool isMoving;
	float rotationSpeed;
	glm::vec3 initPosition;
} Foxbat;

typedef struct _F5ETigerII : public Object {

} F5ETigerII;

typedef struct _ExplosionObject : public Object {
	int    textureFrames;
	float  frameDuration;
} ExplosionObject;





#endif // __OBJECT_H