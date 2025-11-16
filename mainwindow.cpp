#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetShortcuts();
}

void  MainWindow::SetShortcuts()
{
    QShortcut *ctrlZ = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(ctrlZ, &QShortcut::activated, this, &MainWindow::HandleUndo);

    QShortcut *ctrlY = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(ctrlY, &QShortcut::activated, this, &MainWindow::HandleRedo);
}

void MainWindow::HandleUndo()
{
    commandManager.Undo();
}

void MainWindow::HandleRedo()
{
    commandManager.Redo();
}

MainWindow::~MainWindow()
{
    delete ui;
}
