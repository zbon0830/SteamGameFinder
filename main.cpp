#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <QApplication>
#include "UI/Window.h"
using namespace std;

struct Game {
    string app_id;
    string name;
    double price;
    int priceCents; // convert 19.99 to 1999 for easier indexing
    string genres;
};

struct OrderedStore {
    map<string, Game> gamesById;
    map<int, vector<string>> priceIndex;
    map<string, vector<string>> genreIndex;
};

struct UnorderedStore {
    unordered_map<string, Game> gamesById;
    unordered_map<int, vector<string>> priceIndex;
    unordered_map<string, vector<string>> genreIndex;
};

vector<string> splitGenres(const string& genres) {
    vector<string> result;
    string current;

    for (char c : genres) {
        if (c == ',') {
            if (!current.empty()) {
                while (!current.empty() && current.front() == ' ')
                    current.erase(current.begin());
                while (!current.empty() && current.back() == ' ')
                    current.pop_back();
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        while (!current.empty() && current.front() == ' ')
            current.erase(current.begin());
        while (!current.empty() && current.back() == ' ')
            current.pop_back();
        result.push_back(current);
    }

    return result;
}

// INSERTION FUNCTIONS
void insertGame(OrderedStore& store, const Game& game) {
    store.gamesById[game.app_id] = game;
    store.priceIndex[game.priceCents].push_back(game.app_id);

    for (const string& g : splitGenres(game.genres)) {
        store.genreIndex[g].push_back(game.app_id);
    }
}

void insertGame(UnorderedStore& store, const Game& game) {
    store.gamesById[game.app_id] = game;
    store.priceIndex[game.priceCents].push_back(game.app_id);

    for (const string& g : splitGenres(game.genres)) {
        store.genreIndex[g].push_back(game.app_id);
    }
}


int main(int argc, char *argv[]) {
    OrderedStore ordered;
    UnorderedStore unordered;
    sqlite3* db;

    if (sqlite3_open("../database/games.db", &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return 1;
    }

    string sql = "SELECT app_id, name, price, genres FROM games;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement\n";
        return 1;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Game g;

        const unsigned char* appIdText = sqlite3_column_text(stmt, 0);
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        const unsigned char* genresText = sqlite3_column_text(stmt, 3);

        g.app_id = appIdText ? reinterpret_cast<const char*>(appIdText) : "";
        g.name = nameText ? reinterpret_cast<const char*>(nameText) : "";
        g.price = sqlite3_column_double(stmt, 2);
        g.priceCents = static_cast<int>(g.price * 100);
        g.genres = genresText ? reinterpret_cast<const char*>(genresText) : "";

        ordered.gamesById[g.app_id] = g;
        ordered.priceIndex[g.priceCents].push_back(g.app_id);

        vector<string> genreList = splitGenres(g.genres);
        for (const string& genre : genreList) {
            ordered.genreIndex[genre].push_back(g.app_id);
        }

        unordered.gamesById[g.app_id] = g;
        unordered.priceIndex[g.priceCents].push_back(g.app_id);

        for (const string& genre : genreList) {
            unordered.genreIndex[genre].push_back(g.app_id);
        }

    }

    cout << "Loaded data successfully!\n";

    QApplication app(argc, argv);
    Window window;
    window.show();
    return app.exec();

    return 0;
}