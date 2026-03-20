#ifndef GAMEDB_H
#define GAMEDB_H

#include <string>
#include <vector>
#include <sqlite3.h>

struct Game {
    std::string app_id;
    std::string name;
    std::string release_date;
    std::string estimated_owners;
    int peak_ccu;
    int required_age;
    double price;
    int discount_dlc_count;
    std::string about_the_game;
    std::string supported_languages;
    std::string full_audio_languages;
    std::string reviews;
    std::string header_image;
    std::string website;
    std::string support_url;
    std::string support_email;
    int windows_support;
    int mac_support;
    int linux_support;
    int metacritic_score;
    std::string metacritic_url;
    int user_score;
    int positive;
    int negative;
    std::string score_rank;
    int achievements;
    int recommendations;
    std::string notes;
    int average_playtime_forever;
    int average_playtime_two_weeks;
    int median_playtime_forever;
    int median_playtime_two_weeks;
    std::string developers;
    std::string publishers;
    std::string categories;
    std::string genres;
    std::string tags;
    std::string screenshots;
    std::string movies;
};

class GameDB {
private:
    sqlite3* db;

    static std::string safeText(const unsigned char* text);
    static Game rowToGame(sqlite3_stmt* stmt);

public:
    GameDB();
    ~GameDB();

    bool open(const std::string& dbPath);
    void close();

    std::vector<Game> getAllGames();
    std::vector<Game> getGamesByName(const std::string& name);
    std::vector<Game> getGamesByPriceLessThanOrEqual(double maxPrice);
    std::vector<Game> getGamesByPriceRange(double minPrice, double maxPrice);
    std::vector<Game> getGamesByGenre(const std::string& genre);
};

#endif