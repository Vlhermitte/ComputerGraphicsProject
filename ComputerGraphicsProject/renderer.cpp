/*
* \file renderer.cpp
* \author Valentin Lhermitte
* \date 2023-2024
* \brief Renderer  - intit, drawing functions for models, etc..
*/

#include <iostream>
#include "renderer.h"

ObjectGeometry* TerrainGeometry = NULL;
ObjectGeometry* PlayerGeometry = NULL;
ObjectGeometry* SkyboxGeometry = NULL;
ObjectGeometry* ExplosionGeometry = NULL;
ObjectGeometry* CubeGeometry = NULL;
ObjectGeometry* BannerGeometry = NULL;
ObjectGeometry* CommandsBannerGeometry = NULL;
std::vector<ObjectGeometry*> FoxBatGeometries;
std::vector<ObjectGeometry*> CarGeometries;
std::vector<ObjectGeometry*> PoliceGeometries;
std::vector<ObjectGeometry*> CadillacGeometries;
std::vector<ObjectGeometry*> Tree1Geometries;
std::vector<ObjectGeometry*> Tree2Geometries;
std::vector<ObjectGeometry*> ZepplinGeometries;

ShaderProgram commonShaderProgram;
SkyboxShaderProgram skyboxShaderProgram;
ExplosionShaderProgram explosionShaderProgram;
BannerShaderProgram bannerShaderProgram;


GameObjectsList GameObjects;

bool useLighting = false;

// -----------------------  OpenGL Stuff ---------------------------------

/**
 * \brief Load all shader programs.
 */
