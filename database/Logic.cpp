#include "Logic.h"
#include <cctype>

using std::string;
using std::vector;
using std::unordered_set;

static string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {start++;}
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {end--;}
    return s.substr(start, end - start);
}

static string toLowerCase(string s) {
    for (char& c : s) {c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));}
    return s;
}

static string cleanGenreToken(string token) {
    token = trim(token);
    while (!token.empty() &&
           (token.front() == '[' || token.front() == ']' ||
            token.front() == '"' || token.front() == '\'')) {token.erase(token.begin());}
    while (!token.empty() &&
           (token.back() == '[' || token.back() == ']' ||
            token.back() == '"' || token.back() == '\'')) {token.pop_back();}
    token = trim(token);
    token = toLowerCase(token);
    return token;
}

vector<string> splitGenres(const string& genres) {
    vector<string> result;
    string current;
    for (char c : genres) {
        if (c == ',' || c == ';' || c == '|') {
            string cleaned = cleanGenreToken(current);
            if (!cleaned.empty()) {
                result.push_back(cleaned);
            }
            current.clear();
        } else {
            current += c;
        }
    }
    string cleaned = cleanGenreToken(current);
    if (!cleaned.empty()) {
        result.push_back(cleaned);
    }
    return result;
}

void insertGame(OrderedStore& store, const Game& game) {
    store.gamesById[game.app_id] = game;
    int priceCents = static_cast<int>(game.price * 100);
    store.priceIndex[priceCents].push_back(game.app_id);
    vector<string> genreList = splitGenres(game.genres);
    for (const string& g : genreList) {
        store.genreIndex[g].push_back(game.app_id);
    }
}

void insertGame(UnorderedStore& store, const Game& game) {
    store.gamesById[game.app_id] = game;
    int priceCents = static_cast<int>(game.price * 100);
    store.priceIndex[priceCents].push_back(game.app_id);
    vector<string> genreList = splitGenres(game.genres);
    for (const string& g : genreList) {
        store.genreIndex[g].push_back(game.app_id);
    }
}

unordered_set<string> searchPriceOrdered(const OrderedStore& store, int minP, int maxP) {
    unordered_set<string> result;
    auto start = store.priceIndex.lower_bound(minP);
    auto end = store.priceIndex.upper_bound(maxP);
    for (auto it = start; it != end; ++it) {
        for (const string& id : it->second) {
            result.insert(id);
        }
    }
    return result;
}

unordered_set<string> searchGenreOrdered(const OrderedStore& store, const string& genre) {
    unordered_set<string> result;
    string normalizedGenre = cleanGenreToken(genre);
    if (normalizedGenre == "any") {
        for (const auto& pair : store.gamesById) {
            result.insert(pair.first);
        }
        return result;
    }
    if (store.genreIndex.count(normalizedGenre)) {
        for (const string& id : store.genreIndex.at(normalizedGenre)) {
            result.insert(id);
        }
    } else {
        for (const auto& pair : store.genreIndex) {
            if (pair.first.find(normalizedGenre) != string::npos ||
                normalizedGenre.find(pair.first) != string::npos) {
                for (const string& id : pair.second) {
                    result.insert(id);
                }
            }
        }
    }
    return result;
}

unordered_set<string> searchPriceUnordered(const UnorderedStore& store, int minP, int maxP) {
    unordered_set<string> result;
    for (const auto& p : store.priceIndex) {
        if (p.first >= minP && p.first <= maxP) {
            for (const string& id : p.second) {
                result.insert(id);
            }
        }
    }
    return result;
}

unordered_set<string> searchGenreUnordered(const UnorderedStore& store, const string& genre) {
    unordered_set<string> result;
    string normalizedGenre = cleanGenreToken(genre);
    if (normalizedGenre == "any") {
        for (const auto& pair : store.gamesById) {
            result.insert(pair.first);
        }
        return result;
    }
    if (store.genreIndex.count(normalizedGenre)) {
        for (const string& id : store.genreIndex.at(normalizedGenre)) {
            result.insert(id);
        }
    } else {
        for (const auto& pair : store.genreIndex) {
            if (pair.first.find(normalizedGenre) != string::npos ||
                normalizedGenre.find(pair.first) != string::npos) {
                for (const string& id : pair.second) {
                    result.insert(id);
                }
            }
        }
    }
    return result;
}

vector<string> intersect(const unordered_set<string>& a, const unordered_set<string>& b) {
    vector<string> result;
    for (const string& id : a) {
        if (b.count(id)) {
            result.push_back(id);
        }
    }
    return result;
}