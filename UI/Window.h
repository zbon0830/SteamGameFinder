#ifndef WINDOW_H
#define WINDOW_H

#include "../database/GameSearch.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window(GameSearch* searchEngine, QWidget *parent = nullptr);
    ~Window();

private:
    void runSearch();
    void populateResults(const SearchResults& results);

    GameSearch* searchEngine;
    Ui::Window *ui;
};

#endif
