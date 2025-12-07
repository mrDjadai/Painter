#include "mainwindow.h"
#include "StartWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StartWindow start;
    start.show();  // показываем только стартовое окно

    QObject::connect(&start, &StartWindow::openFileRequested, [&](const QString& filename){
        MainWindow* mainWindow = new MainWindow();
 //       mainWindow->loadProject(filename);
        start.close();
        mainWindow->show();
    });

    QObject::connect(&start, &StartWindow::createNewCanvasRequested, [&](int width, int height){
        MainWindow* mainWindow = new MainWindow();
      //  mainWindow->createNewCanvas(width, height);
        start.close();
        mainWindow->show();
    });

    return a.exec();
}
