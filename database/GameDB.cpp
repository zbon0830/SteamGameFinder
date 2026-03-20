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
    Game game;

    game.app_id = safeText(sqlite3_column_text(stmt, 0));
    game.name = safeText(sqlite3_column_text(stmt, 1));
    game.release_date = safeText(sqlite3_column_text(stmt, 2));
    game.estimated_owners = safeText(sqlite3_column_text(stmt, 3));
    game.peak_ccu = sqlite3_column_int(stmt, 4);
    game.required_age = sqlite3_column_int(stmt, 5);
    game.price = sqlite3_column_double(stmt, 6);
    game.discount_dlc_count = sqlite3_column_int(stmt, 7);
    game.about_the_game = safeText(sqlite3_column_text(stmt, 8));
    game.supported_languages = safeText(sqlite3_column_text(stmt, 9));
    game.full_audio_languages = safeText(sqlite3_column_text(stmt, 10));
    game.reviews = safeText(sqlite3_column_text(stmt, 11));
    game.header_image = safeText(sqlite3_column_text(stmt, 12));
    game.website = safeText(sqlite3_column_text(stmt, 13));
    game.support_url = safeText(sqlite3_column_text(stmt, 14));
    game.support_email = safeText(sqlite3_column_text(stmt, 15));
    game.windows_support = sqlite3_column_int(stmt, 16);
    game.mac_support = sqlite3_column_int(stmt, 17);
    game.linux_support = sqlite3_column_int(stmt, 18);
    game.metacritic_score = sqlite3_column_int(stmt, 19);
    game.metacritic_url = safeText(sqlite3_column_text(stmt, 20));
    game.user_score = sqlite3_column_int(stmt, 21);
    game.positive = sqlite3_column_int(stmt, 22);
    game.negative = sqlite3_column_int(stmt, 23);
    game.score_rank = safeText(sqlite3_column_text(stmt, 24));
    game.achievements = sqlite3_column_int(stmt, 25);
    game.recommendations = sqlite3_column_int(stmt, 26);
    game.notes = safeText(sqlite3_column_text(stmt, 27));
    game.average_playtime_forever = sqlite3_column_int(stmt, 28);
    game.average_playtime_two_weeks = sqlite3_column_int(stmt, 29);
    game.median_playtime_forever = sqlite3_column_int(stmt, 30);
    game.median_playtime_two_weeks = sqlite3_column_int(stmt, 31);
    game.developers = safeText(sqlite3_column_text(stmt, 32));
    game.publishers = safeText(sqlite3_column_text(stmt, 33));
    game.categories = safeText(sqlite3_column_text(stmt, 34));
    game.genres = safeText(sqlite3_column_text(stmt, 35));
    game.tags = safeText(sqlite3_column_text(stmt, 36));
    game.screenshots = safeText(sqlite3_column_text(stmt, 37));
    game.movies = safeText(sqlite3_column_text(stmt, 38));

    return game;
}

std::vector<Game> GameDB::getAllGames() {
    std::vector<Game> games;
    const char* sql = "SELECT * FROM games;";
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
    const char* sql = "SELECT * FROM games WHERE name LIKE ?;";
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
    const char* sql = "SELECT * FROM games WHERE price <= ?;";
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
    const char* sql = "SELECT * FROM games WHERE price BETWEEN ? AND ?;";
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
    const char* sql = "SELECT * FROM games WHERE genres LIKE ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }

    std::string pattern = "%" + genre + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        games.push_back(rowToGame(stmt));
    }

    sqlite3_finalize(stmt);
    return games;
}