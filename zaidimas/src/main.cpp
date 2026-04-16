#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>

#include "defines.h"
#include "objects.h"
#include "utils.h"

using namespace sf;
using namespace std;

class Companion {
private:
    Sprite spr;
    float x, y, dy;
    bool isJumping;

    // Animacijos kintamieji
    Texture* tex0; // Stein.png
    Texture* tex1; // Stein1.png
    Clock animClock; // Laikmatis animacijai
    int currentFrame; // 0 arba 1

public:
    // Konstruktorius dabar priima dvi tekstûras
    Companion(Texture& t0, Texture& t1) {
        tex0 = &t0;
        tex1 = &t1;
        currentFrame = 0;

        spr.setTexture(*tex0); // Pradinë tekstûra

        // Nustatome origin pagal tekstûros centrà (darome prielaidà, kad abi tekstûros vienodo dydþio)
        spr.setOrigin(static_cast<float>(tex0->getSize().x) / 2.0f, 0);

        x = 30.0f;
        y = GROUND_Y - static_cast<float>(tex0->getSize().y); // Automatinis aukðtis pagal akmená
        dy = 0;
        isJumping = false;
        animClock.restart(); // Paleidþiame animacijos laikmatá
    }

    void reset() {
        y = GROUND_Y - static_cast<float>(tex0->getSize().y);
        dy = 0;
        isJumping = false;
        currentFrame = 0;
        spr.setTexture(*tex0);
        animClock.restart();
    }

    void update(Plate plates[], float currentSpeed, float playerX) {
        // --- ANIMACIJOS LOGIKA ---
        if (animClock.getElapsedTime().asSeconds() > 0.12f) {
            currentFrame = (currentFrame + 1) % 2; // Perjungiam tarp 0 ir 1

            if (currentFrame == 0) spr.setTexture(*tex0);
            else spr.setTexture(*tex1);

            animClock.restart(); // Nunulinam laikmatá
        }

        x = playerX - 120.0f; // Tarpas

        // Gravitacija
        dy += 0.6f;
        y += dy;

        float groundLevel = GROUND_Y - static_cast<float>(tex0->getSize().y);
        if (y > groundLevel) {
            y = groundLevel;
            dy = 0;
            isJumping = false;
        }

        // AI Ðuolis
        for (int i = 0; i < PLATES_AMOUNT; ++i) {
            if (plates[i].x > x && plates[i].x < x + 150.0f && !isJumping) {
                dy = PLAYER_JUMP_V;
                isJumping = true;
            }
        }
        spr.setPosition(x, y);
    }

    void draw(RenderWindow& window) {
        window.draw(spr);
    }
};

void SetCactusY(Plate& cactus, Texture tCactus[]) {
    float cactusHeight = static_cast<float>(tCactus[cactus.type].getSize().y);
    cactus.y = GROUND_Y - cactusHeight;
}

void ResetGame(Player& player, Companion* buddy, Plate plates[], float& distanceScore, float& totalTime, float& currentSpeed, float& dy, Sprite& sprHouse, Texture tCactus[]) {
    player.lives = INITIAL_LIVES;
    player.x = 150.0f;
    player.y = GROUND_Y - 73.0f;
    dy = 0;
    distanceScore = 0;
    totalTime = 0;
    currentSpeed = 1.0f;
    sprHouse.setPosition(100.0f, GROUND_Y - 250.0f);

    if (buddy) buddy->reset();

    for (int i = 0; i < PLATES_AMOUNT; ++i) {
        plates[i].x = static_cast<float>(WINDOW_WIDTH) + 1200.0f + (i * (CACTUS_INTERVAL + 400.0f));
        plates[i].type = rand() % 3;
        SetCactusY(plates[i], tCactus);
    }
}

