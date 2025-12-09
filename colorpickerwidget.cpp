#include "ColorPickerWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QColorDialog>
#include <QGridLayout>
#include <QDebug>
#include <QRegularExpression>
#include <QKeyEvent>
#include <QApplication>

ColorPickerWidget::ColorPickerWidget(ColorManager* colorManager, QWidget* parent)
    : QWidget(parent)
    , m_colorManager(colorManager)
    , m_primaryColorButton(nullptr)
    , m_secondaryColorButton(nullptr)
    , m_swapButton(nullptr)
    , m_colorHistoryWidget(nullptr)
    , m_historyLayout(nullptr)
{
    setupUI();
    setupConnections();
    updateColorDisplays();

    if (m_colorManager) {
        connect(m_colorManager, &ColorManager::primaryColorChanged,
                this, &ColorPickerWidget::updateColorDisplays);
        connect(m_colorManager, &ColorManager::secondaryColorChanged,
                this, &ColorPickerWidget::updateColorDisplays);
        connect(m_colorManager, &ColorManager::colorHistoryChanged,
                this, &ColorPickerWidget::updateColorDisplays);
    }
}

void ColorPickerWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(8, 4, 8, 8);

    // ===== ПЕРВАЯ СТРОКА:ОСНОВНЫЕ ЦВЕТА =====
    QHBoxLayout* topRowLayout = new QHBoxLayout();
    topRowLayout->setSpacing(15);

    // ===== ПРАВАЯ ЧАСТЬ: ОСНОВНЫЕ ЦВЕТА =====
    QHBoxLayout* colorsLayout = new QHBoxLayout();
    colorsLayout->setSpacing(15);

    // Первичный цвет
    QVBoxLayout* primaryLayout = new QVBoxLayout();
    primaryLayout->setSpacing(20);
    primaryLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* primaryLabel = new QLabel("Первичный");
    primaryLabel->setAlignment(Qt::AlignCenter);
    primaryLabel->setFixedHeight(16);
    primaryLabel->setStyleSheet("color: white; font-weight: bold; font-size: 12px;");
    primaryLayout->addWidget(primaryLabel);

    m_primaryColorButton = new QPushButton();
    m_primaryColorButton->setFixedSize(36, 36);
    m_primaryColorButton->setCursor(Qt::PointingHandCursor);
    primaryLayout->addWidget(m_primaryColorButton, 0, Qt::AlignHCenter);

    colorsLayout->addLayout(primaryLayout);

    // Кнопка обмена цветами
    m_swapButton = new QPushButton("↔");
    m_swapButton->setFixedSize(28, 28);
    m_swapButton->setCursor(Qt::PointingHandCursor);
    m_swapButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #555555;"
        "   color: white;"
        "   border: 1px solid #777777;"
        "   border-radius: 5px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #666666;"
        "   border: 1px solid #888888;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #444444;"
        "}"
        );
    colorsLayout->addWidget(m_swapButton, 0, Qt::AlignVCenter);

    // Вторичный цвет
    QVBoxLayout* secondaryLayout = new QVBoxLayout();
    secondaryLayout->setSpacing(20);
    secondaryLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* secondaryLabel = new QLabel("Вторичный");
    secondaryLabel->setAlignment(Qt::AlignCenter);
    secondaryLabel->setFixedHeight(16);
    secondaryLabel->setStyleSheet("color: white; font-weight: bold; font-size: 12px;");
    secondaryLayout->addWidget(secondaryLabel);

    m_secondaryColorButton = new QPushButton();
    m_secondaryColorButton->setFixedSize(36, 36);
    m_secondaryColorButton->setCursor(Qt::PointingHandCursor);
    secondaryLayout->addWidget(m_secondaryColorButton, 0, Qt::AlignHCenter);

    colorsLayout->addLayout(secondaryLayout);

    topRowLayout->addLayout(colorsLayout);
    topRowLayout->addStretch();

    mainLayout->addLayout(topRowLayout);


    // ===== ВТОРАЯ СТРОКА: ИСТОРИЯ ЦВЕТОВ =====
    QLabel* historyLabel = new QLabel("История цветов");
    historyLabel->setFixedHeight(45);
    historyLabel->setStyleSheet("color: white; font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(historyLabel);

    // Контейнер для истории цветов
    m_colorHistoryWidget = new QWidget();
    m_colorHistoryWidget->setFixedHeight(70);
    m_colorHistoryWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Создаем grid layout для истории
    m_historyLayout = new QGridLayout(m_colorHistoryWidget);
    m_historyLayout->setSpacing(2);
    m_historyLayout->setContentsMargins(2, 2, 2, 2);
    m_historyLayout->setAlignment(Qt::AlignTop);


    mainLayout->addWidget(m_colorHistoryWidget);
}
void ColorPickerWidget::setupConnections()
{
    // Основные цвета
    connect(m_primaryColorButton, &QPushButton::clicked,
            this, &ColorPickerWidget::onPrimaryColorClicked);

    connect(m_secondaryColorButton, &QPushButton::clicked,
            this, &ColorPickerWidget::onSecondaryColorClicked);

    connect(m_swapButton, &QPushButton::clicked,
            this, &ColorPickerWidget::onSwapColorsClicked);
}