void loadShaderPrograms() {

	std::vector<GLuint> shaderList;

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "lightingShaderPerFrag.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "lightingShaderPerFrag.frag"));

	commonShaderProgram.program = pgr::createProgram(shaderList);
	commonShaderProgram.locations.position = glGetAttribLocation(commonShaderProgram.program, "position");
	commonShaderProgram.locations.normal = glGetAttribLocation(commonShaderProgram.program, "normal");
	commonShaderProgram.locations.texCoord = glGetAttribLocation(commonShaderProgram.program, "texCoord");

	// material
	commonShaderProgram.locations.ambient = glGetUniformLocation(commonShaderProgram.program, "material.ambient");
	commonShaderProgram.locations.diffuse = glGetUniformLocation(commonShaderProgram.program, "material.diffuse");
	commonShaderProgram.locations.specular = glGetUniformLocation(commonShaderProgram.program, "material.specular");
	commonShaderProgram.locations.shininess = glGetUniformLocation(commonShaderProgram.program, "material.shininess");
	commonShaderProgram.locations.useTexture = glGetUniformLocation(commonShaderProgram.program, "material.useTexture");
	commonShaderProgram.locations.texSampler = glGetUniformLocation(commonShaderProgram.program, "fragTexSampler");

	// other attributes and uniforms
	commonShaderProgram.locations.PVM = glGetUniformLocation(commonShaderProgram.program, "PVM");
	commonShaderProgram.locations.ViewMatrix = glGetUniformLocation(commonShaderProgram.program, "ViewMatrix");
	commonShaderProgram.locations.ModelMatrix = glGetUniformLocation(commonShaderProgram.program, "ModelMatrix");
	commonShaderProgram.locations.NormalMatrix = glGetUniformLocation(commonShaderProgram.program, "NormalMatrix");
	commonShaderProgram.locations.time = glGetUniformLocation(commonShaderProgram.program, "time");
	commonShaderProgram.locations.fogOn = glGetUniformLocation(commonShaderProgram.program, "fogOn");

	// Lights
	commonShaderProgram.locations.sunAmbient = glGetUniformLocation(commonShaderProgram.program, "sunAmbient");
	commonShaderProgram.locations.sunDiffuse = glGetUniformLocation(commonShaderProgram.program, "sunDiffuse");
	commonShaderProgram.locations.sunSpecular = glGetUniformLocation(commonShaderProgram.program, "sunSpecular");
	commonShaderProgram.locations.turnSunOn = glGetUniformLocation(commonShaderProgram.program, "turnSunOn");
	commonShaderProgram.locations.useSpotLight = glGetUniformLocation(commonShaderProgram.program, "useSpotLight");
	commonShaderProgram.locations.usePointLight = glGetUniformLocation(commonShaderProgram.program, "usePointLight");
	commonShaderProgram.locations.spotLightPosition = glGetUniformLocation(commonShaderProgram.program, "spotLightPosition");
	commonShaderProgram.locations.spotLightDirection = glGetUniformLocation(commonShaderProgram.program, "spotLightDirection");


	// Testing if all attributes are found
	assert(commonShaderProgram.locations.position != -1);
	assert(commonShaderProgram.locations.normal != -1);
	assert(commonShaderProgram.locations.texCoord != -1);

	// Testing if all uniforms are found
	assert(commonShaderProgram.locations.ambient != -1);
	assert(commonShaderProgram.locations.diffuse != -1);
	assert(commonShaderProgram.locations.specular != -1);
	assert(commonShaderProgram.locations.shininess != -1);
	assert(commonShaderProgram.locations.useTexture != -1);
	assert(commonShaderProgram.locations.texSampler != -1);

	assert(commonShaderProgram.locations.PVM != -1);
	assert(commonShaderProgram.locations.ViewMatrix != -1);
	assert(commonShaderProgram.locations.ModelMatrix != -1);
	assert(commonShaderProgram.locations.NormalMatrix != -1);

	assert(commonShaderProgram.locations.sunAmbient != -1);
	assert(commonShaderProgram.locations.sunDiffuse != -1);
	assert(commonShaderProgram.locations.sunSpecular != -1);
	
	WARN_IF(commonShaderProgram.locations.time == -1, "commonShaderProgram.locations.time == -1");
	WARN_IF(commonShaderProgram.locations.fogOn == -1, "commonShaderProgram.locations.fogOn == -1");
	WARN_IF(commonShaderProgram.locations.turnSunOn == -1, "commonShaderProgram.locations.turnSunOn == -1");
	WARN_IF(commonShaderProgram.locations.useSpotLight == -1, "commonShaderProgram.locations.useSpotLight == -1");
	WARN_IF(commonShaderProgram.locations.spotLightPosition == -1, "commonShaderProgram.locations.spotLightPosition == -1");
	WARN_IF(commonShaderProgram.locations.spotLightDirection == -1, "commonShaderProgram.locations.spotLightDirection == -1");

	commonShaderProgram.initialized = true;
	shaderList.clear();

	// Skybox Shaders

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skyboxVertexShader.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skyboxFragmentShader.frag"));

	skyboxShaderProgram.program = pgr::createProgram(shaderList);

	skyboxShaderProgram.locations.screenCoord = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");
	// get uniforms locations
	skyboxShaderProgram.locations.skyboxSampler = glGetUniformLocation(skyboxShaderProgram.program, "skyboxSampler");
	skyboxShaderProgram.locations.inversePVmatrix = glGetUniformLocation(skyboxShaderProgram.program, "inversePVmatrix");


	// assertions (Here using MACRO WARN_IF because the skybox is not essential for the game to work properly)
	WARN_IF(skyboxShaderProgram.locations.screenCoord == -1, "skyboxShaderProgram.locations.screenCoord == -1");
	WARN_IF(skyboxShaderProgram.locations.skyboxSampler == -1, "skyboxShaderProgram.locations.skyboxSampler == -1");
	WARN_IF(skyboxShaderProgram.locations.inversePVmatrix == -1, "skyboxShaderProgram.locations.inversePVmatrix == -1");

	skyboxShaderProgram.initialized = true;
	shaderList.clear();

	// Explosion Shaders

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "explosionVertexShader.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "explosionFragmentShader.frag"));

	explosionShaderProgram.program = pgr::createProgram(shaderList);

	explosionShaderProgram.locations.time = glGetUniformLocation(explosionShaderProgram.program, "time");
	explosionShaderProgram.locations.frameDuration = glGetUniformLocation(explosionShaderProgram.program, "frameDuration");
	explosionShaderProgram.locations.texSampler = glGetUniformLocation(explosionShaderProgram.program, "texSampler");
	explosionShaderProgram.locations.PVM = glGetUniformLocation(explosionShaderProgram.program, "PVM");
	explosionShaderProgram.locations.position = glGetAttribLocation(explosionShaderProgram.program, "position");
	explosionShaderProgram.locations.texCoord = glGetAttribLocation(explosionShaderProgram.program, "texCoord");

	// assertions (Here using MACRO WARN_IF because the skybox is not essential for the game to work properly)
	WARN_IF(explosionShaderProgram.locations.time == -1, "explosionShaderProgram.locations.time == -1");
	WARN_IF(explosionShaderProgram.locations.frameDuration == -1, "explosionShaderProgram.locations.frameDuration == -1");
	WARN_IF(explosionShaderProgram.locations.texSampler == -1, "explosionShaderProgram.locations.texSampler == -1");
	WARN_IF(explosionShaderProgram.locations.PVM == -1, "explosionShaderProgram.locations.PVM == -1");
	WARN_IF(explosionShaderProgram.locations.position == -1, "explosionShaderProgram.locations.position == -1");
	WARN_IF(explosionShaderProgram.locations.texCoord == -1, "explosionShaderProgram.locations.texCoord == -1");

	explosionShaderProgram.initialized = true;
	shaderList.clear();

	// Banner shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "bannerVertexShader.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "bannerFragmentShader.frag"));

	bannerShaderProgram.program = pgr::createProgram(shaderList);

	bannerShaderProgram.locations.position = glGetAttribLocation(bannerShaderProgram.program, "position");
	bannerShaderProgram.locations.texCoord = glGetAttribLocation(bannerShaderProgram.program, "texCoord");
	bannerShaderProgram.locations.PVM = glGetUniformLocation(bannerShaderProgram.program, "PVM");
	bannerShaderProgram.locations.texSampler = glGetUniformLocation(bannerShaderProgram.program, "texSampler");
	bannerShaderProgram.locations.time = glGetUniformLocation(bannerShaderProgram.program, "time");

	WARN_IF(bannerShaderProgram.locations.position == -1, "bannerShaderProgram.locations.position == -1");
	WARN_IF(bannerShaderProgram.locations.PVM == -1, "bannerShaderProgram.locations.PVM == -1");
	WARN_IF(bannerShaderProgram.locations.texCoord == -1, "bannerShaderProgram.locations.texCoord == -1");
	WARN_IF(bannerShaderProgram.locations.texSampler == -1, "bannerShaderProgram.locations.texSampler == -1");
	WARN_IF(bannerShaderProgram.locations.time == -1, "bannerShaderProgram.locations.time == -1");

	bannerShaderProgram.initialized = true;
	shaderList.clear();

}

