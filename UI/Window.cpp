#include "Window.h"
#include "ui_Window.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidgetItem>
#include <QStringList>

Window::Window(GameSearch* searchEngine, QWidget *parent)
    : QMainWindow(parent),
      searchEngine(searchEngine),
      ui(new Ui::Window) {
    ui->setupUi(this);
    setWindowTitle("Steam Bundle Finder");
    resize(1000, 700);
    QStringList genres = {
        "Any",
        "Action",
        "Adventure",
        "Anime",
        "Casual",
        "Co-op",
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
    ui->genreComboBox->addItems(genres);
    ui->modeComboBox->addItems({"Ordered", "Unordered"});
    ui->minPriceSpinBox->setPrefix("$");
    ui->maxPriceSpinBox->setPrefix("$");
    ui->minPriceSpinBox->setMaximum(9999.0);
    ui->maxPriceSpinBox->setMaximum(9999.0);
    ui->minPriceSpinBox->setValue(0.0);
    ui->maxPriceSpinBox->setValue(60.0);
    ui->minPriceSpinBox->setDecimals(2);
    ui->maxPriceSpinBox->setDecimals(2);
    ui->resultsTableWidget->setColumnCount(4);
    ui->resultsTableWidget->setHorizontalHeaderLabels({"Name", "Price", "Genre", "Reviews"});
    ui->resultsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->resultsTableWidget->verticalHeader()->setVisible(false);
    ui->resultsTableWidget->setShowGrid(false);
    ui->resultsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resultsTableWidget->setRowCount(0);
    connect(ui->searchButton, &QPushButton::clicked, this, &Window::runSearch);
    statusBar()->showMessage(searchEngine != nullptr && !searchEngine->empty() ? "Ready" : "No game data loaded");
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
}

Window::~Window() {
    delete ui;
}

void Window::runSearch() {
    if (searchEngine == nullptr || searchEngine->empty()) {
        statusBar()->showMessage("No game data loaded");
        ui->resultsTableWidget->setRowCount(0);
        return;
    }

    const double minPrice = ui->minPriceSpinBox->value();
    const double maxPrice = ui->maxPriceSpinBox->value();
    if (minPrice > maxPrice) {
        statusBar()->showMessage("Min price must be less than or equal to max price");
        return;
    }

    QString genreText = ui->genreComboBox->currentText();
    if (genreText == "Any") {
        genreText.clear();
    }

    const SearchMode mode = ui->modeComboBox->currentText() == "Ordered"
        ? SearchMode::Ordered
        : SearchMode::Unordered;

    const SearchResults results = searchEngine->search(
        genreText.toStdString(),
        static_cast<int>(minPrice * 100.0),
        static_cast<int>(maxPrice * 100.0),
        mode
    );

    populateResults(results);

    statusBar()->showMessage(QString("%1 results in %2 us using %3 search")
                                 .arg(results.games.size())
                                 .arg(results.elapsedMicroseconds)
                                 .arg(ui->modeComboBox->currentText().toLower()));
}

void Window::populateResults(const SearchResults& results) {
    ui->resultsTableWidget->setRowCount(static_cast<int>(results.games.size()));

    for (int row = 0; row < static_cast<int>(results.games.size()); ++row) {
        const Game& game = results.games[static_cast<std::size_t>(row)];

        ui->resultsTableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(game.name)));
        ui->resultsTableWidget->setItem(row, 1, new QTableWidgetItem(QString("$%1").arg(game.price, 0, 'f', 2)));
        ui->resultsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(game.genres)));
        ui->resultsTableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(game.reviews)));
    }
}
