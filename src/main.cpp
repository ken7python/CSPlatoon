// cmake -S . -B build && cmake --build build
// open build/CSplatoon.app
#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>

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
    Color color;
    Color fillColor;

public:
    void Draw() {
        DrawCircle(GetX(), GetY(), radius, color);
    }

    void moveRight(float a = 1.0f) {
        if (x + radius / 2 < screenWidth) {
            x += speedX * dt * a;
        }
    }
    void moveLeft(float a = 1.0f) {
        if (0 < x - radius / 2) {
            x -= speedX * dt * a;
        }
    }
    void moveUp(float a = 1.0f) {
        if (0 < y - radius / 2) {
            y -= speedY * dt * a;
        }
    }
    void moveDown(float a = 1.0f) {
        if (y + radius / 2 < screenHeight) {
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

    int GetX() {
        return static_cast<int>(x);
    }
    int GetY() {
        return static_cast<int>(y);
    }
    Vector2 GetVector() {
        return Vector2{ x, y };
    }
    float GetRadius() {
        return radius;
    }

    Color GetFillColor() {
        return fillColor;
    }

    Player(float posx, float posy, Color c,Color fc = WHITE) {
        x = posx;
        y = posy;
        speedX = 640.0f;
        speedY = 640.0f;
        color = c;
        fillColor = fc;
        radius = 48.0f;
    }
};

const float dotSize = 3.0f;
class Dot {
private:
    float x;
    float y;
    float width;
    float height;
    Color color;
    Rectangle dot;

    bool isHit(Player* p) {
        Vector2 playerPos = p->GetVector();
        float playerRadius = p->GetRadius();
        return CheckCollisionCircleRec(playerPos, playerRadius, dot);
    }
public:
    Dot() : Dot(0, 0) {}
    Dot(float posX, float posY, Color c = LIGHTGRAY) {
        x = posX;
        y = posY;
        width = dotSize;
        height = dotSize;
        dot = Rectangle{ x, y, width, height };
        color = c;
    }
    void Draw() {
        DrawRectangleRec(dot, color);
    }
    void Col(Player* p) {
        if (isHit(p)) {
            color = p->GetFillColor();
        }
    }
};

void padColtrol(Player* p ,int n) {
    float PadX = 0;
    float PadY = 0;
    if (IsGamepadAvailable(n)) {
        PadX = GetGamepadAxisMovement(n, GAMEPAD_AXIS_LEFT_X);
        PadY = GetGamepadAxisMovement(n, GAMEPAD_AXIS_LEFT_Y);
        // cout << "P1 Pad X: " << p1PadX << " Y: " << p1PadY << endl;
        p->moveX(PadX > 0.2f || PadX < -0.2f ? PadX : 0);
        p->moveY(PadY > 0.2f || PadY < -0.2f ? PadY : 0);

        if (IsGamepadButtonDown(n, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            p->moveLeft(1.0f);
        }
        if (IsGamepadButtonDown(n, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            p->moveRight(1.0f);
        }
        if (IsGamepadButtonDown(n, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
            p->moveUp(1.0f);
        }
        if (IsGamepadButtonDown(n, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
            p->moveDown(1.0f);
        }
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "CSplatoon");
    SetTargetFPS(60);

    Player player1(100, 100, BLUE, SKYBLUE);
    Player player2(screenWidth - 100, screenHeight - 100, RED, PINK);


    int H = 0;
    int W = 0;
    const int numH = static_cast<int>(ceil(screenHeight / dotSize) );
    const int numW = static_cast<int>(ceil(screenWidth / dotSize) );
    vector<vector<Dot>> dots(numH, vector<Dot>(numW));
    cout << "numH: " << numH << " numW: " << numW << endl;
    while (H < numH) {
        W = 0;
        while (W < numW) {
            dots[H][W] = Dot(static_cast<float>(W) * dotSize, static_cast<float>(H) * dotSize);
            W++;
        }
        H++;
    }
    cout << "Dots initialized." << endl;

    float nowFPS = 0.0f;

    while (!WindowShouldClose()) {
        dt = GetFrameTime();

        padColtrol(&player1, 0);
        padColtrol(&player2, 1);

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

        if (IsKeyDown(KEY_LEFT)) {
            player2.moveLeft();
        }
        if (IsKeyDown(KEY_RIGHT)) {
            player2.moveRight();
        }
        if (IsKeyDown(KEY_UP)) {
            player2.moveUp();
        }
        if (IsKeyDown(KEY_DOWN)) {
            player2.moveDown();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            H = 0;
            while (H < numH) {
                W = 0;
                while (W < numW) {
                    dots[H][W].Col(&player1);
                    dots[H][W].Col(&player2);

                    dots[H][W].Draw();
                    W++;
                }
                H++;
            }

            player1.Draw();
            player2.Draw();
            // DrawText("Hello Raylib!", 280, 200, 20, DARKGRAY);
        nowFPS = GetFPS();
        DrawText(TextFormat("FPS: %.2f", nowFPS), 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
