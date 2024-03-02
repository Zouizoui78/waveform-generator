#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "QApplication"
#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
