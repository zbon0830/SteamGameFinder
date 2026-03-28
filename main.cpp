#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

#include "database/GameDB.h"
#include "database/GameSearch.h"
#include "UI/Window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GameDB db;
    const QStringList dbCandidates = {
        "database/games.db",
        "../database/games.db"
    };

    bool opened = false;
    for (const QString& candidate : dbCandidates) {
        if (QFileInfo::exists(candidate) && db.open(candidate.toStdString())) {
            opened = true;
            break;
        }
    }

    GameSearch searchEngine;
    if (opened) {
        searchEngine.load(db.getAllGames());
    }

    Window window(&searchEngine);
    window.show();

    const int exitCode = app.exec();
    db.close();
    return exitCode;
}
