// cmake -S . -B build && cmake --build build
// open build/CSplatoon.app
#include "raylib.h"
#include <iostream>

using namespace std;

float dt;

const int screenWidth = 1280;
const int screenHeight = 720;

class Player {
private:
    float x;
    float y;
    int speedX;
    int speedY;
    float radius;

public:
    Color color;
    void Draw() {
        int posX = static_cast<int>(x);
        int posY = static_cast<int>(y);
        DrawCircle(posX, posY, radius, color);
    }

    void moveRight(float a = 1.0f) {
        if (x + radius < screenWidth) {
            x += speedX * dt * a;
        }
    }
    void moveLeft(float a = 1.0f) {
        if (0 < x - radius) {
            x -= speedX * dt * a;
        }
    }
    void moveUp(float a = 1.0f) {
        if (0 < y - radius) {
            y -= speedY * dt * a;
        }
    }
    void moveDown(float a = 1.0f) {
        if (y + radius < screenHeight) {
            y += speedY * dt * a;
        }
    }
    void moveX(float a = 1.0f) {
        if (a > 0) {
            moveRight(a);
        } else {
            moveLeft(-a);
        }
    }
    void moveY(float a = 1.0f) {
        if (a > 0) {
            moveDown(a);
        } else {
            moveUp(-a);
        }
    }

    Player(float posx, float posy, Color c) {
        x = posx;
        y = posy;
        speedX = 640.0f;
        speedY = 640.0f;
        color = c;
        radius = 48.0f;
    }


};

int main() {
    InitWindow(screenWidth, screenHeight, "CSplatoon");
    SetTargetFPS(60);

    Player player1(100, 100, BLUE);

    float p1PadX = 0;
    float p1PadY = 0;

    while (!WindowShouldClose()) {
        dt = GetFrameTime();
        if (IsGamepadAvailable(0)) {
            p1PadX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            p1PadY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            // cout << "P1 Pad X: " << p1PadX << " Y: " << p1PadY << endl;
            player1.moveX(p1PadX > 0.2f || p1PadX < -0.2f ? p1PadX : 0);
            player1.moveY(p1PadY > 0.2f || p1PadY < -0.2f ? p1PadY : 0);
        }

        if (IsKeyDown(KEY_A)) {
            player1.moveLeft();
        }
        if (IsKeyDown(KEY_D)) {
            player1.moveRight();
        }
        if (IsKeyDown(KEY_W)) {
            player1.moveUp();
        }
        if (IsKeyDown(KEY_S)) {
            player1.moveDown();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            player1.Draw();
            // DrawText("Hello Raylib!", 280, 200, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
