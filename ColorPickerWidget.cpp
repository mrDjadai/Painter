#include "ColorPickerWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QColorDialog>
#include <QGridLayout>
#include <QRegularExpression>
#include <QKeyEvent>
#include <QApplication>
#include "Config.h"

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

    QHBoxLayout* topRowLayout = new QHBoxLayout();
    topRowLayout->setSpacing(15);

    QHBoxLayout* colorsLayout = new QHBoxLayout();
    colorsLayout->setSpacing(15);

    QVBoxLayout* primaryLayout = new QVBoxLayout();
    primaryLayout->setSpacing(20);
    primaryLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* primaryLabel = new QLabel("Первичный");
    primaryLabel->setAlignment(Qt::AlignCenter);
    primaryLabel->setFixedHeight(COLOR_HISTORY_LABEL_HEIGHT / 3);
    primaryLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px;")
                                    .arg(COLOR_BUTTON_TEXT_COLOR)
                                    .arg(COLOR_HISTORY_LABEL_FONT_SIZE));
    primaryLayout->addWidget(primaryLabel);

    m_primaryColorButton = new QPushButton();
    m_primaryColorButton->setFixedSize(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE);
    m_primaryColorButton->setCursor(Qt::PointingHandCursor);
    m_primaryColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #777777;")
                                            .arg(COLOR_BUTTON_BACKGROUND_PRIMARY));
    primaryLayout->addWidget(m_primaryColorButton, 0, Qt::AlignHCenter);

    colorsLayout->addLayout(primaryLayout);

    m_swapButton = new QPushButton("↔");
    m_swapButton->setFixedSize(SWAP_BUTTON_SIZE, SWAP_BUTTON_SIZE);
    m_swapButton->setCursor(Qt::PointingHandCursor);
    m_swapButton->setStyleSheet(QString(
                                    "QPushButton {"
                                    "   background-color: %1;"
                                    "   color: %2;"
                                    "   border: 1px solid %3;"
                                    "   border-radius: 5px;"
                                    "   font-size: 13px;"
                                    "   font-weight: bold;"
                                    "}"
                                    "QPushButton:hover {"
                                    "   background-color: %4;"
                                    "   border: 1px solid %5;"
                                    "}"
                                    "QPushButton:pressed {"
                                    "   background-color: %6;"
                                    "}")
                                    .arg(COLOR_SWAP_BUTTON_BACKGROUND)
                                    .arg(COLOR_SWAP_BUTTON_TEXT)
                                    .arg(COLOR_SWAP_BUTTON_BORDER)
                                    .arg(COLOR_SWAP_BUTTON_HOVER_BACKGROUND)
                                    .arg(COLOR_SWAP_BUTTON_HOVER_BORDER)
                                    .arg(COLOR_SWAP_BUTTON_PRESSED_BACKGROUND)
                                );
    colorsLayout->addWidget(m_swapButton, 0, Qt::AlignVCenter);

    QVBoxLayout* secondaryLayout = new QVBoxLayout();
    secondaryLayout->setSpacing(20);
    secondaryLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* secondaryLabel = new QLabel("Вторичный");
    secondaryLabel->setAlignment(Qt::AlignCenter);
    secondaryLabel->setFixedHeight(COLOR_HISTORY_LABEL_HEIGHT / 3);
    secondaryLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px;")
                                      .arg(COLOR_BUTTON_TEXT_COLOR)
                                      .arg(COLOR_HISTORY_LABEL_FONT_SIZE));
    secondaryLayout->addWidget(secondaryLabel);

    m_secondaryColorButton = new QPushButton();
    m_secondaryColorButton->setFixedSize(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE);
    m_secondaryColorButton->setCursor(Qt::PointingHandCursor);
    m_secondaryColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #777777;")
                                              .arg(COLOR_BUTTON_BACKGROUND_SECONDARY));
    secondaryLayout->addWidget(m_secondaryColorButton, 0, Qt::AlignHCenter);

    colorsLayout->addLayout(secondaryLayout);

    topRowLayout->addLayout(colorsLayout);
    topRowLayout->addStretch();

    mainLayout->addLayout(topRowLayout);

    QLabel* historyLabel = new QLabel("История цветов");
    historyLabel->setFixedHeight(COLOR_HISTORY_LABEL_HEIGHT);
    historyLabel->setStyleSheet(QString("color: %1; font-weight: %2; font-size: %3px;")
                                    .arg(COLOR_HISTORY_LABEL_COLOR)
                                    .arg(COLOR_HISTORY_LABEL_FONT_WEIGHT)
                                    .arg(COLOR_HISTORY_LABEL_FONT_SIZE));
    mainLayout->addWidget(historyLabel);

    m_colorHistoryWidget = new QWidget();
    m_colorHistoryWidget->setFixedHeight(COLOR_HISTORY_HEIGHT);
    m_colorHistoryWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_historyLayout = new QGridLayout(m_colorHistoryWidget);
    m_historyLayout->setSpacing(COLOR_HISTORY_SPACING);
    m_historyLayout->setContentsMargins(COLOR_HISTORY_MARGIN, COLOR_HISTORY_MARGIN,
                                        COLOR_HISTORY_MARGIN, COLOR_HISTORY_MARGIN);
    m_historyLayout->setAlignment(Qt::AlignTop);

    mainLayout->addWidget(m_colorHistoryWidget);
}

void ColorPickerWidget::setupConnections()
{
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
            colorButton->setFixedSize(12, 12);
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

            colorButton->setProperty("colorIndex", i);

            connect(colorButton, &QPushButton::clicked,
                    this, &ColorPickerWidget::onHistoryColorClicked);

            m_historyButtons.append(colorButton);
            m_historyLayout->addWidget(colorButton, 0, i);
        }
    }

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
