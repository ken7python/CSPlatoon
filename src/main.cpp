// cmake -S . -B build && cmake --build build
// open build/CSplatoon.app
// c++ main.cpp -O3 -o game -lraylib -std=c++17 && ./game
#include "raylib.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <raymath.h>

using namespace std;

float dt;

const int screenWidth = 1280;
const int screenHeight = 720;
const int bulletsMax = 5;

class pArg {
private:
    Color color;
    Color bColor;
    int pid;
public:
    int getId() {
        return pid;
    }
    Color getColor() {
        return color;
    }
    Color getBColor() {
        return bColor;
    }
    pArg(int id, Color c = WHITE,Color bulletColor = WHITE) {
        pid = id;
        color = c;
        bColor = bulletColor;
    }
};

class Player {
private:
    float x;
    float y;
    float xv;
    float yv;
    bool isBounce = false;
    float bounceWall = 4.0f;
    int speedX;
    int speedY;
    float radius;
    Color color;
    // Color fillColor;
    pArg arg;

public:
    void Draw() {
        if (abs(xv) > 0.0f) {
            x = x + xv * dt;
            xv = xv * 0.8f;
        }
        if (abs(yv) > 0.0f) {
            y = y + yv * dt;
            yv = yv * 0.8f;
        }

        float bounceMax = 2000.0f;
        // 壁跳ね返り
        if (x < 0) {
            x = 0;
            xv = bounceWall * abs(xv);
            if (bounceMax < abs(xv)) {
                xv = bounceMax;
            }
            cout << "XV" << xv << endl;
        }
        if (x > screenWidth) {
            x = static_cast<float>(screenWidth);
            xv = -1 * bounceWall * abs(xv);
            if (bounceMax < abs(xv)) {
                xv = -1 * bounceMax;
            }
            cout << "XV" << xv << endl;
        }

        if (y < 0) {
            y = 0;
            yv = bounceWall * abs(yv);
            if (bounceMax < abs(yv)) {
                yv = bounceMax;
            }
            cout << "YV" << yv << endl;
        }
        if (y > screenHeight) {
            y = static_cast<float>(screenHeight);
            yv = -bounceWall * abs(yv);
            if (bounceMax < abs(yv)) {
                yv = -bounceMax;
            }
            cout << "YV" << yv << endl;
        }

        DrawCircle(GetX(), GetY(), radius, color);
    }

