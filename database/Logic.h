#ifndef SEARCHLOGIC_H
#define SEARCHLOGIC_H
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <iostream>
#include "GameDB.h"

struct OrderedStore {
    std::map<std::string, Game> gamesById;
    std::map<int, std::vector<std::string>> priceIndex;
    std::map<std::string, std::vector<std::string>> genreIndex;
};
struct UnorderedStore {
    std::unordered_map<std::string, Game> gamesById;
    std::unordered_map<int, std::vector<std::string>> priceIndex;
    std::unordered_map<std::string, std::vector<std::string>> genreIndex;
};
std::vector<std::string> splitGenres(const std::string& genres);
void insertGame(OrderedStore& store, const Game& game);
void insertGame(UnorderedStore& store, const Game& game);
std::unordered_set<std::string> searchPriceOrdered(const OrderedStore& store, int minP, int maxP);
std::unordered_set<std::string> searchGenreOrdered(const OrderedStore& store, const std::string& genre);
std::unordered_set<std::string> searchPriceUnordered(const UnorderedStore& store, int minP, int maxP);
std::unordered_set<std::string> searchGenreUnordered(const UnorderedStore& store, const std::string& genre);
std::vector<std::string> intersect(const std::unordered_set<std::string>& a, const std::unordered_set<std::string>& b);
template <typename Store>
void printResults(const Store& store, const std::vector<std::string>& ids) {
    for (const std::string& id : ids) {
        const Game& g = store.gamesById.at(id);
        std::cout << g.name
                  << " | $" << g.price
                  << " | " << g.genres << std::endl;
    }
}

#endif