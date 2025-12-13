#include "StartWindow.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include "Config.h"

StartWindow::StartWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Painter");

    m_openButton = new QPushButton("Открыть файл");
    m_openImageButton = new QPushButton("Открыть изображение");
    m_createButton = new QPushButton("Создать новый холст");


    m_widthSpin = new QSpinBox();
    m_widthSpin->setRange(MIN_CANVAS_SIZE, MAX_CANVAS_SIZE);
    m_widthSpin->setValue(DEFAULT_CANVAS_WIDTH);

    m_heightSpin = new QSpinBox();
    m_heightSpin->setRange(MIN_CANVAS_SIZE, MAX_CANVAS_SIZE);
    m_heightSpin->setValue(DEFAULT_CANVAS_HEIGHT);

    QLabel* widthLabel = new QLabel("Ширина:");
    QLabel* heightLabel = new QLabel("Высота:");

    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(widthLabel);
    sizeLayout->addWidget(m_widthSpin);
    sizeLayout->addWidget(heightLabel);
    sizeLayout->addWidget(m_heightSpin);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_openButton);
    mainLayout->addWidget(m_openImageButton);
    mainLayout->addWidget(m_createButton);
    mainLayout->addLayout(sizeLayout);

    connect(m_openImageButton, &QPushButton::clicked, this, &StartWindow::onOpenImage);
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

void StartWindow::onOpenImage()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Открыть изображение",
        QString(),
        "Изображения (*.png *.jpg *.jpeg *.bmp)"
        );

    if (!filename.isEmpty()) {
        emit openImageRequested(filename);
    }
}
