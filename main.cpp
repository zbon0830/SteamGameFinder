#include <iostream>
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
    map<string, Game> gamesbyID;
    map<int, vector<string>> priceIndex;
    map<string, vector<string>> genreIndex;
};

struct UnorderedStore {
    unordered_map<string, Game> gamesById;
    unordered_map<int, vector<string>> priceIndex;
    unordered_map<string, vector<string>> genreIndex;
};


//main to create the window pop up and commented out game querry main
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Window window;
    window.show();
    return app.exec();
}