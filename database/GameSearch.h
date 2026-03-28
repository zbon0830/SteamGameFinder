#ifndef GAMESEARCH_H
#define GAMESEARCH_H

#include "GameDB.h"

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class SearchMode {
    Ordered,
    Unordered
};

struct SearchResults {
    std::vector<Game> games;
    long long elapsedMicroseconds = 0;
};

class GameSearch {
public:
    void load(const std::vector<Game>& games);
    SearchResults search(const std::string& genre, int minPriceCents, int maxPriceCents, SearchMode mode) const;
    bool empty() const;

private:
    struct IndexedGame {
        Game game;
        int priceCents = 0;
    };

    struct OrderedStore {
        std::map<std::string, IndexedGame> gamesById;
        std::map<int, std::vector<std::string>> priceIndex;
        std::map<std::string, std::vector<std::string>> genreIndex;
    };

    struct UnorderedStore {
        std::unordered_map<std::string, IndexedGame> gamesById;
        std::unordered_map<int, std::vector<std::string>> priceIndex;
        std::unordered_map<std::string, std::vector<std::string>> genreIndex;
    };

    OrderedStore orderedStore;
    UnorderedStore unorderedStore;

    static std::vector<std::string> splitGenres(const std::string& genres);
    static void insertGame(OrderedStore& store, const Game& game);
    static void insertGame(UnorderedStore& store, const Game& game);
    static std::unordered_set<std::string> searchPrice(const OrderedStore& store, int minPriceCents, int maxPriceCents);
    static std::unordered_set<std::string> searchPrice(const UnorderedStore& store, int minPriceCents, int maxPriceCents);
    static std::unordered_set<std::string> searchGenre(const OrderedStore& store, const std::string& genre);
    static std::unordered_set<std::string> searchGenre(const UnorderedStore& store, const std::string& genre);
    static std::vector<std::string> intersect(const std::unordered_set<std::string>& a, const std::unordered_set<std::string>& b);

    template <typename Store>
    static SearchResults buildResults(const Store& store, const std::vector<std::string>& ids, long long elapsedMicroseconds) {
        SearchResults results;
        results.elapsedMicroseconds = elapsedMicroseconds;
        results.games.reserve(ids.size());

        for (const std::string& id : ids) {
            results.games.push_back(store.gamesById.at(id).game);
        }

        return results;
    }
};

#endif
