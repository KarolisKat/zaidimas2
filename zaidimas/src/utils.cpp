#include "utils.h"

namespace utils {
    // Patikriname, ar skaièius telpa á rëmus
    bool IsBetween(float value, float min, float max) {
        return (value >= min && value <= max);
    }

    bool CheckCollision(Player& player, Plate& plate) {
        // Tikriname X aðá (ar kojos lieèia kaktusà horizontaliai)
        bool xOverlap = IsBetween(player.LegsStartX(), plate.StartX(), plate.EndX()) ||
            IsBetween(player.LegsEndX(), plate.StartX(), plate.EndX());

        // Tikriname Y aðá (ar kojos yra kaktuso aukðtyje)
        bool yOverlap = IsBetween(player.LegsY(), plate.TopY(), plate.BottomY());

        return xOverlap && yOverlap;
    }
}