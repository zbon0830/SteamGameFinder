#include "Window.h"
#include "ui_Window.h"
#include <QStringList>
#include <QHeaderView>
#include <QAbstractItemView>

Window::Window(QWidget *parent)
    : QMainWindow(parent),
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