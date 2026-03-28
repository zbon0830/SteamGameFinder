#include "GameSearch.h"

#include <algorithm>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

std::vector<std::string> GameSearch::splitGenres(const std::string& genres) {
    std::vector<std::string> result;
    std::string current;

    for (char c : genres) {
        if (c == ',') {
            if (!current.empty()) {
                while (!current.empty() && current.front() == ' ') {
                    current.erase(current.begin());
                }
                while (!current.empty() && current.back() == ' ') {
                    current.pop_back();
                }
                if (!current.empty()) {
                    result.push_back(current);
                }
                current.clear();
            }
            continue;
        }

        current += c;
    }

    if (!current.empty()) {
        while (!current.empty() && current.front() == ' ') {
            current.erase(current.begin());
        }
        while (!current.empty() && current.back() == ' ') {
            current.pop_back();
        }
        if (!current.empty()) {
            result.push_back(current);
        }
    }

    return result;
}

void GameSearch::insertGame(OrderedStore& store, const Game& game) {
    const int priceCents = static_cast<int>(game.price * 100);
    store.gamesById[game.app_id] = {game, priceCents};
    store.priceIndex[priceCents].push_back(game.app_id);

    for (const std::string& genre : splitGenres(game.genres)) {
        store.genreIndex[genre].push_back(game.app_id);
    }
}

void GameSearch::insertGame(UnorderedStore& store, const Game& game) {
    const int priceCents = static_cast<int>(game.price * 100);
    store.gamesById[game.app_id] = {game, priceCents};
    store.priceIndex[priceCents].push_back(game.app_id);

    for (const std::string& genre : splitGenres(game.genres)) {
        store.genreIndex[genre].push_back(game.app_id);
    }
}

std::unordered_set<std::string> GameSearch::searchPrice(const OrderedStore& store, int minPriceCents, int maxPriceCents) {
    std::unordered_set<std::string> result;

    const auto start = store.priceIndex.lower_bound(minPriceCents);
    const auto end = store.priceIndex.upper_bound(maxPriceCents);

    for (auto it = start; it != end; ++it) {
        for (const std::string& id : it->second) {
            result.insert(id);
        }
    }

    return result;
}

std::unordered_set<std::string> GameSearch::searchPrice(const UnorderedStore& store, int minPriceCents, int maxPriceCents) {
    std::unordered_set<std::string> result;

    for (const auto& [price, ids] : store.priceIndex) {
        if (price < minPriceCents || price > maxPriceCents) {
            continue;
        }

        for (const std::string& id : ids) {
            result.insert(id);
        }
    }

    return result;
}

std::unordered_set<std::string> GameSearch::searchGenre(const OrderedStore& store, const std::string& genre) {
    std::unordered_set<std::string> result;

    if (genre.empty()) {
        for (const auto& [id, indexedGame] : store.gamesById) {
            result.insert(id);
        }
        return result;
    }

    const auto it = store.genreIndex.find(genre);
    if (it == store.genreIndex.end()) {
        return result;
    }

    for (const std::string& id : it->second) {
        result.insert(id);
    }

    return result;
}

std::unordered_set<std::string> GameSearch::searchGenre(const UnorderedStore& store, const std::string& genre) {
    std::unordered_set<std::string> result;

    if (genre.empty()) {
        for (const auto& [id, indexedGame] : store.gamesById) {
            result.insert(id);
        }
        return result;
    }

    const auto it = store.genreIndex.find(genre);
    if (it == store.genreIndex.end()) {
        return result;
    }

    for (const std::string& id : it->second) {
        result.insert(id);
    }

    return result;
}

std::vector<std::string> GameSearch::intersect(const std::unordered_set<std::string>& a, const std::unordered_set<std::string>& b) {
    std::vector<std::string> result;
    result.reserve(std::min(a.size(), b.size()));

    for (const std::string& id : a) {
        if (b.count(id) != 0U) {
            result.push_back(id);
        }
    }

    return result;
}

void GameSearch::load(const std::vector<Game>& games) {
    orderedStore = OrderedStore{};
    unorderedStore = UnorderedStore{};

    for (const Game& game : games) {
        insertGame(orderedStore, game);
        insertGame(unorderedStore, game);
    }
}

SearchResults GameSearch::search(const std::string& genre, int minPriceCents, int maxPriceCents, SearchMode mode) const {
    const auto startedAt = Clock::now();

    if (mode == SearchMode::Ordered) {
        const auto priceMatches = searchPrice(orderedStore, minPriceCents, maxPriceCents);
        const auto genreMatches = searchGenre(orderedStore, genre);
        auto ids = intersect(priceMatches, genreMatches);

        std::sort(ids.begin(), ids.end(), [this](const std::string& left, const std::string& right) {
            return orderedStore.gamesById.at(left).game.name < orderedStore.gamesById.at(right).game.name;
        });

        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - startedAt).count();
        return buildResults(orderedStore, ids, elapsed);
    }

    const auto priceMatches = searchPrice(unorderedStore, minPriceCents, maxPriceCents);
    const auto genreMatches = searchGenre(unorderedStore, genre);
    auto ids = intersect(priceMatches, genreMatches);

    std::sort(ids.begin(), ids.end(), [this](const std::string& left, const std::string& right) {
        return unorderedStore.gamesById.at(left).game.name < unorderedStore.gamesById.at(right).game.name;
    });

    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - startedAt).count();
    return buildResults(unorderedStore, ids, elapsed);
}

bool GameSearch::empty() const {
    return orderedStore.gamesById.empty();
}
