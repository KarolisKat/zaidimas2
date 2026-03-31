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

// Funkcija kaktuso aukðèiui nustatyti
void SetCactusY(Plate& cactus, Texture tCactus[]) {
    float cactusHeight = (float)tCactus[cactus.type].getSize().y;
    cactus.y = GROUND_Y - cactusHeight;
}

// Funkcija visiems kintamiesiems atstatyti (Restart)
void ResetGame(Player& player, Plate plates[], float& distanceScore, float& totalTime, float& currentSpeed, float& dy, Sprite& sprHouse, Texture tCactus[]) {
    player.lives = INITIAL_LIVES;
    player.x = 150.0f;
    player.y = GROUND_Y - 73.0f;
    dy = 0;
    distanceScore = 0;
    totalTime = 0;
    currentSpeed = 1.0f;
    sprHouse.setPosition(100.0f, GROUND_Y - 250.0f);

    for (int i = 0; i < PLATES_AMOUNT; ++i) {
        plates[i].x = (float)WINDOW_WIDTH + 1200.0f + (i * (CACTUS_INTERVAL + 400.0f));
        plates[i].type = rand() % 3;
        SetCactusY(plates[i], tCactus);
    }
}

// Þaidimo mechanikos atnaujinimas
void UpdateGame(Player& player, Plate plates[], float& dy, float& distanceScore, Texture tCactus[], float dt, float totalTime, float& currentSpeed)
{
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
                plates[i].x = max(plates[prevIdx].x + CACTUS_INTERVAL + (rand() % 200), (float)WINDOW_WIDTH + 100);
                plates[i].type = rand() % 3;
                SetCactusY(plates[i], tCactus);
            }
        }
    }
}

int main()
{
    srand((unsigned)time(nullptr));
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "scape stein");
    app.setFramerateLimit(60);

    // Tekstûros
    Texture tBack, tPlayer, tHouse, tHeart, tLogo, tButton, tPlayAgain, tMainMenu, tCactus[3];
    if (!tBack.loadFromFile("resources/background.png") ||
        !tPlayer.loadFromFile("resources/doodle2.png") ||
        !tHouse.loadFromFile("resources/house.png") ||
        !tHeart.loadFromFile("resources/heart.png") ||
        !tLogo.loadFromFile("resources/logo.png") ||
        !tButton.loadFromFile("resources/button.png") ||
        !tPlayAgain.loadFromFile("resources/playagain.png") ||
        !tMainMenu.loadFromFile("resources/mainmenu.png") ||
        !tCactus[0].loadFromFile("resources/cactus.png") ||
        !tCactus[1].loadFromFile("resources/cactus2.png") ||
        !tCactus[2].loadFromFile("resources/cactus3.png"))
    {
        cerr << "Klaida uþkraunant failus!" << endl;
        return -1;
    }

    // Spritai
    Sprite sprBack(tBack), sprPlayer(tPlayer), sprHouse(tHouse), sprHeart(tHeart), sprCactus[PLATES_AMOUNT];
    Sprite sprLogo(tLogo), sprButton(tButton), sprPlayAgain(tPlayAgain), sprMainMenu(tMainMenu);

    // Pozicijø nustatymas (Origin centre)
    sprLogo.setOrigin(75.0f, 75.0f);
    sprLogo.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 60.0f);
    sprButton.setOrigin(75.0f, 20.0f);
    sprButton.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 70.0f);

    sprPlayAgain.setOrigin(75.0f, 20.0f);
    sprPlayAgain.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 30.0f);
    sprMainMenu.setOrigin(75.0f, 20.0f);
    sprMainMenu.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 85.0f);

    sprPlayer.setOrigin(PLAYER_WIDTH / 2.0f, 0);
    sprPlayer.setScale(-1.0f, 1.0f);

    // Ðriftas
    Font font;
    font.loadFromFile("resources/arialbd.ttf");
    Text uiText;
    uiText.setFont(font);
    uiText.setCharacterSize(30);

    // Þaidimo kintamieji
    Player player;
    Plate plates[PLATES_AMOUNT];
    float dy = 0, distanceScore = 0, totalTime = 0, currentSpeed = 1.0f;
    bool isGameStarted = false;

    ResetGame(player, plates, distanceScore, totalTime, currentSpeed, dy, sprHouse, tCactus);

    Clock gameClock;

    while (app.isOpen())
    {
        float dt = gameClock.restart().asSeconds();
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed) app.close();

            // Pelës paspaudimø sekimas
            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));

                if (!isGameStarted) {
                    if (sprButton.getGlobalBounds().contains(mousePos)) isGameStarted = true;
                }
                else if (player.lives <= 0) {
                    if (sprPlayAgain.getGlobalBounds().contains(mousePos)) {
                        ResetGame(player, plates, distanceScore, totalTime, currentSpeed, dy, sprHouse, tCactus);
                    }
                    if (sprMainMenu.getGlobalBounds().contains(mousePos)) {
                        isGameStarted = false;
                        ResetGame(player, plates, distanceScore, totalTime, currentSpeed, dy, sprHouse, tCactus);
                    }
                }
            }

            // Ðuolis
            if (isGameStarted && player.lives > 0 && e.type == Event::KeyPressed) {
                if ((e.key.code == Keyboard::Space || e.key.code == Keyboard::Up) && dy == 0)
                    dy = PLAYER_JUMP_V;
            }
        }

        // Logikos atnaujinimas
        if (isGameStarted && player.lives > 0) {
            totalTime += dt;
            UpdateGame(player, plates, dy, distanceScore, tCactus, dt, totalTime, currentSpeed);
            sprHouse.move(-currentSpeed * 1.1f, 0);
        }

        app.clear();

        if (!isGameStarted) {
            // --- PAGRINDINIS MENIU ---
            app.draw(sprBack);
            app.draw(sprLogo);
            app.draw(sprButton);
        }
        else if (player.lives > 0) {
            // --- ÞAIDIMO EIGA ---
            app.draw(sprBack);

            if (sprHouse.getPosition().x + 300 > 0) app.draw(sprHouse);

            for (int i = 0; i < PLATES_AMOUNT; ++i) {
                sprCactus[i].setTexture(tCactus[plates[i].type], true);
                sprCactus[i].setPosition(plates[i].x, plates[i].y);
                app.draw(sprCactus[i]);
            }

            sprPlayer.setPosition(player.x, player.y);
            app.draw(sprPlayer);

            // UI Tekstas
            uiText.setFillColor(Color::Black);
            uiText.setString("Score: " + to_string((int)distanceScore));
            uiText.setOrigin(0, 0); // Atstatome origin tekstui
            uiText.setPosition(20, 10);
            app.draw(uiText);

            // Ðirdelës
            for (int i = 0; i < player.lives; i++) {
                sprHeart.setPosition(WINDOW_WIDTH - 150.0f + (i * 45.0f), 15.0f);
                app.draw(sprHeart);
            }
        }
        else {
            // --- MIRUS (GAME OVER) ---
            app.clear(Color(0, 192, 255)); // Tavo norima spalva #00c0ff

            uiText.setFillColor(Color::White);
            uiText.setString("SCORE: " + to_string((int)distanceScore));

            // Centruojame rezultatà
            FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            uiText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 50.0f);

            app.draw(uiText);
            app.draw(sprPlayAgain);
            app.draw(sprMainMenu);
        }

        app.display();
    }

    return 0;
}