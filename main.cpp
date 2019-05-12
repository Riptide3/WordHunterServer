#include "Server/server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Database db;
    db.init();
    Server w;
    w.show();

    return a.exec();
}
