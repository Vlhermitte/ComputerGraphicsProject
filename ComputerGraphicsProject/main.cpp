#include <iostream>
#include "main.h"


struct _GameState {
	bool wireframeMode; // false
	bool fpsCameraMode; // false
	bool sceneCamera; // false
	float cameraElevationAngle; // in degrees
	float elapsedTime; // in seconds
	bool keyMap[KEYS_COUNT]; 
	bool fogOn; // false
	bool turnSunOn; // true
	bool useSpotLight; // false

	int windowWidth; // 800 (currently not used)
	int windowHeight; // 800 (currently not used)

	_GameState() : 
		wireframeMode(false), 
		fpsCameraMode(false), 
		sceneCamera(false), 
		cameraElevationAngle(0.0f), 
		elapsedTime(0.0f), 
		fogOn(false),
		turnSunOn(true),
		useSpotLight(false), 
		windowWidth(WINDOW_WIDTH), 
		windowHeight(WINDOW_HEIGHT) {
		for (int i = 0; i < KEYS_COUNT; i++)
			keyMap[i] = false;
	}
} GameState;

// -----------------------  Application ---------------------------------

/**
 * \brief Initialize application data and OpenGL stuff.
 */
void initApplication() {
	// init OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	// initialize random seed
	srand((unsigned int)time(NULL));

	// - all programs (shaders), buffers, textures, ...
	loadShaderPrograms();

	// init scene objects
	initSceneObjects();

	GameObjects.player = NULL;
	GameObjects.foxbat = NULL;
	GameObjects.terrain = NULL;
	GameObjects.car = NULL;

	// init your Application
	// - setup the initial application state

	// restart the game
	restartGame();
}

/**
 * \brief Delete all OpenGL objects and application data.
 */
void finalizeApplication() {

	cleanUpObjects();

	delete GameObjects.player;
	delete GameObjects.terrain;

	// delete buffers
	cleanupModels();

	// delete shaders
	cleanupShaderPrograms();
}

/**
 * \brief Restart the game. Delete all objects and reinitialize them.
 */
void restartGame() {
	// delete all objects
	cleanUpObjects();

	GameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// Objects reinisialisation
	reinisialiseObjects();

	// GameState reinitialization
	if (GameState.fpsCameraMode or GameState.sceneCamera) {
		GameState.fpsCameraMode = false;
		GameState.sceneCamera = false;
		glutPassiveMotionFunc(NULL);
	}
	GameState.cameraElevationAngle = 0.0f;

	GameState.fogOn = false;

	// reset key map
	for (int i = 0; i < KEYS_COUNT; i++)
		GameState.keyMap[i] = false;
}

void reinisialiseObjects() {
	if (GameObjects.player == NULL) {
		GameObjects.player = new Player();
		GameObjects.player->isInitialized = true;
	}
	if (GameObjects.terrain == NULL) {
		GameObjects.terrain = new Terrain();
		GameObjects.terrain->isInitialized = true;
	}
	if (GameObjects.foxbat == NULL) {
		GameObjects.foxbat = new Foxbat();
		GameObjects.foxbat->isInitialized = true;
	}
	if (GameObjects.car == NULL) {
		GameObjects.car = new Object();
		GameObjects.car->isInitialized = true;
	}
	if (GameObjects.tree1 == NULL) {
		GameObjects.tree1 = new Object();
		GameObjects.tree1->isInitialized = true;
	}
	if (GameObjects.tree2 == NULL) {
		GameObjects.tree2 = new Object();
		GameObjects.tree2->isInitialized = true;
	}

	// Reinitialization Player
	GameObjects.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
	GameObjects.player->viewAngle = 90.0f; // degrees
	GameObjects.player->direction = glm::vec3(cos(glm::radians(GameObjects.player->viewAngle)), sin(glm::radians(GameObjects.player->viewAngle)), 0.0f);
	GameObjects.player->speed = 0.0f;
	GameObjects.player->size = PLAYER_SIZE;
	GameObjects.player->destroyed = false;
	GameObjects.player->startTime = GameState.elapsedTime;
	GameObjects.player->currentTime = GameObjects.player->startTime;

	// Reinitialization Foxbat object
	GameObjects.foxbat->position = glm::vec3(0.1f, 0.3f, 0.0f);
	GameObjects.foxbat->initPosition = GameObjects.foxbat->position;
	GameObjects.foxbat->direction = glm::vec3(0.8f, 0.5f, 0.0f);
	GameObjects.foxbat->speed = 0.4f;
	GameObjects.foxbat->size = FOXBAT_SIZE;
	GameObjects.foxbat->destroyed = false;
	GameObjects.foxbat->isMoving = false;

	// Reinitialization Car object
	GameObjects.car->position = glm::vec3(0.8f, 0.15f, -0.3f);
	GameObjects.car->direction = glm::vec3(0.1f, 0.1f, 0.0f);
	GameObjects.car->speed = 0.0f;
	GameObjects.car->size = CAR_SIZE;
	GameObjects.car->destroyed = false;

	// Reinitialization Tree1 object
	GameObjects.tree1->position = glm::vec3(-0.7f, -0.6f, -0.15f);
	GameObjects.tree1->direction = glm::vec3(0.1f, 0.1f, 0.0f);
	GameObjects.tree1->speed = 0.0f;
	GameObjects.tree1->size = TREE_SIZE;
	GameObjects.tree1->destroyed = false;

	// Reinitialization Tree2 object
	GameObjects.tree2->position = glm::vec3(0.6f, 0.3f, -0.15f);
	GameObjects.tree2->direction = glm::vec3(0.1f, 0.1f, 0.0f);
	GameObjects.tree2->speed = 0.0f;
	GameObjects.tree2->size = TREE_SIZE;
	GameObjects.tree2->destroyed = false;

	// Setting up the terrain with position (0,0,MIN_HEIGHT) (xyz)
	GameObjects.terrain->position = glm::vec3(0.0f, 0.0f, MIN_HEIGHT);
	GameObjects.terrain->size = TERRAIN_SIZE;
}