void UpdateGame(Player& player, Plate plates[], float& dy, float& distanceScore, Texture tCactus[], float dt, float totalTime, float& currentSpeed) {
    dy += 0.6f;
    player.y += dy;

    if (player.y > GROUND_Y - 73.0f) {
        player.y = GROUND_Y - 73.0f;
        dy = 0;
    }

    if (player.lives > 0) {
        if (currentSpeed < 7.0f) currentSpeed += 2.2f * dt;
        distanceScore += currentSpeed * dt;
    }

    if (totalTime > 3.0f && player.lives > 0) {
        float gameFlowSpeed = currentSpeed + (distanceScore / 250.0f);

        for (int i = 0; i < PLATES_AMOUNT; ++i) {
            plates[i].x -= gameFlowSpeed;

            if (utils::CheckCollision(player, plates[i])) {
                player.lives--;
                int prevIdx = (i == 0) ? PLATES_AMOUNT - 1 : i - 1;
                plates[i].x = plates[prevIdx].x + CACTUS_INTERVAL + (rand() % 300 + 100);
                plates[i].type = rand() % 3;
                SetCactusY(plates[i], tCactus);
            }

            if (plates[i].x < -150.0f) {
                int prevIdx = (i == 0) ? PLATES_AMOUNT - 1 : i - 1;
                plates[i].x = max(plates[prevIdx].x + CACTUS_INTERVAL + (rand() % 200), static_cast<float>(WINDOW_WIDTH) + 100);
                plates[i].type = rand() % 3;
                SetCactusY(plates[i], tCactus);
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    RenderWindow* app = new RenderWindow(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "scape stein");
    app->setFramerateLimit(60);

    // Tekstûros (Rodyklës)
    Texture* tBack = new Texture();
    Texture* tPlayer = new Texture();
    Texture* tStein0 = new Texture(); // Nauja: stein.png
    Texture* tStein1 = new Texture(); // Nauja: stein1.png
    Texture* tHouse = new Texture();
    Texture* tHeart = new Texture();
    Texture* tLogo = new Texture();
    Texture* tButton = new Texture();
    Texture* tPlayAgain = new Texture();
    Texture* tMainMenu = new Texture();
    Texture tCactus[3];

    // Pakeistas uþkrovimas: vietoj doodle1 -> stein ir stein1
    if (!tBack->loadFromFile("resources/background.png") ||
        !tPlayer->loadFromFile("resources/player1.png") ||
        !tStein0->loadFromFile("resources/stein.png") ||  // <--- PAKEISTA
        !tStein1->loadFromFile("resources/stein1.png") || // <--- PAKEISTA
        !tHouse->loadFromFile("resources/house.png") ||
        !tHeart->loadFromFile("resources/heart.png") ||
        !tLogo->loadFromFile("resources/logo.png") ||
        !tButton->loadFromFile("resources/button.png") ||
        !tPlayAgain->loadFromFile("resources/playagain.png") ||
        !tMainMenu->loadFromFile("resources/mainmenu.png") ||
        !tCactus[0].loadFromFile("resources/cactus.png") ||
        !tCactus[1].loadFromFile("resources/cactus2.png") ||
        !tCactus[2].loadFromFile("resources/cactus3.png")) {
        cerr << "Klaida: Nepavyko rasti resursu aplanke resources/!" << endl;
        return -1;
    }

    // Objektai
    // Kompanionui perduodame abi tekstûras
    Companion* buddy = new Companion(*tStein0, *tStein1);

    Sprite sprBack(*tBack), sprPlayer(*tPlayer), sprHouse(*tHouse), sprHeart(*tHeart);
    Sprite sprCactus[PLATES_AMOUNT];
    Sprite sprLogo(*tLogo), sprButton(*tButton), sprPlayAgain(*tPlayAgain), sprMainMenu(*tMainMenu);

    // Nustatymai
    sprLogo.setOrigin(75.0f, 75.0f);
    sprLogo.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 60.0f);
    sprButton.setOrigin(75.0f, 20.0f);
    sprButton.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 70.0f);
    sprPlayAgain.setOrigin(75.0f, 20.0f);
    sprPlayAgain.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 30.0f);
    sprMainMenu.setOrigin(75.0f, 20.0f);
    sprMainMenu.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 85.0f);
    sprPlayer.setOrigin(PLAYER_WIDTH / 2.0f, 0);

    Font font;
    font.loadFromFile("resources/arialbd.ttf");
    Text uiText;
    uiText.setFont(font);
    uiText.setCharacterSize(30);

    Player player;
    Plate plates[PLATES_AMOUNT];
    float dy = 0, distanceScore = 0, totalTime = 0, currentSpeed = 1.0f;
    bool isGameStarted = false;

    ResetGame(player, buddy, plates, distanceScore, totalTime, currentSpeed, dy, sprHouse, tCactus);

    Clock gameClock;

    while (app->isOpen()) {
        float dt = gameClock.restart().asSeconds();
        Event e;
        while (app->pollEvent(e)) {
            if (e.type == Event::Closed) app->close();

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = app->mapPixelToCoords(Mouse::getPosition(*app));
                if (!isGameStarted) {
                    if (sprButton.getGlobalBounds().contains(mousePos)) isGameStarted = true;
                }
                else if (player.lives <= 0) {
                    if (sprPlayAgain.getGlobalBounds().contains(mousePos)) {
                        ResetGame(player, buddy, plates, distanceScore, totalTime, currentSpeed, dy, sprHouse, tCactus);
                    }
                    if (sprMainMenu.getGlobalBounds().contains(mousePos)) {
                        isGameStarted = false;
                        ResetGame(player, buddy, plates, distanceScore, totalTime, currentSpeed, dy, sprHouse, tCactus);
                    }
                }
            }

            if (isGameStarted && player.lives > 0 && e.type == Event::KeyPressed) {
                if ((e.key.code == Keyboard::Space || e.key.code == Keyboard::Up) && dy == 0)
                    dy = PLAYER_JUMP_V;
            }
        }

        if (isGameStarted && player.lives > 0) {
            totalTime += dt;
            UpdateGame(player, plates, dy, distanceScore, tCactus, dt, totalTime, currentSpeed);
            buddy->update(plates, currentSpeed, player.x);
            sprHouse.move(-currentSpeed * 1.1f, 0);
        }

        app->clear();

        if (!isGameStarted) {
            app->draw(sprBack);
            app->draw(sprLogo);
            app->draw(sprButton);
        }
        else if (player.lives > 0) {
            app->draw(sprBack);
            if (sprHouse.getPosition().x + 300 > 0) app->draw(sprHouse);

            for (int i = 0; i < PLATES_AMOUNT; ++i) {
                sprCactus[i].setTexture(tCactus[plates[i].type], true);
                sprCactus[i].setPosition(plates[i].x, plates[i].y);
                app->draw(sprCactus[i]);
            }

            buddy->draw(*app); // Pieðiame animuotà akmená
            sprPlayer.setPosition(player.x, player.y);
            app->draw(sprPlayer);

            uiText.setFillColor(Color::Black);
            uiText.setString("Score: " + to_string(static_cast<int>(distanceScore)));
            uiText.setPosition(20, 10);
            app->draw(uiText);

            for (int i = 0; i < player.lives; i++) {
                sprHeart.setPosition(WINDOW_WIDTH - 150.0f + (i * 45.0f), 15.0f);
                app->draw(sprHeart);
            }
        }
        else {
            app->clear(Color(0, 192, 255));
            uiText.setFillColor(Color::White);
            uiText.setString("SCORE: " + to_string(static_cast<int>(distanceScore)));
            FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            uiText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 50.0f);
            app->draw(uiText);
            app->draw(sprPlayAgain);
            app->draw(sprMainMenu);
        }
        app->display();
    }

    // Rankinis iðvalymas (Destruction)
    delete tBack; delete tPlayer; delete tStein0; delete tStein1; delete tHouse;
    delete tHeart; delete tLogo; delete tButton; delete tPlayAgain;
    delete tMainMenu; delete buddy; delete app;

    return 0;
}