#ifndef WINDOW_H
#define WINDOW_H
#include <QMainWindow>
#include "GameDB.h"
#include "Logic.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow {
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

private:
    Ui::Window *ui;
    GameDB db;
    OrderedStore orderedStore;
    UnorderedStore unorderedStore;
    bool useOrderedSearch = true;
    void loadStores();
    void runSearch();
    void updateSearchModeButton();
};

#endif
