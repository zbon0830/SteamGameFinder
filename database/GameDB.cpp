#include "GameDB.h"
#include <iostream>

GameDB::GameDB() : db(nullptr) {}

GameDB::~GameDB() {
    close();
}

bool GameDB::open(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

void GameDB::close() {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

std::string GameDB::safeText(const unsigned char* text) {
    return text ? reinterpret_cast<const char*>(text) : "";
}

Game GameDB::rowToGame(sqlite3_stmt* stmt) {
    Game game{};
    std::string featureText = safeText(sqlite3_column_text(stmt, 3));
    std::string tagText = safeText(sqlite3_column_text(stmt, 4));

    game.app_id = safeText(sqlite3_column_text(stmt, 0));
    game.name = safeText(sqlite3_column_text(stmt, 1));
    game.price = sqlite3_column_double(stmt, 2);

    game.tags = tagText;
    game.categories = safeText(sqlite3_column_text(stmt, 5));

    if (!tagText.empty() && !featureText.empty()) {
        game.genres = tagText + "," + featureText;
    } else if (!tagText.empty()) {
        game.genres = tagText;
    } else {
        game.genres = featureText;
    }

    game.recommendations = sqlite3_column_int(stmt, 6);

    return game;
}

std::vector<Game> GameDB::getAllGames() {
    std::vector<Game> games;
    const char* sql =
        "SELECT app_id, name, price, genres, tags, categories, recommendations "
        "FROM games;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        games.push_back(rowToGame(stmt));
    }

    sqlite3_finalize(stmt);
    return games;
}

std::vector<Game> GameDB::getGamesByName(const std::string& name) {
    std::vector<Game> games;
    const char* sql =
        "SELECT app_id, name, price, genres, tags, categories, recommendations "
        "FROM games WHERE name LIKE ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }

    std::string pattern = "%" + name + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        games.push_back(rowToGame(stmt));
    }

    sqlite3_finalize(stmt);
    return games;
}

std::vector<Game> GameDB::getGamesByPriceLessThanOrEqual(double maxPrice) {
    std::vector<Game> games;
    const char* sql =
        "SELECT app_id, name, price, genres, tags, categories, recommendations "
        "FROM games WHERE price <= ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }

    sqlite3_bind_double(stmt, 1, maxPrice);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        games.push_back(rowToGame(stmt));
    }

    sqlite3_finalize(stmt);
    return games;
}

std::vector<Game> GameDB::getGamesByPriceRange(double minPrice, double maxPrice) {
    std::vector<Game> games;
    const char* sql =
        "SELECT app_id, name, price, genres, tags, categories, recommendations "
        "FROM games WHERE price BETWEEN ? AND ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }

    sqlite3_bind_double(stmt, 1, minPrice);
    sqlite3_bind_double(stmt, 2, maxPrice);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        games.push_back(rowToGame(stmt));
    }

    sqlite3_finalize(stmt);
    return games;
}

std::vector<Game> GameDB::getGamesByGenre(const std::string& genre) {
    std::vector<Game> games;
    const char* sql =
        "SELECT app_id, name, price, genres, tags, categories, recommendations "
        "FROM games WHERE tags LIKE ? OR genres LIKE ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }

    std::string pattern = "%" + genre + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        games.push_back(rowToGame(stmt));
    }

    sqlite3_finalize(stmt);
    return games;
}
