#include "GameDB.h"
#include <iostream>

void printGame(const Game& game) {
    std::cout << "Name: " << game.name << '\n';
    std::cout << "Price: $" << game.price << '\n';
    std::cout << "Genres: " << game.genres << '\n';
    std::cout << "--------------------------\n";
}

int main() {
    GameDB db;

    if (!db.open("games.db")) {
        return 1;
    }

    std::cout << "Games with name containing 'Portal':\n";
    std::vector<Game> byName = db.getGamesByName("Portal");
    for (const auto& game : byName) {
        printGame(game);
    }

    std::cout << "\nGames under $10:\n";
    std::vector<Game> cheapGames = db.getGamesByPriceLessThanOrEqual(10.0);
    for (const auto& game : cheapGames) {
        printGame(game);
    }

    db.close();
    return 0;
}