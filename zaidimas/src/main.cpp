#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "defines.h"
#include "objects.h"
#include "utils.h"

using namespace sf;
using namespace std;

// Funkcija kaktuso aukðèiui nustatyti pagal tekstûrà
void SetCactusY(Plate& cactus, Texture tCactus[]) {
    Vector2u size = tCactus[cactus.type].getSize();
    cactus.y = GROUND_Y - (float)size.y;
}

// Pagrindinë þaidimo logikos atnaujinimo funkcija
void UpdateGame(Player& player, Plate plates[], float& dy, float& score, Texture tCactus[])
{
    // Gravitacija ir ðuolis
    dy += 0.6f;
    player.y += dy;

    // Þemës riba
    if (player.y > GROUND_Y - 73.0f) {
        player.y = GROUND_Y - 73.0f;
        dy = 0;
    }

    // Greitis didëja kartu su taðkais
    float speed = 6.0f + (score / 20.0f);

    for (int i = 0; i < PLATES_AMOUNT; ++i)
    {
        plates[i].x -= speed;

        // --- SUSIDÛRIMO TIKRINIMAS (Tavo originalus metodas) ---
        if (utils::CheckCollision(player, plates[i])) {
            player.lives--;
            // Perkeliam kaktusà uþ ekrano ribø, kad jis „nesuvalgytø“ visø gyvybiø iðkart
            plates[i].x = -200.0f;
        }

        // Jei kaktusas iðvaþiavo pro kairæ pusæ
        if (plates[i].x < -150.0f) {
            // Surandame paskutiná kaktusà, kad iðlaikytume intervalà
            int prevIdx = (i == 0) ? PLATES_AMOUNT - 1 : i - 1;
            plates[i].x = plates[prevIdx].x + CACTUS_INTERVAL;

            // Sugeneruojame naujà kaktuso tipà
            plates[i].type = rand() % 3;
            SetCactusY(plates[i], tCactus);

            score += 1.0f;
        }
    }
}

int main()
{
    srand((unsigned)time(nullptr));
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dino Doodle Run");
    app.setFramerateLimit(60);

    // Tekstûrø krovimas
    Texture tBack, tPlayer, tCactus[3];
    if (!tBack.loadFromFile("resources/background.png") ||
        !tPlayer.loadFromFile("resources/doodle2.png") ||
        !tCactus[0].loadFromFile("resources/cactus.png") ||
        !tCactus[1].loadFromFile("resources/cactus2.png") ||
        !tCactus[2].loadFromFile("resources/cactus3.png"))
    {
        cout << "Klaida: Nepavyko uþkrauti paveikslëliø!" << endl;
        return -1;
    }

    // Sprite nustatymai
    Sprite sprBack(tBack), sprPlayer(tPlayer), sprCactus[PLATES_AMOUNT];
    sprPlayer.setOrigin(PLAYER_WIDTH / 2.0f, 0);
    sprPlayer.setScale(-1.0f, 1.0f); // Dinozauras þiûri á deðinæ

    // Ðriftas ir tekstas (UI)
    Font font;
    if (!font.loadFromFile("resources/arialbd.ttf")) return -1;
    Text uiText;
    uiText.setFont(font);
    uiText.setCharacterSize(25);
    uiText.setFillColor(Color::Black);

    // Þaidëjo pradþia
    Player player;
    player.x = 150.0f;
    player.y = GROUND_Y - 73.0f;
    player.lives = INITIAL_LIVES;

    // Kaktusø pradþia
    Plate plates[PLATES_AMOUNT];
    for (int i = 0; i < PLATES_AMOUNT; ++i) {
        plates[i].x = (float)WINDOW_WIDTH + (i * CACTUS_INTERVAL);
        plates[i].type = rand() % 3;
        SetCactusY(plates[i], tCactus);
    }

    float dy = 0, score = 0;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed) app.close();

            // Ðuolis
            if (e.type == Event::KeyPressed) {
                if ((e.key.code == Keyboard::Space || e.key.code == Keyboard::Up) && dy == 0)
                    dy = PLAYER_JUMP_V;
            }
        }

        // Logikos atnaujinimas
        if (player.lives > 0) {
            UpdateGame(player, plates, dy, score, tCactus);
        }

        // --- PIEÐIMAS ---
        app.clear();
        app.draw(sprBack);

        // Kaktusø pieðimas
        for (int i = 0; i < PLATES_AMOUNT; ++i) {
            sprCactus[i].setTexture(tCactus[plates[i].type]);
            sprCactus[i].setPosition(plates[i].x, plates[i].y);
            app.draw(sprCactus[i]);
        }

        // Þaidëjo pieðimas
        sprPlayer.setPosition(player.x, player.y);
        app.draw(sprPlayer);

        // UI pieðimas
        uiText.setString("Gyvybes: " + to_string(player.lives) + "  Taskai: " + to_string((int)score));
        uiText.setPosition(20, 10);
        app.draw(uiText);

        // Game Over ekranas
        if (player.lives <= 0) {
            uiText.setString("GAME OVER! Spausk R kad pradetum is naujo");
            uiText.setPosition(WINDOW_WIDTH / 6.0f, WINDOW_HEIGHT / 2.0f);
            app.draw(uiText);

            if (Keyboard::isKeyPressed(Keyboard::R)) {
                // Restartas
                player.lives = INITIAL_LIVES;
                score = 0;
                dy = 0;
                for (int i = 0; i < PLATES_AMOUNT; ++i) {
                    plates[i].x = (float)WINDOW_WIDTH + (i * CACTUS_INTERVAL);
                    plates[i].type = rand() % 3;
                    SetCactusY(plates[i], tCactus);
                }
            }
        }

        app.display();
    }

    return 0;
}