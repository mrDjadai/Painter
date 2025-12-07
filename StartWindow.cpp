#include "StartWindow.h"
#include <QFileDialog>
#include <QHBoxLayout>

StartWindow::StartWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Стартовое окно");

    m_openButton = new QPushButton("Открыть файл");
    m_createButton = new QPushButton("Создать новый холст");

    m_widthSpin = new QSpinBox();
    m_widthSpin->setRange(100, 5000);
    m_widthSpin->setValue(800);

    m_heightSpin = new QSpinBox();
    m_heightSpin->setRange(100, 5000);
    m_heightSpin->setValue(600);

    QLabel* widthLabel = new QLabel("Ширина:");
    QLabel* heightLabel = new QLabel("Высота:");

    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(widthLabel);
    sizeLayout->addWidget(m_widthSpin);
    sizeLayout->addWidget(heightLabel);
    sizeLayout->addWidget(m_heightSpin);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_openButton);
    mainLayout->addWidget(m_createButton);
    mainLayout->addLayout(sizeLayout);

    connect(m_openButton, &QPushButton::clicked, this, &StartWindow::onOpenFile);
    connect(m_createButton, &QPushButton::clicked, this, &StartWindow::onCreateCanvas);
}

void StartWindow::onOpenFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть проект", QString(), "Painter (*.ptr)");
    if (!filename.isEmpty()) {
        emit openFileRequested(filename);
    }
}

void StartWindow::onCreateCanvas()
{
    int w = m_widthSpin->value();
    int h = m_heightSpin->value();
    emit createNewCanvasRequested(w, h);
}