/**
 * \brief Delete all objects.
 */
void cleanUpObjects() {
	// delete all non essential objects (i.e not the player and the terrain)
	// TODO : Delete objects when we have impl
}

// -----------------------  Colision Detection ---------------------------------

bool detectColision(const glm::vec3& point, const glm::vec3& center, float radius) {
	// Using sphere colision detection
	float distance = glm::distance(point, center);
	return distance < radius;
}


void checkCollisions() {
	// check colision between player and foxbat
	if (!GameObjects.foxbat->destroyed && detectColision(GameObjects.player->position, GameObjects.foxbat->position, GameObjects.foxbat->size)) {
		// add explosion
		addExplosion(GameObjects.foxbat->position);
		// destroy foxbat
		GameObjects.foxbat->destroyed = true;
	}

	// check colision between player and tree1
	if (!GameObjects.tree1->destroyed && detectColision(GameObjects.player->position, GameObjects.tree1->position, GameObjects.tree1->size)) {
		// add explosion
		addExplosion(GameObjects.tree1->position);
		// destroy tree1
		GameObjects.tree1->destroyed = true;
	}

	// check colision between player and tree2
	if (!GameObjects.tree2->destroyed && detectColision(GameObjects.player->position, GameObjects.tree2->position, GameObjects.tree2->size)) {
		// add explosion
		addExplosion(GameObjects.tree2->position);
		// destroy tree2
		GameObjects.tree2->destroyed = true;
	}
}


// -----------------------  Scene objects ---------------------------------

/**
 * \brief Draw all scene objects.
 */
void drawScene() {
	// setup parallel projection
	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f * SCENE_DEPTH, 10.0f * SCENE_DEPTH
	);
	// static viewpoint - top view
	glm::mat4 orthoViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	glm::mat4 viewMatrix = orthoViewMatrix;
	glm::mat4 projectionMatrix = orthoProjectionMatrix;

	// defining spotlight direction (default is the player direction) 
	glm::vec3 spotlightDirection = GameObjects.player->direction;
	if (GameState.fpsCameraMode) {
		glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::vec3 cameraPosition = GameObjects.player->position;
		glm::vec3 cameraDirection = GameObjects.player->direction * std::cos(glm::radians(-GameState.cameraElevationAngle))
			+ std::sin(glm::radians(-GameState.cameraElevationAngle)) * glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraCenter = cameraPosition + cameraDirection;

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);

		projectionMatrix = glm::perspective(glm::radians(70.0f), GameState.windowWidth / (float)GameState.windowHeight, 0.1f, 10.0f);
		// if the camera is in fps mode the spotlight direction is the camera direction
		spotlightDirection = cameraDirection;
	}
	else if (GameState.sceneCamera) {
		glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraPosition = glm::vec3(0.0f, -0.2f, 0.2f);
		glm::vec3 cameraTarget = GameObjects.player->position;

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraTarget,
			cameraUpVector
		);

		projectionMatrix = glm::perspective(glm::radians(100.0f), GameState.windowWidth / (float)GameState.windowHeight, 0.1f, 10.0f);

	}

	CHECK_GL_ERROR();

	glUseProgram(commonShaderProgram.program);
	glUniform1f(commonShaderProgram.locations.time, GameState.elapsedTime);
	glUniform1i(commonShaderProgram.locations.fogOn, GameState.fogOn);
	glUniform1i(commonShaderProgram.locations.turnSunOn, GameState.turnSunOn);
	glUniform1i(commonShaderProgram.locations.useSpotLight, GameState.useSpotLight);
	glUniform3fv(commonShaderProgram.locations.spotLightPosition, 1, glm::value_ptr(GameObjects.player->position));
	glUniform3fv(commonShaderProgram.locations.spotLightDirection, 1, glm::value_ptr(spotlightDirection));
	glUseProgram(0);

	// draw the scene objects
	drawObjects(GameObjects, viewMatrix, projectionMatrix);
	
	// draw skybox (only if the fog is off)
	if (!GameState.fogOn)
		drawSkybox(viewMatrix, projectionMatrix);
}