    void moveRight(float a = 1.0f) {
        if (x + radius / 2 < screenWidth) {
            // x += speedX * dt * a;
            xv += speedX * a;
        }
    }
    void moveLeft(float a = 1.0f) {
        if (0 < x - radius / 2) {
            // x -= speedX * dt * a;
            xv -= speedX * a;
        }
    }
    void moveUp(float a = 1.0f) {
        if (0 < y - radius / 2) {
            // y -= speedY * dt * a;
            yv -= speedY * a;
        }
    }
    void moveDown(float a = 1.0f) {
        if (y + radius / 2 < screenHeight) {
            // y += speedY * dt * a;
            yv += speedY * a;
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
    float GetXV() {
        return xv;
    }
    float GetYV() {
        return yv;
    }
    bool GetIsBounce() {
        return isBounce;
    }

    Vector2 GetVector() {
        return Vector2{ x, y };
    }
    float GetRadius() {
        return radius;
    }

    bool isHit(Player* p) {
        Vector2 otherPos = p->GetVector();
        float otherRadius = p->GetRadius();
        return CheckCollisionCircles(GetVector(), radius, otherPos, otherRadius);
    }

    void pBounce(Player* p, Sound* coll) {
        if (isHit(p) && !isBounce && !p->GetIsBounce()) {
            isBounce = true;
            p->isBounce = true;

            // パターン1
            // float bouceForce = 8.0f;
            // Vector2 pos1 = GetVector();
            // Vector2 pos2 = p->GetVector();
            // Vector2 dir = Vector2Subtract(pos1, pos2);
            // dir = Vector2Normalize(dir);
            // moveX(dir.x * bouceForce);
            // moveY(dir.y * bouceForce);
            // p->moveX(-dir.x * bouceForce);
            // p->moveY(-dir.y * bouceForce);

            // パターン2
            float dx = p->x - x;
            float dy = p->y - y;
            float dl = sqrt(dx*dx + dy*dy);
            float dxu = dx / dl;
            float dyu = dy / dl;

            float dot1 = xv*(+dxu) + yv*(+dyu);
            float dot2 = p->xv*(-dxu) + p->yv*(-dyu);

            dot1 = dot1 * 1.8f;  // for Impact deform
            dot2 = dot2 * 1.8f;  // for Impact deform

            xv = 0;
            yv = 0;
            xv = xv + (-dxu)*dot2 - (+dxu)*dot1;
            yv = yv + (-dyu)*dot2 - (+dyu)*dot1;
            p->xv = p->xv + (+dxu)*dot1 - (-dxu)*dot2;
            p->yv = p->yv + (+dyu)*dot1 - (-dyu)*dot2;
            PlaySound(*coll);
        } else if (!isHit(p)) {
            isBounce = false;
            p->isBounce = false;
        }
    }
    // Color GetFillColor() {
    //     return arg.getColor();
    // }
    pArg GetArg() {
        return arg;
    }

    void setPosition(float posx, float posy) {
        x = posx;
        y = posy;
    }
    void damage() {
        radius = radius - 4.0f;
        if (radius < 8.0f) {
            radius = 8.0f;
        }
    }
    void bigger() {
        radius = radius + 4.0f;
        if (96.0f < radius) {
            radius = 96.0f;
        }
    }

    void init() {
        radius = 48.0f;
    }

    Player(float posx, float posy, Color c,pArg parg) : arg(parg) {
        x = posx;
        y = posy;
        xv = 0.0f;
        yv = 0.0f;
        speedX = 200.0f;
        speedY = 200.0f;
        color = c;
        radius = 48.0f;
    }
};

class Bullet {
private:
    float x;
    float y;
    float xv;
    float yv;
    float radius;
    pArg arg;
    Sound damageSound;
public:
    void Draw() {
        x = x + xv * dt;
        y = y + yv * dt;
        DrawCircle(static_cast<int>(x), static_cast<int>(y), radius, arg.getBColor());
    }
    Vector2 GetVector() {
        return Vector2{ x, y };
    }
    pArg GetArg() {
        return arg;
    }
    float GetRadius() {
        return radius;
    }
    bool isHit(Player* p) {
        Vector2 playerPos = p->GetVector();
        float playerRadius = p->GetRadius();
        return CheckCollisionCircles(GetVector(), radius, playerPos, playerRadius);
    }
    void stop() {
        xv = 0.0f;
        yv = 0.0f;
    }
    void damage() {
        SetSoundVolume(damageSound, 2.5f);
        PlaySound(damageSound);
    }

    Bullet(Player *player, Player* enemy) : arg(player->GetArg()) {
        Vector2 pos = player->GetVector();
        Vector2 enemyPos = enemy->GetVector();
        Sound shot = LoadSound("assets/shot.mp3");
        damageSound = LoadSound("assets/damage.mp3");
        x = pos.x;
        y = pos.y;
        xv = 1.25f * (enemyPos.x - pos.x);
        yv = 1.25f * (enemyPos.y - pos.y);
        // radius = 20.0f;
        radius = player->GetRadius() / 4.0f;
        if (radius < 8.0f) {
            radius = 8.0f;
        }
        SetSoundVolume(shot, 2.5f);
        PlaySound(shot);
    }
};

const int dotSize = 5;
class Dot {
private:
    int x;
    int y;
    // float width;
    // float height;
    pArg arg;
    Color color;
    Rectangle dot;

    bool isHit(Player* p) {
        Vector2 playerPos = p->GetVector();
        float playerRadius = p->GetRadius();
        return CheckCollisionCircleRec(playerPos, playerRadius, dot);
    }
    bool isHitB(Bullet *b){
        if (b->GetArg().getId() == arg.getId()) return false;
        Vector2 bPos = b->GetVector();
        float bRadius = b->GetRadius();
        return CheckCollisionCircleRec(bPos, bRadius, dot);
    }
public:
    Dot() : Dot(0, 0) {}
    Dot(int posX, int posY) : arg(-1, LIGHTGRAY) {
        // width = dotSize;
        // height = dotSize;
        dot = Rectangle{ static_cast<float>(posX), static_cast<float>(posY), dotSize, dotSize };
        // color = c;
        // arg = pArg(-1, WHITE);
    }
    void Draw() {
        DrawRectangleRec(dot, arg.getColor());
    }
    void Col(Player* p, vector<Bullet>* bullets) {
        if (isHit(p)) {
            // color = p->GetFillColor();
            arg = p->GetArg();
        }
        for (auto& b : *bullets) {
            if (isHitB(&b)) {
                arg = p->GetArg();
            }
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

void WASDcontrol(Player* p) {
    if (IsKeyDown(KEY_A)) {
        p->moveLeft();
    }
    if (IsKeyDown(KEY_D)) {
        p->moveRight();
    }
    if (IsKeyDown(KEY_W)) {
        p->moveUp();
    }
    if (IsKeyDown(KEY_S)) {
        p->moveDown();
    }
}

void ArrowControl(Player* p) {
    if (IsKeyDown(KEY_LEFT)) {
        p->moveLeft();
    }
    if (IsKeyDown(KEY_RIGHT)) {
        p->moveRight();
    }
    if (IsKeyDown(KEY_UP)) {
        p->moveUp();
    }
    if (IsKeyDown(KEY_DOWN)) {
        p->moveDown();
    }
}

int H = 0;
int W = 0;
const int numH = static_cast<int>(ceil(screenHeight / dotSize) );
const int numW = static_cast<int>(ceil(screenWidth / dotSize) );

int allDots = numH * numW;

vector<vector<Dot>> dots(numH, vector<Dot>(numW));

void initDots() {
    H = 0;
    while (H < numH) {
        W = 0;
        while (W < numW) {
            // dots[H][W] = Dot(static_cast<float>(W) * dotSize, static_cast<float>(H) * dotSize);
            dots[H][W] = Dot(W * dotSize, H * dotSize);
            W++;
            // cout << "Initializing Dots: " << (H * numW + W) << "/" << allDots << "\r";
        }
        H++;
    }
    cout << "Dots:" << allDots << endl;
    cout << "Dots initialized." << endl;
}

void DrawBullets(vector<Bullet>* bullets,Player* me,Player* enemy) {
    for (auto& b : *bullets) {
        Vector2 pos = b.GetVector();
        if (b.isHit(enemy)) {
            // 当たった弾を削除
            b.damage();
            bullets->erase(bullets->begin());
            enemy->damage();
            me->bigger();
            continue;
        }
        if (pos.x < -50 || screenWidth + 50 < pos.x || pos.y < -50 || screenHeight + 50 < pos.y) {
            // 画面外に出た弾を削除
            bullets->erase(bullets->begin());
            continue;
        }
        b.Draw();
    }
}
void initBullets(vector<Bullet>* bullets) {
    bullets->clear();
}

void DrawTextCenterEx(Font font, const char* text, float x, float y, float fontSize, Color color) {
    Vector2 textPos = { x, y };
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.0f);
    textPos.x -= textSize.x / 2.0f;
    textPos.y -= textSize.y / 2.0f;
    DrawTextEx(font, text, textPos, fontSize, 1.0f, color);
}

void DrawTextRightEx(Font font, const char* text, float x, float y, float fontSize, Color color) {
    Vector2 textPos = { x, y };
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.0f);
    textPos.x -= textSize.x;
    DrawTextEx(font, text, textPos, fontSize, 1.0f, color);
}

void DrawTextLeftEx(Font font, const char* text, float x, float y, float fontSize, Color color) {
    Vector2 textPos = { x, y };
    DrawTextEx(font, text, textPos, fontSize, 1.0f, color);
}

int main() {
#if defined(__APPLE__)
    {
        // For macOS .app: ensure assets are loaded from Contents/Resources
        std::filesystem::path appDir = GetApplicationDirectory();
        std::filesystem::path resourcesDir = appDir / ".." / "Resources";
        std::error_code ec;
        if (std::filesystem::exists(resourcesDir, ec)) {
            std::filesystem::current_path(resourcesDir, ec);
        }
    }
#endif
    InitWindow(screenWidth, screenHeight, "C-Spraytoon");
    InitAudioDevice();
    SetTargetFPS(120);

    Music opening = LoadMusicStream("assets/opening.mp3");
    Music ending = LoadMusicStream("assets/ending.mp3");
    Music bgm = LoadMusicStream("assets/bgm.mp3");
    Sound coll = LoadSound("assets/coll.mp3");
    SetSoundVolume(coll, 10.0f);

    pArg argP1 = pArg(0, SKYBLUE, DARKBLUE);
    Player player1(100, 100, BLUE, argP1);
    vector<Bullet> bullets1;

    pArg argP2 = pArg(1, PINK, MAROON);
    vector<Bullet> bullets2;

    Player player2(screenWidth - 100, screenHeight - 100, RED, argP2);

    cout << "numH: " << numH << " numW: " << numW << endl;

    initDots();

    float nowFPS = 0.0f;
    float time;
    int pBlue;
    int pRed;

    Texture2D logo = LoadTexture("assets/logo.png");
    // Font font = LoadFontEx("assets/font.ttf", 96, 0, 0);
    std::vector<int> cps;

    // ASCII全部
    for(int i=0x20;i<=0x7E;i++){
        cps.push_back(i);
    }

    // ASCII（英数字・記号）
    for(int i = 0x20; i <= 0x7E; i++){
        cps.push_back(i);
    }

    // ひらがな
    for(int i = 0x3040; i <= 0x309F; i++){
        cps.push_back(i);
    }

    // カタカナ
    for(int i = 0x30A0; i <= 0x30FF; i++){
        cps.push_back(i);
    }

    // 句読点追加
    cps.push_back(0x3001); // 、
    cps.push_back(0x3002); // 。

    // 「秒」だけ追加
    cps.push_back(0x79D2);

    Font font = LoadFontEx(
        "assets/font.ttf",
        96,
        cps.data(),
        cps.size()
    );


    if (font.texture.id == 0) {
        printf("フォントロード失敗\n");
    }

    // ゲーム開始
    while (!WindowShouldClose()) {
        time = 60.0f;

        player1.setPosition(100.0f, 100.0f);
        player1.init();
        player2.setPosition(screenWidth - 100.0f, screenHeight - 100.0f);
        player2.init();

        initDots();
        initBullets(&bullets1);
        initBullets(&bullets2);

        // タイトルシーン
        PlayMusicStream(opening);
        SetMusicVolume(opening, 1.5f);
        while (!WindowShouldClose()) {
            UpdateMusicStream(opening);
            dt = GetFrameTime();

            // プレイヤー操作1
            player1.pBounce(&player2, &coll);
            padColtrol(&player1, 0);
            WASDcontrol(&player1);
            if (IsKeyPressed(KEY_F) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                if (bullets1.size() < bulletsMax) bullets1.push_back(Bullet(&player1, &player2));
            }

            // プレイヤー操作2
            player2.pBounce(&player1, &coll);
            padColtrol(&player2, 1);
            ArrowControl(&player2);
            if (IsKeyPressed(KEY_K) || IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                if (bullets2.size() < bulletsMax) bullets2.push_back(Bullet(&player2, &player1));
            }


            // マス描画
            BeginDrawing();
            ClearBackground(RAYWHITE);

            pBlue = 0;
            pRed = 0;
            H = 0;
            while (H < numH) {
                W = 0;
                while (W < numW) {
                    Dot* target = &(dots[H][W]);
                    target->Col(&player1, &bullets1);
                    target->Col(&player2, &bullets2);
                    target->Draw();

                    target->isPlayers(&argP1) ? pBlue++ : 0;
                    target->isPlayers(&argP2) ? pRed++ : 0;

                    W++;
                }
                H++;
            }


            // ロゴ描画
            DrawTexture(logo, screenWidth / 2 - logo.width / 2, screenHeight / 2 - logo.height / 1.5f, WHITE);

            // スタートボタン描画
            Rectangle startBtn = Rectangle{static_cast<float>(screenWidth / 2 - 150) ,static_cast<float>(screenHeight / 2 + 64), 300.0f, 64.0f};
            bool p1Start = CheckCollisionCircleRec(player1.GetVector(), player1.GetRadius(), startBtn);
            bool p2Start = CheckCollisionCircleRec(player2.GetVector(), player2.GetRadius(), startBtn);
            if (p1Start && p2Start) break;
            DrawRectangleRec(startBtn, p1Start || p2Start ? DARKGREEN : RAYWHITE);
            DrawTextCenterEx(font, "スタート", screenWidth / 2.0f, screenHeight / 2.0f + 96.0f, 32.0f, BLACK);

            //　説明描画
            DrawTextCenterEx(font, "あそぶには、ふたりともここにとまってね", screenWidth / 2.0f, screenHeight / 2.0f + 160.0f, 24.0f, DARKGRAY);

            DrawRectangle(screenWidth / 2 - 300, screenHeight - 150, 600, 125, WHITE);
            DrawTextCenterEx(font, "P1: WASDキー + Fキー", screenWidth / 2.0f, screenHeight - 130.0f, 24.0f, BLUE);
            DrawTextCenterEx(font, "P2: やじるしキー + Kキー", screenWidth / 2.0f, screenHeight - 100.0f, 24.0f, RED);
            DrawTextCenterEx(font, "P1 & P2:ジョイスティックでいどう、あかボタンでシュート", screenWidth / 2.0f, screenHeight - 70.0f, 24.0f, BLACK);
            DrawTextCenterEx(font, "たくさん、ぬりつぶしたほうが、かちだよ",screenWidth / 2.0f, screenHeight - 40.0f, 24.0f, BLACK);
            // cout << "Blue: " << pBlue << " Red: " << pRed << endl;

            // プレイヤー描画
            player1.Draw();
            DrawBullets(&bullets1, &player1, &player2);

            player2.Draw();
            DrawBullets(&bullets2, &player2, &player1);

            nowFPS = GetFPS();
            // DrawText(TextFormat("FPS: %.2f", nowFPS), 10, 10, 20, BLACK);
            // DrawTextEx(font,TextFormat("FPS:%.2f", nowFPS), {10.0f,10.0f}, 20.0f, 1, BLACK);
            DrawTextLeftEx(font, TextFormat("FPS:%.2f", nowFPS), 10.0f,10.0f, 28.0f, BLACK);

            EndDrawing();
        }

        // ゲームシーン
        player1.setPosition(100.0f, 100.0f);
        player1.init();
        player2.setPosition(screenWidth - 100.0f, screenHeight - 100.0f);
        player2.init();

        initDots();
        initBullets(&bullets1);
        initBullets(&bullets2);

        StopMusicStream(opening);
        SetMusicPitch(bgm, 1.0f);
        PlayMusicStream(bgm);
        while (!WindowShouldClose()) {
            if (time <= 10.0f) {
                SetMusicPitch(bgm, 1.1f);
            }
            UpdateMusicStream(bgm);
            dt = GetFrameTime();
            time -= dt;
            if (time <= 0.0f) {
                break;
            }

            player1.pBounce(&player2, &coll);
            padColtrol(&player1, 0);
            WASDcontrol(&player1);
            if (IsKeyPressed(KEY_F) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                if (bullets1.size() < bulletsMax) bullets1.push_back(Bullet(&player1, &player2));
            }

            player2.pBounce(&player1, &coll);
            padColtrol(&player2, 1);
            ArrowControl(&player2);
            if (IsKeyPressed(KEY_K) || IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                if (bullets2.size() < bulletsMax) bullets2.push_back(Bullet(&player2, &player1));
            }

            // Bounce(&player1, &player2);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            pBlue = 0;
            pRed = 0;
            H = 0;
            while (H < numH) {
                W = 0;
                while (W < numW) {
                    Dot* target = &(dots[H][W]);
                    target->Col(&player1, &bullets1);
                    target->Col(&player2, &bullets2);
                    target->Draw();

                    target->isPlayers(&argP1) ? pBlue++ : 0;
                    target->isPlayers(&argP2) ? pRed++ : 0;

                    W++;
                }
                H++;
            }

            // cout << "Blue: " << pBlue << " Red: " << pRed << endl;

            player1.Draw();
            DrawBullets(&bullets1,&player1,&player2);

            player2.Draw();
            DrawBullets(&bullets2, &player2,&player1);

            // DrawText("Hello Raylib!", 280, 200, 20, DARKGRAY);
            nowFPS = GetFPS();
            DrawTextLeftEx(font, TextFormat("FPS:%.2f", nowFPS), 10.0f,10.0f, 28.0f, BLACK);

            // DrawText(TextFormat("Time: %.2f", time), screenWidth - 360, 10, 64, BLACK);

            string timeTextJ = time <= 55 ? "ラスト:" : "あと:";
            string timeText = timeTextJ + "%.2f秒";
            // DrawTextEx(font, TextFormat(timeText.c_str(), time), {screenWidth - 60 * 6,10.0f}, 64.0f, 1.0f, BLACK);
            DrawTextRightEx(font, TextFormat(timeText.c_str(), time), static_cast<float>(screenWidth - 20),10.0f, 64.0f, BLACK);

            EndDrawing();
        }

        //　ゲーム終了シーン
        StopMusicStream(bgm);
        SetMusicVolume(ending, 1.25f);
        PlayMusicStream(ending);
        for (auto & b : bullets1) {
            b.stop();
        }
        for (auto & b : bullets2) {
            b.stop();
        }

        while (!WindowShouldClose()) {
            UpdateMusicStream(ending);

            if (IsKeyPressed(KEY_SPACE)) break;
            BeginDrawing();
            ClearBackground(RAYWHITE);

            pBlue = 0;
            pRed = 0;
            H = 0;
            while (H < numH) {
                W = 0;
                while (W < numW) {
                    Dot* target = &(dots[H][W]);
                    target->Col(&player1, &bullets1);
                    target->Col(&player2, &bullets2);
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

            // 試合結果描画
            // DrawTextEx(font,TextFormat("FPS:%.2f", nowFPS), {10.0f,10.0f}, 20.0f, 1, BLACK);
            DrawTextLeftEx(font, TextFormat("FPS:%.2f", nowFPS), 10.0f,10.0f, 28.0f, BLACK);

            int perBlue = (100 * pBlue) / allDots;
            int perRed = (100 * pRed) / allDots;

            DrawTextRightEx(font, TextFormat("%d%%", perBlue), screenWidth / 2.0f - 64.0f, screenHeight / 2.0f - 64.0f, 128.0f, BLUE);
            DrawTextCenterEx(font, "vs", screenWidth / 2.0f, screenHeight / 2.0f, 64.0f, BLACK);
            DrawTextLeftEx(font, TextFormat("%d%%", perRed ), screenWidth / 2.0f + 64.0f, screenHeight / 2.0f - 64.0f, 128.0f, RED);
            {
                // 勝利メッセージをフォントサイズ 64 で中央に表示
                const int winFontSize = 64;
                const char* blueText = "あおのかち";
                const char* redText = "あかのかち";
                const char* drawText = "DRAW!";
                float x = screenWidth / 2.0f;
                float y = screenHeight / 2.0f + 128.0f;
                if (perBlue > perRed) {
                    DrawTextCenterEx(font, blueText, x, y, winFontSize, BLUE);
                } else if (perRed > perBlue) {
                    DrawTextCenterEx(font, redText, x, y, winFontSize, RED);
                } else {
                    DrawTextCenterEx(font, drawText, x, y, winFontSize, DARKGRAY);
                }
                // DrawText("Press [SPACE] to Restart", screenWidth / 2 - 200, screenHeight - 100, 32, DARKGRAY);
                // DrawTextEx(font, "Press [SPACE] to Restart", {screenWidth / 2 - 200, screenHeight - 100}, 32, 1, DARKGRAY);
                DrawTextCenterEx(font, "スペースキーでリトライ", screenWidth / 2.0f, screenHeight - 100.0f, 32.0f, DARKGRAY);
            }
            // 試合結果ここまで

            EndDrawing();
        }
        StopMusicStream(ending);
    }

    CloseWindow();
    return 0;
}
