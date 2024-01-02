#pragma once

#ifndef __MAIN_H
#define __MAIN_H

#include "pgr.h"
#include "renderer.h"
#include "spline.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;
constexpr char WINDOW_TITLE[] = "Project Computer Graphics Valentin Lhermitte";

// ----------------------- Application ---------------------------------
void initApplication();
void finalizeApplication();
void restartGame();
void reinisialiseObjects();
void cleanUpObjects();

// -----------------------  Scene objects ---------------------------------
void drawScene();

// -----------------------  Explosion ---------------------------------
void addExplosion(const glm::vec3& position);

// -----------------------  Window callbacks ---------------------------------
void displayCb();
void reshapeCb(int width, int height);

// -----------------------  Keyboard callbacks ---------------------------------
void mouseMotionCb(int mouseX, int mouseY);
void keyboardCb(unsigned char keyPressed, int mouseX, int mouseY);
void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY);
void specialPressedKeyboardCb(int specKeyPressed, int mouseX, int mouseY);
void specialReleasedKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY);

// -----------------------  Player Movements handeling ---------------------------------
void movePlayerForward(float deltaSpeed);
void movePlayerBackward(float deltaSpeed);
void movePlayerLeft(float deltaSpeed);
void movePlayerRight(float deltaSpeed);
void movePlayerUp(float deltaSpeed);
void movePlayerDown(float deltaSpeed);

// -----------------------  GLUT callbacks ---------------------------------
void timerCb(int timerId);

#endif // __MAIN_H