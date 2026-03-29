#include "Window.h"
#include "ui_Window.h"
#include <QStringList>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTableWidgetItem>
#include <QStatusBar>
#include <chrono>
#include <algorithm>
#include <cctype>

namespace {
    std::string trimString(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {start++;}

        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {end--;}

        return s.substr(start, end - start);
    }

    std::string NormalizeLabel(const std::string& s) {
        std::string cleaned = trimString(s);

        for (char& c : cleaned) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        return cleaned;
    }

    bool labelExistsInStore(const OrderedStore& store, const std::string& label) {
        std::string normalized = NormalizeLabel(label);

        if (store.genreIndex.count(normalized)) {
            return true;
        }

        for (const auto& pair : store.genreIndex) {
            if (pair.first.find(normalized) != std::string::npos ||
                normalized.find(pair.first) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    QStringList buildDropdownOptions(const OrderedStore& store) {
        std::vector<QString> preferredOptions = {
            "Action",
            "Adventure",
            "Anime",
            "Casual",
            "Co-op",
            "Early Access",
            "Fantasy",
            "Fighting",
            "Horror",
            "Indie",
            "Multiplayer",
            "Mystery",
            "Open World",
            "Platformer",
            "Puzzle",
            "Racing",
            "RPG",
            "Sandbox",
            "Sci-Fi",
            "Shooter",
            "Simulation",
            "Sports",
            "Story",
            "Strategy",
            "Survival"
        };

        QStringList result;
        result << "Any";

        for (const QString& option : preferredOptions) {
            if (labelExistsInStore(store, option.toStdString())) {
                result << option;
            }
        }

        return result;
    }

    int recommendationScore(const Game& game) {
        return game.recommendations;
    }

    template <typename Store>
    void sortResults(std::vector<std::string>& results,
                     const Store& store,
                     const QString& sortOption) {
        if (sortOption == "Name (A-Z)") {
            std::sort(results.begin(), results.end(),
                [&](const std::string& a, const std::string& b) {
                    return store.gamesById.at(a).name < store.gamesById.at(b).name;
                });
        }
        else if (sortOption == "Price (Low-High)") {
            std::sort(results.begin(), results.end(),
                [&](const std::string& a, const std::string& b) {
                    return store.gamesById.at(a).price < store.gamesById.at(b).price;
                });
        }
        else if (sortOption == "Price (High-Low)") {
            std::sort(results.begin(), results.end(),
                [&](const std::string& a, const std::string& b) {
                    return store.gamesById.at(a).price > store.gamesById.at(b).price;
                });
        }
        else if (sortOption == "Recommendations (High-Low)") {
            std::sort(results.begin(), results.end(),
                [&](const std::string& a, const std::string& b) {
                    return recommendationScore(store.gamesById.at(a)) > recommendationScore(store.gamesById.at(b));
                });
        }
    }
}

Window::Window(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Window) {
    ui->setupUi(this);

    setWindowTitle("Steam Game Finder");
    resize(1000, 700);

    ui->minPriceSpinBox->setPrefix("$");
    ui->maxPriceSpinBox->setPrefix("$");
    ui->minPriceSpinBox->setMaximum(9999.0);
    ui->maxPriceSpinBox->setMaximum(9999.0);
    ui->minPriceSpinBox->setValue(0.0);
    ui->maxPriceSpinBox->setValue(60.0);
    ui->minPriceSpinBox->setDecimals(2);
    ui->maxPriceSpinBox->setDecimals(2);

    ui->sortComboBox->clear();
    ui->sortComboBox->addItems({
        "Default",
        "Name (A-Z)",
        "Price (Low-High)",
        "Price (High-Low)",
        "Recommendations (High-Low)"
    });

    ui->resultsTableWidget->setColumnCount(4);
    ui->resultsTableWidget->setHorizontalHeaderLabels({"Name", "Price", "Genre", "Recommendations"});
    ui->resultsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->resultsTableWidget->verticalHeader()->setVisible(false);
    ui->resultsTableWidget->setShowGrid(false);
    ui->resultsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resultsTableWidget->setRowCount(0);
    ui->searchModeButton->setCheckable(true);
    updateSearchModeButton();

    this->setStyleSheet(R"(
        QMainWindow {background-color: #0d1324;}
        QWidget {
            color: #f3f4f6;
            font-size: 13px;
        }
        QLabel#titleLabel {
            font-size: 26px;
            font-weight: bold;
            color: white;
            padding: 8px;
        }
        QGroupBox {
            background-color: #1b2336;
            border: 1px solid #2b3550;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 16px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 4px 0 4px;
            color: white;
        }
        QLineEdit, QComboBox, QDoubleSpinBox {
            background-color: #2a3145;
            border: 1px solid #3a4258;
            border-radius: 6px;
            padding: 6px;
            color: white;
            min-height: 28px;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QPushButton {
            background-color: #6d5dfc;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #7c6cff;
        }
        QTableWidget {
            background-color: #111827;
            border: 1px solid #2b3550;
            border-radius: 8px;
            color: white;
            gridline-color: #111827;
        }
        QHeaderView::section {
            background-color: #1b2336;
            color: white;
            border: none;
            padding: 8px;
            font-weight: bold;
        }
    )");

    connect(ui->searchButton, &QPushButton::clicked, this, &Window::runSearch);
    connect(ui->searchModeButton, &QPushButton::clicked, this, [this]() {
        useOrderedSearch = !useOrderedSearch;
        updateSearchModeButton();
    });

    QString dbPath = QCoreApplication::applicationDirPath() + "/../database/games.db";

    if (!db.open(dbPath.toStdString())) {
        QMessageBox::critical(this, "Database Error",
                              "Could not open games.db\n\nPath tried:\n" + dbPath);
        return;
    }

    loadStores();
}

void Window::loadStores() {
    std::vector<Game> allGames = db.getAllGames();

    for (const Game& game : allGames) {
        insertGame(orderedStore, game);
        insertGame(unorderedStore, game);
    }

    ui->genreComboBox->clear();
    ui->genreComboBox->addItems(buildDropdownOptions(orderedStore));

    statusBar()->showMessage(
        QString("Loaded %1 games. Choose filters and click Search.")
            .arg(static_cast<int>(allGames.size()))
    );
}

void Window::updateSearchModeButton() {
    ui->searchModeButton->setChecked(!useOrderedSearch);
    ui->searchModeButton->setText(
        useOrderedSearch ? "Search Mode: Ordered" : "Search Mode: Unordered"
    );
}

void Window::runSearch() {
    double minPriceInput = ui->minPriceSpinBox->value();
    double maxPriceInput = ui->maxPriceSpinBox->value();
    QString genre = ui->genreComboBox->currentText();
    QString sortOption = ui->sortComboBox->currentText();

    if (minPriceInput > maxPriceInput) {
        QMessageBox::warning(this, "Invalid Price Range",
                             "Minimum price cannot be greater than maximum price.");
        return;
    }

    int minPrice = static_cast<int>(minPriceInput * 100);
    int maxPrice = static_cast<int>(maxPriceInput * 100);

    ui->resultsTableWidget->setRowCount(0);
    ui->resultsTableWidget->setUpdatesEnabled(false);

    if (useOrderedSearch) {
        auto orderedStart = std::chrono::high_resolution_clock::now();
        auto orderedPrice = searchPriceOrdered(orderedStore, minPrice, maxPrice);

        std::vector<std::string> orderedResult;
        if (genre == "Any") {
            orderedResult.assign(orderedPrice.begin(), orderedPrice.end());
        } else {
            auto orderedGenre = searchGenreOrdered(orderedStore, genre.toStdString());
            orderedResult = intersect(orderedPrice, orderedGenre);
        }

        auto orderedEnd = std::chrono::high_resolution_clock::now();
        sortResults(orderedResult, orderedStore, sortOption);

        int rowsToShow = static_cast<int>(orderedResult.size());
        for (int i = 0; i < rowsToShow; i++) {
            const Game& game = orderedStore.gamesById.at(orderedResult[i]);

            int row = ui->resultsTableWidget->rowCount();
            ui->resultsTableWidget->insertRow(row);

            ui->resultsTableWidget->setItem(row, 0,
                new QTableWidgetItem(QString::fromStdString(game.name)));
            ui->resultsTableWidget->setItem(row, 1,
                new QTableWidgetItem(QString("$%1").arg(game.price, 0, 'f', 2)));

            QString displayGenre = game.tags.empty()
                ? QString::fromStdString(game.genres)
                : QString::fromStdString(game.tags);
            ui->resultsTableWidget->setItem(row, 2, new QTableWidgetItem(displayGenre));

            ui->resultsTableWidget->setItem(
                row, 3, new QTableWidgetItem(QString::number(game.recommendations))
            );
        }

        ui->resultsTableWidget->setUpdatesEnabled(true);
        auto orderedTime = std::chrono::duration_cast<std::chrono::microseconds>(orderedEnd - orderedStart).count();
        statusBar()->showMessage(
            QString("Showing %1 of %2 game(s) | mode: ordered | time: %3 us")
                .arg(rowsToShow)
                .arg(static_cast<int>(orderedResult.size()))
                .arg(orderedTime)
        );
        return;
    }

    auto unorderedStart = std::chrono::high_resolution_clock::now();
    auto unorderedPrice = searchPriceUnordered(unorderedStore, minPrice, maxPrice);

    std::vector<std::string> unorderedResult;
    if (genre == "Any") {
        unorderedResult.assign(unorderedPrice.begin(), unorderedPrice.end());
    } else {
        auto unorderedGenre = searchGenreUnordered(unorderedStore, genre.toStdString());
        unorderedResult = intersect(unorderedPrice, unorderedGenre);
    }

    auto unorderedEnd = std::chrono::high_resolution_clock::now();
    sortResults(unorderedResult, unorderedStore, sortOption);

    int rowsToShow = static_cast<int>(unorderedResult.size());
    for (int i = 0; i < rowsToShow; i++) {
        const Game& game = unorderedStore.gamesById.at(unorderedResult[i]);

        int row = ui->resultsTableWidget->rowCount();
        ui->resultsTableWidget->insertRow(row);

        ui->resultsTableWidget->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(game.name)));
        ui->resultsTableWidget->setItem(row, 1,
            new QTableWidgetItem(QString("$%1").arg(game.price, 0, 'f', 2)));

        QString displayGenre = game.tags.empty()
            ? QString::fromStdString(game.genres)
            : QString::fromStdString(game.tags);
        ui->resultsTableWidget->setItem(row, 2, new QTableWidgetItem(displayGenre));

        ui->resultsTableWidget->setItem(
            row, 3, new QTableWidgetItem(QString::number(game.recommendations))
        );
    }

    ui->resultsTableWidget->setUpdatesEnabled(true);
    auto unorderedTime = std::chrono::duration_cast<std::chrono::microseconds>(unorderedEnd - unorderedStart).count();
    statusBar()->showMessage(
        QString("Showing %1 of %2 game(s) | mode: unordered | time: %3 us")
            .arg(rowsToShow)
            .arg(static_cast<int>(unorderedResult.size()))
            .arg(unorderedTime)
    );
}

Window::~Window() {
    delete ui;
}