void ColorPickerWidget::updateColorDisplays()
{
    if (!m_colorManager) return;

    // Обновляем основные цвета
    QColor primaryColor = m_colorManager->primaryColor();
    QColor secondaryColor = m_colorManager->secondaryColor();

    QString primaryStyle = QString(
                               "QPushButton {"
                               "   background-color: %1;"
                               "   border: 2px solid #000000;"
                               "   border-radius: 5px;" // Увеличили радиус скругления
                               "}"
                               "QPushButton:hover {"
                               "   border: 2px solid #0066cc;"
                               "}"
                               ).arg(primaryColor.name());

    QString secondaryStyle = QString(
                                 "QPushButton {"
                                 "   background-color: %1;"
                                 "   border: 2px solid #000000;"
                                 "   border-radius: 5px;" // Увеличили радиус скругления
                                 "}"
                                 "QPushButton:hover {"
                                 "   border: 2px solid #0066cc;"
                                 "}"
                                 ).arg(secondaryColor.name());

    m_primaryColorButton->setStyleSheet(primaryStyle);
    m_primaryColorButton->setToolTip(QString("Click to change primary color\nCurrent: %1")
                                         .arg(primaryColor.name()));

    m_secondaryColorButton->setStyleSheet(secondaryStyle);
    m_secondaryColorButton->setToolTip(QString("Click to change secondary color\nCurrent: %1")
                                           .arg(secondaryColor.name()));

    // Обновляем историю цветов
    // Очищаем старые кнопки
    for (QPushButton* button : m_historyButtons) {
        m_historyLayout->removeWidget(button);
        delete button;
    }
    m_historyButtons.clear();

    const QVector<QColor>& history = m_colorManager->colorHistory();

    if (!history.isEmpty())
    {
        int maxColors = qMin(16, history.size());
        for (int i = 0; i < maxColors; ++i) {
            QPushButton* colorButton = new QPushButton();
            colorButton->setFixedSize(12, 12); // Увеличили размер кнопок истории
            colorButton->setCursor(Qt::PointingHandCursor);

            QString buttonStyle = QString(
                                      "QPushButton {"
                                      "   background-color: %1;"
                                      "   border: 1px solid #666666;"
                                      "   border-radius: 3px;" // Увеличили радиус
                                      "}"
                                      "QPushButton:hover {"
                                      "   border: 1px solid #0066cc;"
                                      "}"
                                      ).arg(history[i].name());

            colorButton->setStyleSheet(buttonStyle);
            colorButton->setToolTip(history[i].name());

            // Сохраняем индекс цвета
            colorButton->setProperty("colorIndex", i);

            // Подключаем сигнал
            connect(colorButton, &QPushButton::clicked,
                    this, &ColorPickerWidget::onHistoryColorClicked);

            m_historyButtons.append(colorButton);
            m_historyLayout->addWidget(colorButton, 0, i);
        }
    }

    // Обновляем layout
    m_colorHistoryWidget->updateGeometry();
}

void ColorPickerWidget::onPrimaryColorClicked()
{
    showColorDialog(true);
}

void ColorPickerWidget::onSecondaryColorClicked()
{
    showColorDialog(false);
}

void ColorPickerWidget::onSwapColorsClicked()
{
    if (m_colorManager) {
        m_colorManager->swapColors();
    }
}

void ColorPickerWidget::onHistoryColorClicked()
{
    if (!m_colorManager) return;

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int index = button->property("colorIndex").toInt();
    const QVector<QColor>& history = m_colorManager->colorHistory();

    if (index >= 0 && index < history.size()) {
        m_colorManager->setPrimaryColor(history[index]);
    }
}


void ColorPickerWidget::showColorDialog(bool isPrimary)
{
    if (!m_colorManager) return;

    QColor currentColor = isPrimary ? m_colorManager->primaryColor()
                                    : m_colorManager->secondaryColor();

    QColorDialog dialog(this);
    dialog.setCurrentColor(currentColor);
    dialog.setOption(QColorDialog::ShowAlphaChannel, true);
    dialog.setOption(QColorDialog::DontUseNativeDialog, false);
    dialog.setWindowTitle(isPrimary ? "Select Primary Color" : "Select Secondary Color");

    if (dialog.exec() == QDialog::Accepted) {
        QColor color = dialog.selectedColor();
        if (isPrimary) {
            m_colorManager->setPrimaryColor(color);
        } else {
            m_colorManager->setSecondaryColor(color);
        }
    }
}
