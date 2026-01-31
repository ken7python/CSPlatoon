// cmake -S . -B build && cmake --build build
// open build/CSplatoon.app
#include "raylib.h"
#include <iostream>
#include <vector>

using namespace std;

float dt;

const int screenWidth = 1280;
const int screenHeight = 720;

class pArg {
private:
    Color color;
    int pid;
public:
    int getId() {
        return pid;
    }
    Color getColor() {
        return color;
    }
    pArg(int id, Color c = WHITE) {
        pid = id;
        color = c;
    }
};

class Player {
private:
    float x;
    float y;
    int speedX;
    int speedY;
    float radius;
    Color color;
    // Color fillColor;
    pArg arg;

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

    // Color GetFillColor() {
    //     return arg.getColor();
    // }
    pArg GetArg() {
        return arg;
    }

    Player(float posx, float posy, Color c,pArg parg) : arg(parg) {
        x = posx;
        y = posy;
        speedX = 720.0f;
        speedY = 720.0f;
        color = c;
        radius = 48.0f;
    }
};

const float dotSize = 5.0f;
class Dot {
private:
    float x;
    float y;
    float width;
    float height;
    pArg arg;
    Color color;
    Rectangle dot;

    bool isHit(Player* p) {
        Vector2 playerPos = p->GetVector();
        float playerRadius = p->GetRadius();
        return CheckCollisionCircleRec(playerPos, playerRadius, dot);
    }
public:
    Dot() : Dot(0, 0) {}
    Dot(float posX, float posY) : arg(-1, LIGHTGRAY) {
        x = posX;
        y = posY;
        width = dotSize;
        height = dotSize;
        dot = Rectangle{ x, y, width, height };
        // color = c;
        // arg = pArg(-1, WHITE);
    }
    void Draw() {
        DrawRectangleRec(dot, arg.getColor());
    }
    void Col(Player* p) {
        if (isHit(p)) {
            // color = p->GetFillColor();
            arg = p->GetArg();
        }
    }
    bool isPlayers(pArg* p) {
        return p->getId() == arg.getId();
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
    InitAudioDevice();
    Music bgm = LoadMusicStream("assets/bgm.mp3");
    Music ending = LoadMusicStream("assets/ending.mp3");
    SetTargetFPS(60);

    pArg argP1 = pArg(0, SKYBLUE);
    Player player1(100, 100, BLUE, argP1);

    pArg argP2 = pArg(1, PINK);
    Player player2(screenWidth - 100, screenHeight - 100, RED, argP2);


    int H = 0;
    int W = 0;
    const int numH = static_cast<int>(ceil(screenHeight / dotSize) );
    const int numW = static_cast<int>(ceil(screenWidth / dotSize) );
    int allDots = numH * numW;

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
    float time = 60.0f;
    int pBlue;
    int pRed;

    PlayMusicStream(bgm);
    // ゲームシーン
    while (!WindowShouldClose()) {
        UpdateMusicStream(bgm);
        dt = GetFrameTime();
        time -= dt;
        if (time <= 0.0f) {
            break;
        }

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

            pBlue = 0;
            pRed = 0;
            H = 0;
            while (H < numH) {
                W = 0;
                while (W < numW) {
                    Dot* target = &(dots[H][W]);
                    target->Col(&player1);
                    target->Col(&player2);
                    target->Draw();

                    target->isPlayers(&argP1) ? pBlue++ : 0;
                    target->isPlayers(&argP2) ? pRed++ : 0;

                    W++;
                }
                H++;
            }

            cout << "Blue: " << pBlue << " Red: " << pRed << endl;

            player1.Draw();
            player2.Draw();
            // DrawText("Hello Raylib!", 280, 200, 20, DARKGRAY);
        nowFPS = GetFPS();
        DrawText(TextFormat("FPS: %.2f", nowFPS), 10, 10, 20, BLACK);
        DrawText(TextFormat("Time: %.1f", time), screenWidth - 360, 10, 64, BLACK);

        EndDrawing();
    }

    //　ゲーム終了シーン
    StopMusicStream(bgm);
    PlayMusicStream(ending);
    while (!WindowShouldClose()) {
        UpdateMusicStream(ending);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        pBlue = 0;
        pRed = 0;
        H = 0;
        while (H < numH) {
            W = 0;
            while (W < numW) {
                Dot* target = &(dots[H][W]);
                target->Col(&player1);
                target->Col(&player2);
                target->Draw();

                target->isPlayers(&argP1) ? pBlue++ : 0;
                target->isPlayers(&argP2) ? pRed++ : 0;

                W++;
            }
            H++;
        }

        // cout << "Blue: " << pBlue << " Red: " << pRed << endl;

        player1.Draw();
        player2.Draw();

        nowFPS = GetFPS();
        DrawText(TextFormat("FPS: %.2f", nowFPS), 10, 10, 20, BLACK);

        int perBlue = (100 * pBlue) / allDots;
        int perRed = (100 * pRed) / allDots;
        DrawText(TextFormat("%d%%", perBlue), screenWidth / 2 - 64 - 256, screenHeight / 2 - 64, 128, BLUE);
        DrawText("vs", screenWidth / 2 - 32, screenHeight / 2 - 32, 64, BLACK);
        DrawText(TextFormat("%d%%", perRed ), screenWidth / 2 + 64 + 32,screenHeight / 2 - 64, 128, RED);
        {
            // 勝利メッセージをフォントサイズ 64 で中央に表示
            const int winFontSize = 64;
            const char* blueText = "BLUE WINS!";
            const char* redText = "RED WINS!";
            const char* drawText = "DRAW!";
            if (perBlue > perRed) {
                int tw = MeasureText(blueText, winFontSize);
                DrawText(blueText, screenWidth/2 - tw/2, screenHeight/2 + 64, winFontSize, BLUE);
            } else if (perRed > perBlue) {
                int tw = MeasureText(redText, winFontSize);
                DrawText(redText, screenWidth/2 - tw/2, screenHeight/2 + 64, winFontSize, RED);
            } else {
                int tw = MeasureText(drawText, winFontSize);
                DrawText(drawText, screenWidth/2 - tw/2, screenHeight/2 + 64, winFontSize, DARKGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}