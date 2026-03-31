#pragma once
#include "objects.h"

namespace utils {
    // Patikriname, ar skaièius yra tarp dviejø ribø
    bool IsBetween(float value, float min, float max);

    // Pagrindinë susidûrimo logika, kurià naudojai anksèiau
    bool CheckCollision(Player& player, Plate& plate);
}