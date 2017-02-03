#include "gamewindow.h"
#include "configure.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen* splash = new QSplashScreen;
    splash->setPixmap(QPixmap(QString(QObject::tr(":/images/res/desktopPicture.png"))));
    splash->show();

    Qt::Alignment topRight = Qt::AlignTop | Qt::AlignRight;
    splash->showMessage(QObject::tr("Setting up the main window..."), topRight, Qt::white);

    GameWindow* w = new GameWindow;

    // 加载配置信息
    splash->showMessage(QObject::tr("Loading configure..."), topRight, Qt::white);
    // readConfigure();

    QThread::sleep(2);

    w->show();
    splash->finish(w);
    delete splash;

    return a.exec();
}