/**
 * \brief Delete all shader program objects.
 */
void cleanupShaderPrograms(void) {

	pgr::deleteProgramAndShaders(commonShaderProgram.program);
	pgr::deleteProgramAndShaders(skyboxShaderProgram.program);
	pgr::deleteProgramAndShaders(explosionShaderProgram.program);
}


// -----------------------  Init scene objects ---------------------------------

/**
 * \brief Init the objects in the scene.
 */

void initTerrain() {
	if (loadSingleMesh(TERRAIN_MODEL_NAME, commonShaderProgram, &TerrainGeometry) != true) {
		std::cerr << "initTerrain() : Cannot load terrain model" << std::endl;
	}
}

void initPlayer() {
	if(loadSingleMesh(PLAYER_MODEL_NAME, commonShaderProgram, &PlayerGeometry) != true) {
		std::cerr << "initPlayer() : Cannot load player model" << std::endl;
	}

	CHECK_GL_ERROR();
}

void initSkybox() {
	GLint screenCoordLoc = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");
	CHECK_GL_ERROR();
	static const float screenCoords[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f
	};

	SkyboxGeometry = new ObjectGeometry;

	SkyboxGeometry->material.ambient = glm::vec3(0.4f);
	SkyboxGeometry->material.diffuse = glm::vec3(0.4f);
	SkyboxGeometry->material.specular = glm::vec3(0.4f);
	SkyboxGeometry->material.shininess = 32.0f;

	glGenVertexArrays(1, &SkyboxGeometry->vertexArrayObject);
	glBindVertexArray(SkyboxGeometry->vertexArrayObject);

	glGenBuffers(1, &(SkyboxGeometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxGeometry->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(screenCoordLoc);
	glVertexAttribPointer(screenCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	CHECK_GL_ERROR();

	SkyboxGeometry->numTriangles = 2;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &SkyboxGeometry->material.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxGeometry->material.texture);

	const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	GLuint targets[] = {
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_PATH_NAME) + "/sh_" + suffixes[i] + ".jpg";
		std::cout << "Loading cube map texture: " << texName << std::endl;
		if (!pgr::loadTexImage2D(texName, targets[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// unbind the texture (just in case someone will mess up with texture calls later)
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
}

void initExplosion(ObjectGeometry ** geometry) {
	*geometry = new ObjectGeometry();

	std::string textureName = EXPLOSION_TEXTURE_NAME;
	(*geometry)->material.texture = pgr::createTexture(textureName);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explosionVertexData), explosionVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(explosionShaderProgram.locations.position);
	// vertices of triangles - start at the beginning of the array (interlaced array)
	glVertexAttribPointer(explosionShaderProgram.locations.position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(explosionShaderProgram.locations.texCoord);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(explosionShaderProgram.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = explosionNumQuadVertices;
}

void initBanner(ObjectGeometry** geometry, std::string pathName) {
	*geometry = new ObjectGeometry;

	(*geometry)->material.texture = pgr::createTexture(pathName);
	glBindTexture(GL_TEXTURE_2D, (*geometry)->material.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bannerVertexData), bannerVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(bannerShaderProgram.locations.position);
	glEnableVertexAttribArray(bannerShaderProgram.locations.texCoord);
	// vertices of triangles - start at the beginning of the interlaced array
	glVertexAttribPointer(bannerShaderProgram.locations.position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	// texture coordinates of each vertices are stored just after its position
	glVertexAttribPointer(bannerShaderProgram.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = bannerNumQuadVertices;
}

void initCube(ObjectGeometry** geometry) {
	*geometry = new ObjectGeometry();

	std::string textureName = CUBE_TEXTURE_NAME;
	(*geometry)->material.texture = pgr::createTexture(textureName);

	// VAO
	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// VBO
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// EBO
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// Position attribute
	glEnableVertexAttribArray(commonShaderProgram.locations.position);
	glVertexAttribPointer(commonShaderProgram.locations.position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	CHECK_GL_ERROR();

	// Texture attribute
	glEnableVertexAttribArray(commonShaderProgram.locations.texCoord);
	glVertexAttribPointer(commonShaderProgram.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	CHECK_GL_ERROR();

	// Normal attribute
	glEnableVertexAttribArray(commonShaderProgram.locations.normal);
	glVertexAttribPointer(commonShaderProgram.locations.normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	CHECK_GL_ERROR();

	(*geometry)->material.ambient = glm::vec3(1.0f, 0.0f, 1.0f);
	(*geometry)->material.diffuse = glm::vec3(1.0f, 0.0f, 1.0f);
	(*geometry)->material.specular = glm::vec3(1.0f, 0.0f, 1.0f);
	(*geometry)->material.shininess = 10.0f;
	
	glBindVertexArray(0);
	CHECK_GL_ERROR();

	(*geometry)->numTriangles = sizeof(cubeIndices) / sizeof(cubeIndices[0]) / 3;
}

void initModel(const std::string ModelName, std::vector<ObjectGeometry*> *ModelGeometries) {
	if (loadMeshes(ModelName, commonShaderProgram, *ModelGeometries) != true) {
		std::cerr << "\033[31minitModel : Cannot load : " << ModelName << "\033[0m" << std::endl;
	}
}

void initSceneObjects() {
	useLighting = true;
	initTerrain();
	initPlayer();
	initSkybox();
	initExplosion(&ExplosionGeometry);
	initBanner(&BannerGeometry, GAMEOVER_BANNER_NAME);
	initBanner(&CommandsBannerGeometry, COMMANDS_BANNER_NAME);
	initCube(&CubeGeometry);
	initModel(FOXBAT_MODEL_NAME, &FoxBatGeometries);
	initModel(CAR_MODEL_NAME, &CarGeometries);
	initModel(POLICE_MODEL_NAME, &PoliceGeometries);
	initModel(CADILLAC_MODEL_NAME, &CadillacGeometries);
	initModel(ZEPPLIN_MODEL_NAME, &ZepplinGeometries);
	initModel(TREE1_MODEL_NAME, &Tree1Geometries);
	initModel(TREE2_MODEL_NAME, &Tree2Geometries);
}


// -----------------------  Bounds Detection ---------------------------------

glm::vec3 checkBounds(const glm::vec3& position, float objectSize) {
	glm::vec3 newPosition = position;
	if (position.x > TERRAIN_SIZE - objectSize) {
		newPosition.x = TERRAIN_SIZE - objectSize;
	}
	else if (position.x < -TERRAIN_SIZE + objectSize) {
		newPosition.x = -TERRAIN_SIZE + objectSize;
	}
	if (position.y > TERRAIN_SIZE - objectSize) {
		newPosition.y = TERRAIN_SIZE - objectSize;
	}
	else if (position.y < -TERRAIN_SIZE + objectSize) {
		newPosition.y = -TERRAIN_SIZE + objectSize;
	}
	return newPosition;
}

// -----------------------  Drawing ---------------------------------

/**
 * \brief Draw the objects in the scene.
 */

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(commonShaderProgram.locations.PVM, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(commonShaderProgram.locations.ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(commonShaderProgram.locations.ModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	const glm::mat3 modelRotationMatrix = glm::mat3(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2]
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat4(glm::mat3(modelRotationMatrix))));
	glUniformMatrix4fv(commonShaderProgram.locations.NormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	CHECK_GL_ERROR();

	// Passing Sun component 
	Light sun;
	sun.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	sun.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	sun.specular = glm::vec3(1.0f, 1.0f, 1.0f);

	glUniform3fv(commonShaderProgram.locations.sunAmbient, 1, glm::value_ptr(sun.ambient));
	glUniform3fv(commonShaderProgram.locations.sunDiffuse, 1, glm::value_ptr(sun.diffuse));
	glUniform3fv(commonShaderProgram.locations.sunSpecular, 1, glm::value_ptr(sun.specular));
}

void setMaterialUniforms(const Material& material) {
	glUniform3fv(commonShaderProgram.locations.ambient, 1, glm::value_ptr(material.ambient));
	glUniform3fv(commonShaderProgram.locations.diffuse, 1, glm::value_ptr(material.diffuse));
	glUniform3fv(commonShaderProgram.locations.specular, 1, glm::value_ptr(material.specular));
	glUniform1f(commonShaderProgram.locations.shininess, material.shininess);
	CHECK_GL_ERROR();
	if (material.texture != 0) {
		glUniform1i(commonShaderProgram.locations.useTexture, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.texture);
		glUniform1i(commonShaderProgram.locations.texSampler, 0);
	}
	else {
		glUniform1i(commonShaderProgram.locations.useTexture, 0);
	}
}

void drawPlayer(Player* Player, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Player->isInitialized && !Player->destroyed) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, Player->id, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		CHECK_GL_ERROR();
		glUseProgram(commonShaderProgram.program);

		// prepare modeling transform matrix
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Player->position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(Player->viewAngle), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Player->size, Player->size, Player->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		// set material uniforms
		setMaterialUniforms(PlayerGeometry->material);

		glBindVertexArray(PlayerGeometry->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, PlayerGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);
		glUseProgram(0);

		glDisable(GL_STENCIL_TEST);
		CHECK_GL_ERROR();
	}
}

void drawTerrain(Terrain* Terrain, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Terrain->isInitialized) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, Terrain->id, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		CHECK_GL_ERROR();
		glUseProgram(commonShaderProgram.program);

		// prepare modeling transform matrix
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Terrain->position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Terrain->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		TerrainGeometry->material.shininess = 30.0f;
		// set material uniforms
		setMaterialUniforms(TerrainGeometry->material);

		// draw geometry
		glBindVertexArray(TerrainGeometry->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, TerrainGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);

		glDisable(GL_STENCIL_TEST);
		CHECK_GL_ERROR();
	}
	else {
		std::cerr << "Terrain not initialised" << std::endl;
	}
}

void drawCube(Object* Cube, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Cube->isInitialized) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, Cube->id, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		CHECK_GL_ERROR();
		glUseProgram(commonShaderProgram.program);

		// prepare modeling transform matrix
		glm::mat4 modelMatrix = alignObject(Cube->position, (Cube->direction), glm::vec3(1.0f, 1.0f, 1.0f)); // make the cube rotate around its center
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Cube->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		// set material uniforms
		setMaterialUniforms(CubeGeometry->material);

		// draw geometry
		glBindVertexArray(CubeGeometry->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, CubeGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
		CHECK_GL_ERROR();

		glBindVertexArray(0);
		glUseProgram(0);

		glDisable(GL_STENCIL_TEST);
		CHECK_GL_ERROR();
	}
	else {
		std::cerr << "Cube not initialised" << std::endl;
	}

}

void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(skyboxShaderProgram.program);
	glm::mat4 matrix = projectionMatrix * viewMatrix;

	// crate view rotation matrix by using view matrix with cleared translation (we must rotate by 90� on X-axis because we start on Top view)
	glm::mat4 viewRotation = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);
	glUniformMatrix4fv(skyboxShaderProgram.locations.inversePVmatrix, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skyboxShaderProgram.locations.skyboxSampler, 0);

	glBindVertexArray(SkyboxGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxGeometry->material.texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawModel(Object* Model, std::vector<ObjectGeometry*> ModelGeometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Model->isInitialized && !Model->destroyed) {
		
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, Model->id, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		CHECK_GL_ERROR();

		glUseProgram(commonShaderProgram.program);

		// prepare modelling transform matrix
		glm::mat4 modelMatrix = alignObject(Model->position, (Model->direction), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

		modelMatrix = glm::scale(modelMatrix, glm::vec3(Model->size, Model->size, Model->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		for (size_t i = 0; i < ModelGeometry.size(); i++) {
			setMaterialUniforms(ModelGeometry[i]->material);

			// draw geometry
			glBindVertexArray(ModelGeometry[i]->vertexArrayObject);
			glDrawElements(GL_TRIANGLES, ModelGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
		glUseProgram(0);

		glDisable(GL_STENCIL_TEST);
		CHECK_GL_ERROR();
	}
}

void drawExplosion(ExplosionObject* explosion, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	// enable blending and set proper blending function  
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(explosionShaderProgram.program);

	glm::mat4 viewTranslateMatrix = viewMatrix * glm::translate(glm::mat4(1.0f), explosion->position);
	glm::mat4 viewTranslateRotateMatrix = viewTranslateMatrix * glm::mat4(glm::inverse(glm::mat3(viewMatrix)));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(explosion->size));
	glm::mat4 PVMmatrix = projectionMatrix * viewTranslateRotateMatrix * scaleMatrix;

	glUniformMatrix4fv(explosionShaderProgram.locations.PVM, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
	glUniform1f(explosionShaderProgram.locations.time, explosion->currentTime - explosion->startTime);
	glUniform1i(explosionShaderProgram.locations.texSampler, 0);
	glUniform1f(explosionShaderProgram.locations.frameDuration, explosion->frameDuration);

	glBindVertexArray(ExplosionGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, ExplosionGeometry->material.texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, ExplosionGeometry->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);
}

void drawGameOver(Object* Banner, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(bannerShaderProgram.program);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), Banner->position);
	matrix = glm::scale(matrix, glm::vec3(Banner->size));

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(bannerShaderProgram.locations.PVM, 1, GL_FALSE, glm::value_ptr(PVMmatrix));        // model-view-projection
	glUniform1f(bannerShaderProgram.locations.time, Banner->currentTime - Banner->startTime);
	glUniform1i(bannerShaderProgram.locations.texSampler, 0);

	glBindTexture(GL_TEXTURE_2D, BannerGeometry->material.texture);
	glBindVertexArray(BannerGeometry->vertexArrayObject);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, BannerGeometry->numTriangles);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void drawCommandsBanner(Object* Banner, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(bannerShaderProgram.program);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), Banner->position);
	matrix = glm::scale(matrix, glm::vec3(Banner->size));

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(bannerShaderProgram.locations.PVM, 1, GL_FALSE, glm::value_ptr(PVMmatrix));        // model-view-projection
	glUniform1f(bannerShaderProgram.locations.time, Banner->currentTime - Banner->startTime);
	glUniform1i(bannerShaderProgram.locations.texSampler, 0);

	glBindTexture(GL_TEXTURE_2D, CommandsBannerGeometry->material.texture);
	glBindVertexArray(CommandsBannerGeometry->vertexArrayObject);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, CommandsBannerGeometry->numTriangles);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void drawObjects(GameObjectsList GameObjects, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	drawTerrain(GameObjects.terrain, viewMatrix, projectionMatrix);
	drawPlayer(GameObjects.player, viewMatrix, projectionMatrix);
	drawCube(GameObjects.cube, viewMatrix, projectionMatrix);
	drawModel(GameObjects.foxbat, FoxBatGeometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.zepplin, ZepplinGeometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.car, CarGeometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.police, PoliceGeometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.cadillac, CadillacGeometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.tree1, Tree1Geometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.tree2, Tree2Geometries, viewMatrix, projectionMatrix);

	
	glDisable(GL_DEPTH_TEST);
	for (std::list<void*>::iterator it = GameObjects.explosions.begin(); it != GameObjects.explosions.end(); ++it) {
		ExplosionObject* explosion = (ExplosionObject*)(*it);
		drawExplosion(explosion, viewMatrix, projectionMatrix);
	}
	glEnable(GL_DEPTH_TEST);
}


// -----------------------  Cleanup scene objects ----------------------------

void cleanupGeometry(ObjectGeometry* geometry) {
	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

}

void cleanupModels() {
	cleanupGeometry(PlayerGeometry);
	cleanupGeometry(TerrainGeometry);
	cleanupGeometry(SkyboxGeometry);
	cleanupGeometry(ExplosionGeometry);
	cleanupGeometry(CubeGeometry);
	for (size_t i = 0; i < FoxBatGeometries.size(); i++) {
		cleanupGeometry(FoxBatGeometries[i]);
	}
	for (size_t i = 0; i < CarGeometries.size(); i++) {
		cleanupGeometry(CarGeometries[i]);
	}
	for (size_t i = 0; i < PoliceGeometries.size(); i++) {
		cleanupGeometry(PoliceGeometries[i]);
	}
	for (size_t i = 0; i < CadillacGeometries.size(); i++) {
		cleanupGeometry(CadillacGeometries[i]);
	}
	for (size_t i = 0; i < ZepplinGeometries.size(); i++) {
		cleanupGeometry(ZepplinGeometries[i]);
	}
	for (size_t i = 0; i < Tree1Geometries.size(); i++) {
		cleanupGeometry(Tree1Geometries[i]);
	}
	for (size_t i = 0; i < Tree2Geometries.size(); i++) {
		cleanupGeometry(Tree2Geometries[i]);
	}
}

// -----------------------  Loading .obj file ---------------------------------


/** Load mesh using assimp library
 *  Vertex, normals and texture coordinates data are stored without interleaving |VVVVV...|NNNNN...|tttt
 * \param fileName [in] file to open/load
 * \param shader [in] vao will connect loaded data to shader
 * \param geometry
 */
bool loadMeshes(const std::string& fileName, ShaderProgram& shader, std::vector<ObjectGeometry*>& geometries) {
	Assimp::Importer importer;

	std::cout << "Loading model " << fileName << std::endl;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	for (int i = 0; i < scn->mNumMeshes; i++) {
		std::cout << "Mesh " << i << " has " << scn->mMeshes[i]->mNumVertices << " vertices" << std::endl;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we can access the data by scn->*
	for (int i = 0; i < scn->mNumMeshes; i++) {
		// get the current mesh
		const aiMesh* mesh = scn->mMeshes[i];

		ObjectGeometry* geometry = new ObjectGeometry;

		// vertex buffer object, store all vertex positions and normals
		glGenBuffers(1, &(geometry->vertexBufferObject));
		glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
		// first store all vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
		// then store all normals
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

		// just texture 0 for now
		float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
		float* currentTextureCoord = textureCoords;

		// copy texture coordinates
		aiVector3D vect;

		if (mesh->HasTextureCoords(0)) {
			// we use 2D textures with 2 coordinates and ignore the third coordinate
			for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
				vect = (mesh->mTextureCoords[0])[idx];
				*currentTextureCoord++ = vect.x;
				*currentTextureCoord++ = vect.y;
			}
		}

		// finally store all texture coordinates
		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

		// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		}

		// copy our temporary index array to OpenGL and free the array
		glGenBuffers(1, &(geometry->elementBufferObject));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

		delete[] indices;

		// copy material
		// copy the material info to structure
		const aiMaterial* material = scn->mMaterials[mesh->mMaterialIndex];
		aiColor4D color;
		aiString name;
		aiReturn retValue = AI_SUCCESS;

		material->Get(AI_MATKEY_NAME, name);

		// diffiuse
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry->material.diffuse = glm::vec3(color.r, color.g, color.b);

		// ambient
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry->material.ambient = glm::vec3(color.r, color.g, color.b);

		// specular
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry->material.specular = glm::vec3(color.r, color.g, color.b);

		// shininess
		ai_real shininess, strength;
		unsigned int max;	// changed: to unsigned

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;
		max = 1;
		if ((retValue = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;
		geometry->material.shininess = shininess * strength;


		geometry->material.texture = 0;
		// load texture image
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			// get texture name 
			aiString path; // filename

			aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");
			// insert correct texture file path 
			if (found != std::string::npos) { // not found
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading texture file: " << textureName << std::endl;
			geometry->material.texture = pgr::createTexture(textureName);
		}
		else {
			std::cout << "No texture found for mesh " << i << std::endl;
		}
		CHECK_GL_ERROR();

		glGenVertexArrays(1, &(geometry->vertexArrayObject));
		glBindVertexArray(geometry->vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject); // bind our element array buffer (indices) to vao
		glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

		glEnableVertexAttribArray(shader.locations.position);
		glVertexAttribPointer(shader.locations.position, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if (useLighting) {
			glEnableVertexAttribArray(shader.locations.normal);
			glVertexAttribPointer(shader.locations.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
		}
		else {
			glDisableVertexAttribArray(shader.locations.color);
			// following line is problematic on AMD/ATI graphic cards
			// -> if you see black screen (no objects at all) than try to set color manually in vertex shader to see at least something
			glVertexAttrib3f(shader.locations.color, color.r, color.g, color.b);
		}

		glEnableVertexAttribArray(shader.locations.texCoord);
		glVertexAttribPointer(shader.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
		CHECK_GL_ERROR();

		glBindVertexArray(0);

		geometry->numTriangles = mesh->mNumFaces;

		geometries.push_back(geometry);
	}

	return true;
}

bool loadSingleMesh(const std::string & fileName, ShaderProgram & shader, ObjectGeometry **geometry) {
	Assimp::Importer importer;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		*geometry = NULL;
		return false;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		*geometry = NULL;
		return false;
	}

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	const aiMesh* mesh = scn->mMeshes[0];

	*geometry = new ObjectGeometry;

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// then store all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	// just texture 0 for now
	float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;

	// copy texture coordinates
	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// we use 2D textures with 2 coordinates and ignore the third coordinate
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	// finally store all texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// copy the material info to structure
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	(*geometry)->material.diffuse = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->material.ambient = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->material.specular = glm::vec3(color.r, color.g, color.b);

	ai_real shininess, strength;
	unsigned int max;	// changed: to unsigned

	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
		shininess = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	(*geometry)->material.shininess = shininess * strength;

	(*geometry)->material.texture = 0;

	// load texture image
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		// get texture name 
		aiString path; // filename

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = fileName.find_last_of("/\\");
		// insert correct texture file path 
		if (found != std::string::npos) { // not found
			//subMesh_p->textureName.insert(0, "/");
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		(*geometry)->material.texture = pgr::createTexture(textureName);
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glEnableVertexAttribArray(shader.locations.position);
	glVertexAttribPointer(shader.locations.position, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (useLighting) {
		glEnableVertexAttribArray(shader.locations.normal);
		glVertexAttribPointer(shader.locations.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
	}
	else {
		glDisableVertexAttribArray(shader.locations.color);
		// following line is problematic on AMD/ATI graphic cards
		// -> if you see black screen (no objects at all) than try to set color manually in vertex shader to see at least something
		glVertexAttrib3f(shader.locations.color, color.r, color.g, color.b);
	}

	glEnableVertexAttribArray(shader.locations.texCoord);
	glVertexAttribPointer(shader.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	(*geometry)->numTriangles = mesh->mNumFaces;

	return true;
}