/**
 * \brief Draw the player.
 * \param player Player object.
 * \param viewMatrix View matrix.
 * \param projectionMatrix Projection matrix.
 */
void updateObjects(float elapsedTime) {
	// update the scene objects
	float timeDelta = elapsedTime - GameObjects.player->currentTime;

	// Check colisions 
	checkCollisions();
	
	// Update Player
	GameObjects.player->position += GameObjects.player->direction * GameObjects.player->speed * 0.01f;
	// We clamp the player position to the scene size 
	// Not using the checkBounds() because we don't want to teleport the player to the other side of the scene
	GameObjects.player->position = glm::clamp(
		GameObjects.player->position, 
		glm::vec3(-SCENE_WIDTH + GameObjects.player->size, -SCENE_HEIGHT + GameObjects.player->size, MIN_HEIGHT), 
		glm::vec3(SCENE_WIDTH - GameObjects.player->size, SCENE_HEIGHT - GameObjects.player->size, MAX_HEIGHT)
	);

	// Update Foxbat (airplane)
	if (GameObjects.foxbat->isMoving) {
		GameObjects.foxbat->currentTime = elapsedTime;
		float curveParamT = GameObjects.foxbat->speed * (GameObjects.foxbat->currentTime - GameObjects.foxbat->startTime);
		glm::vec3 closedCurve = evaluateClosedCurve(curveData, curveSize, curveParamT);
		GameObjects.foxbat->position = GameObjects.foxbat->initPosition + closedCurve;
		GameObjects.foxbat->position = checkBounds(GameObjects.foxbat->position, GameObjects.foxbat->size);
		GameObjects.foxbat->direction = glm::normalize(evaluateClosedCurve_1stDerivative(curveData, curveSize, curveParamT));
	}

	// Update Explosion frame (ietrate through the list of frames)
	std::list<void*>::iterator it = GameObjects.explosions.begin();
	while (it != GameObjects.explosions.end()) {
		ExplosionObject* explosion = (ExplosionObject*)(*it);

		// update explosion
		explosion->currentTime = elapsedTime;

		if (explosion->currentTime > explosion->startTime + explosion->textureFrames * explosion->frameDuration) {
			explosion->destroyed = true;
		}
		if (explosion->destroyed == true) {
			it = GameObjects.explosions.erase(it);
		}
		else {
			++it;
		}
	}

}

/*
* \brief Add a new explosion in the GameObjects.explosions list
* \param glm::vec3 position
*/
void addExplosion(const glm::vec3& position) {

	ExplosionObject* newExplosion = new ExplosionObject;

	newExplosion->speed = 0.0f;
	newExplosion->destroyed = false;

	newExplosion->startTime = GameState.elapsedTime;
	newExplosion->currentTime = newExplosion->startTime;

	newExplosion->size = 0.1f;
	newExplosion->direction = glm::vec3(0.0f, 0.0f, 1.0f);

	newExplosion->frameDuration = 0.1f;
	newExplosion->textureFrames = 16;

	newExplosion->position = position;

	GameObjects.explosions.push_back(newExplosion);
}

// -----------------------  Window callbacks ---------------------------------

/**
 * \brief Draw the window contents.
 */
void displayCb() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the window contents (scene objects)
	drawScene();

	glutSwapBuffers();
}

// -----------------------  Keyboard callbacks ---------------------------------

/**
 * \brief Handle keyboard events.
 * \param key ASCII code of the key that was pressed.
 * \param x X-coordinate of the mouse cursor.
 * \param y Y-coordinate of the mouse cursor.
 */

