#include <iostream>
#include "main.h"


struct _GameObjects {
	Player* player;
	Terrain* terrain;
} GameObjects;

struct _GameState {
	bool wireframeMode; // false
	bool fpsCameraMode; // false
	bool sceneCamera; // false
	float cameraElevationAngle; // in degrees
	float elapsedTime; // in seconds
	bool keyMap[KEYS_COUNT]; 

	int windowWidth; // 500 (currently not used)
	int windowHeight; // 500 (currently not used)
} GameState;

// -----------------------  Application ---------------------------------

/**
 * \brief Initialize application data and OpenGL stuff.
 */
void initApplication() {
	// init OpenGL
	// - all programs (shaders), buffers, textures, ...
	loadShaderPrograms();

	// init scene objects
	initSceneObjects();

	GameObjects.player = NULL;
	GameObjects.terrain = NULL;

	// init your Application
	// - setup the initial application state

	// restart the game
	restartGame();
}

/**
 * \brief Delete all OpenGL objects and application data.
 */
void finalizeApplication() {

	// cleanUpObjects();

	// delete buffers
	// cleanupModels();

	// delete shaders
	cleanupShaderPrograms();
}

/**
 * \brief Restart the game. Delete all objects and reinitialize them.
 */
void restartGame() {
	// delete all objects
	cleanUpObjects();

	GameState.elapsedTime = 0.01f * (float)glutGet(GLUT_ELAPSED_TIME);

	if (GameObjects.player == NULL)
		GameObjects.player = new Player();
	if (GameObjects.terrain == NULL)
		GameObjects.terrain = new Terrain();

	// GameObjects reinitialization
	GameObjects.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
	GameObjects.player->viewAngle = 90.0f; // degrees
	GameObjects.player->direction = glm::vec3(cos(glm::radians(GameObjects.player->viewAngle)), sin(glm::radians(GameObjects.player->viewAngle)), 0.0f);
	GameObjects.player->speed = 0.0f;
	GameObjects.player->size = PLAYER_SIZE;
	GameObjects.player->destroyed = false;
	GameObjects.player->startTime = GameState.elapsedTime;
	GameObjects.player->currentTime = GameObjects.player->startTime;

	// Setting up the terrain with position (0,0,MIN_HEIGHT) (xyz)
	GameObjects.terrain->position = glm::vec3(0.0f, 0.0f, MIN_HEIGHT);
	GameObjects.terrain->size = TERRAIN_SIZE;

	// GameState reinitialization
	if (GameState.fpsCameraMode or GameState.sceneCamera) {
		GameState.fpsCameraMode = false;
		GameState.sceneCamera = false;
		glutPassiveMotionFunc(NULL);
	}
	GameState.cameraElevationAngle = 0.0f;

	// reset key map
	for (int i = 0; i < KEYS_COUNT; i++)
		GameState.keyMap[i] = false;
}

/**
 * \brief Delete all objects.
 */
void cleanUpObjects() {
		// delete all objects
	delete GameObjects.floor;
	delete GameObjects.apple;
	delete GameObjects.ferrari;

}


// -----------------------  Scene objects ---------------------------------

/**
 * \brief Draw all scene objects.
 */
void drawScene() {
	// std::cout << "Drawing scene" << std::endl;

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

	if (GameState.fpsCameraMode) {
		glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::vec3 cameraPosition = GameObjects.player->position;
		glm::vec3 cameraDirection = GameObjects.player->direction;
		glm::vec3 cameraCenter = cameraPosition + cameraDirection * std::cos(glm::radians(-GameState.cameraElevationAngle))
			+ std::sin(glm::radians(-GameState.cameraElevationAngle)) * glm::vec3(0.0f, 0.0f, 1.0f);

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);

		projectionMatrix = glm::perspective(glm::radians(70.0f), GameState.windowWidth / (float)GameState.windowHeight, 0.1f, 10.0f);
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

	// draw the scene objects
	drawTerrain(GameObjects.terrain, viewMatrix, projectionMatrix);
	drawPlayer(GameObjects.player, viewMatrix, projectionMatrix);
	// drawApple(GameObject.apple, viewMatrixApple, projectionMatrixApple);
	// drawFerrari(GameObject.ferrari, viewMatrixFerrari, projectionMatrixFerrari);
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
	// printf("Time delta: %f\n", timeDelta);
	GameObjects.player->currentTime = elapsedTime;
	// printf("GameObjects.player->speed: %f\n", GameObjects.player->speed);
	GameObjects.player->position += GameObjects.player->direction * GameObjects.player->speed * 0.01f;
	if (GameObjects.player->position.z < MAX_HEIGHT)
		GameObjects.player->position.z += GameObjects.player->verticalSpeed * 0.01f;
	// printf("Player position: %f, %f, %f\n", GameObjects.player->position.x, GameObjects.player->position.y, GameObjects.player->position.z);

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
			GameState.fpsCameraMode = false;
			if (GameState.sceneCamera) {
				printf("Scene camera\n");
				glutPassiveMotionFunc(NULL);
			}
			else {
				printf("Top view Camera\n");
			}
			break;
		case ' ':
			GameState.keyMap[KEY_SPACE] = true;
			break;
	default:
		break;
	}
}

void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY) {

	switch (keyReleased) {
	case ' ':
		GameState.keyMap[KEY_SPACE] = false;
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
	printf("Player position: %f, %f, %f\n", GameObjects.player->position.x, GameObjects.player->position.y, GameObjects.player->position.z);
	GameObjects.player->verticalSpeed += deltaSpeed;
}

void movePlayerDown(float deltaSpeed) {
	printf("Player position: %f, %f, %f\n", GameObjects.player->position.x, GameObjects.player->position.y, GameObjects.player->position.z);
	GameObjects.player->verticalSpeed -= deltaSpeed;
}

// -----------------------  GLUT callbacks ---------------------------------

/**
 * \brief Timer callback.
 * \param timerId Identifier of the timer.
 */

void timerCb(int timerId) {

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

	if (GameState.keyMap[KEY_SHIFT_L] == true)
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