void mouseMotionCb(int mouseX, int mouseY) {
	// std::cout << "Player view angle: " << GameState.cameraElevationAngle << std::endl;
	float cameraElevationAngleDelta = 0.01f * (mouseY - GameState.windowHeight / 2);

	if (fabs(GameState.cameraElevationAngle + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX)
		GameState.cameraElevationAngle += cameraElevationAngleDelta;

	// set mouse pointer to the window center (Might not work in a VM) 
	glutWarpPointer(GameState.windowWidth / 2, GameState.windowHeight / 2);

	glutPostRedisplay();
}

// The keyboard callback is triggered when keyboard function keys or ASCII
void keyboardCb(unsigned char keyPressed, int mouseX, int mouseY) {
	
	switch (keyPressed) {
		case 27: // ESC key
			glutLeaveMainLoop();
			exit(EXIT_SUCCESS);
			break;
		case 'r': // restart game
			restartGame();
			break;
		case 'w': // switch wireframe mode
			if (GameState.wireframeMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GameState.wireframeMode = false;
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				GameState.wireframeMode = true;
			}
			break;
		case 'c': // switch camera mode to first person
			GameState.fpsCameraMode = !GameState.fpsCameraMode;
			if (GameState.fpsCameraMode) {
				GameState.sceneCamera = false;
				printf("First person camera\n");
				glutPassiveMotionFunc(mouseMotionCb);
				glutWarpPointer(GameState.windowWidth / 2, GameState.windowHeight / 2);
			}
			else {
				printf("Top view Camera\n");
				glutPassiveMotionFunc(NULL);
			}
			break;
		case 'v': // switch camera mode to scene camera
			GameState.sceneCamera = !GameState.sceneCamera;
			if (GameState.sceneCamera) {
				GameState.fpsCameraMode = false;
				printf("Scene camera\n");
				glutPassiveMotionFunc(NULL);
			}
			else {
				printf("Top view Camera\n");
			}
			break;
		case 'f':
			GameState.fogOn = !GameState.fogOn;
			// print fog on or fog off
			GameState.fogOn ? printf("Fog On\n") : printf("Fog Off\n");
			break;
		case 'y':
			GameState.turnSunOn = !GameState.turnSunOn;
			// print sun on or sun off
			GameState.turnSunOn ? printf("Sun On\n") : printf("Sun Off\n");
			break;
		case 'u':
			GameState.useSpotLight = !GameState.useSpotLight;
			// print spot light on or spot light off
			GameState.useSpotLight ? printf("Spot light On\n") : printf("Spot light Off\n");
			break;
		case 'm':
			GameObjects.foxbat->isMoving = !GameObjects.foxbat->isMoving;
			GameObjects.foxbat->isMoving ? printf("Foxbat moving\n") : printf("Foxbat stopped\n");
			break;
		case 'e':
			if (!GameObjects.car->destroyed) {
				addExplosion(GameObjects.car->position);
				GameObjects.car->destroyed = true;
				printf("Car destroyed\n");
			}
			break;

		// Disbaled because buggy
		/*case ' ':
			GameState.keyMap[KEY_SPACE] = true;
			break;
		case 'b':
			GameState.keyMap[KEY_B] = true;
			break;*/
	default:
		break;
	}
}

void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY) {

	switch (keyReleased) {
	case ' ':
		GameState.keyMap[KEY_SPACE] = false;
		break;
	case 'b':
		GameState.keyMap[KEY_B] = false;
		break;
	default:
		break;
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are pressed.
void specialPressedKeyboardCb(int specKeyPressed, int mouseX, int mouseY) {

	switch (specKeyPressed) {
		case GLUT_KEY_RIGHT:
			GameState.keyMap[KEY_RIGHT_ARROW] = true;
			break;
		case GLUT_KEY_LEFT:
			GameState.keyMap[KEY_LEFT_ARROW] = true;
			break;
		case GLUT_KEY_UP:
			GameState.keyMap[KEY_UP_ARROW] = true;
			break;
		case GLUT_KEY_DOWN:
			GameState.keyMap[KEY_DOWN_ARROW] = true;
			break;
		case GLUT_KEY_SHIFT_L:
			GameState.keyMap[KEY_SHIFT_L] = true;
			break;
	default:
		break;
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are released.
void specialReleasedKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY) {

	switch (specKeyReleased) {
	case GLUT_KEY_RIGHT:
		GameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		GameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		GameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		GameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_SHIFT_L:
		GameState.keyMap[KEY_SHIFT_L] = false;
		break;
	default:
		break;
	}
}

// -----------------------  Player Movements handeling ---------------------------------

void movePlayerForward(float deltaSpeed) {

	if (GameObjects.player->speed < PLAYER_SPEED_MAX) {
		GameObjects.player->speed += deltaSpeed;
	}
}

void movePlayerBackward(float deltaSpeed) {

	if (GameObjects.player->speed > PLAYER_SPEED_MIN) {
		GameObjects.player->speed -= deltaSpeed;
	}
}

void movePlayerLeft(float deltaAngle) {
	GameObjects.player->viewAngle = std::fmod(GameObjects.player->viewAngle + deltaAngle, 360.0);
	glm::vec3 newVector(
		std::cos(glm::radians(GameObjects.player->viewAngle)),
		std::sin(glm::radians(GameObjects.player->viewAngle)),
		0
	);
	GameObjects.player->direction = newVector;
}

void movePlayerRight(float deltaAngle) {
	GameObjects.player->viewAngle = std::fmod(GameObjects.player->viewAngle - deltaAngle, 360.0);
	glm::vec3 newVector(
		std::cos(glm::radians(GameObjects.player->viewAngle)),
		std::sin(glm::radians(GameObjects.player->viewAngle)),
		0
	);
	GameObjects.player->direction = newVector;
}

void movePlayerUp(float deltaSpeed) {
	printf("Player up : position: %f, %f, %f\n", GameObjects.player->position.x, GameObjects.player->position.y, GameObjects.player->position.z);
	GameObjects.player->verticalSpeed += deltaSpeed;
}

void movePlayerDown(float deltaSpeed) {
	printf("Player down : position: %f, %f, %f\n", GameObjects.player->position.x, GameObjects.player->position.y, GameObjects.player->position.z);
	GameObjects.player->verticalSpeed -= deltaSpeed;
}

// -----------------------  GLUT callbacks ---------------------------------

/**
 * \brief Timer callback.
 * \param timerId Identifier of the timer.
 */

void timerCb(int timerId) {
	GameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	if (GameState.keyMap[KEY_UP_ARROW] == true)
		movePlayerForward(PLAYER_SPEED_INCREMENT);

	if (GameState.keyMap[KEY_DOWN_ARROW] == true)
		movePlayerBackward(PLAYER_SPEED_INCREMENT);

	if (GameState.keyMap[KEY_RIGHT_ARROW] == true)
		movePlayerRight(PLAYER_VIEW_ANGLE_DELTA);

	if (GameState.keyMap[KEY_LEFT_ARROW] == true)
		movePlayerLeft(PLAYER_VIEW_ANGLE_DELTA);

	if (GameState.keyMap[KEY_SPACE] == true)
		movePlayerUp(PLAYER_UP_SPEED_INCREMENT);

	if (GameState.keyMap[KEY_B] == true)
		movePlayerDown(PLAYER_UP_SPEED_INCREMENT);
	
	// update objects in the scene
	updateObjects(GameState.elapsedTime);

	glutTimerFunc(1000 / 30, timerCb, 0); // redraw every 30 ms = frame rate of 33 FPS
	glutPostRedisplay();
}

// Called whenever the window is resized. The new window size is given, in pixels.
// This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshapeCb(int newWidth, int newHeight) {

	GameState.windowWidth = newWidth;
	GameState.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

int main(int argc, char** argv) {

	// initialize the GLUT library (windowing system)
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);


	// for each window
	{
		//   initial window size + title
		glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		glutCreateWindow(WINDOW_TITLE);

		// callbacks - use only those you need
		glutDisplayFunc(displayCb);
		glutReshapeFunc(reshapeCb);
		glutKeyboardFunc(keyboardCb);
		// glutKeyboardUpFunc(keyboardUpCb);
		glutSpecialFunc(specialPressedKeyboardCb);     // key pressed
		glutSpecialUpFunc(specialReleasedKeyboardUpCb); // key released
		// glutMouseFunc(mouseCb);
		// glutMotionFunc(mouseMotionCb);
		glutTimerFunc(1000 / 30, timerCb, 0); // redraw every 30 ms

	}
	// end for each window 

	// initialize pgr-framework (GL, DevIl, etc.)
	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	// init your stuff - shaders & program, buffers, locations, state of the application
	initApplication();

	// handle window close by the user
	glutCloseFunc(finalizeApplication);

	// Infinite loop handling the events
	glutMainLoop();

	return EXIT_SUCCESS;
}