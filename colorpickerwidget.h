#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QWidget>
#include <QColor>
#include "ColorManager.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QGridLayout;

class ColorPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPickerWidget(ColorManager* colorManager, QWidget* parent = nullptr);
    ~ColorPickerWidget() = default;

private slots:
    void updateColorDisplays();
    void onPrimaryColorClicked();
    void onSecondaryColorClicked();
    void onSwapColorsClicked();
    void onApplyHexClicked();
    void onHistoryColorClicked();
    void onHexInputChanged(const QString& text);
    void onHexInputFocusChanged(bool hasFocus);

private:
    void setupUI();
    void setupConnections();
    void showColorDialog(bool isPrimary);
    void clearErrorState();
    void onHexInputFocusLost();
    ColorManager* m_colorManager;

    // UI элементы для основных цветов
    QPushButton* m_primaryColorButton;
    QPushButton* m_secondaryColorButton;
    QPushButton* m_swapButton;

    // Hex ввод
    QLineEdit* m_hexInput;

    // История цветов
    QWidget* m_colorHistoryWidget;
    QGridLayout* m_historyLayout;
    QList<QPushButton*> m_historyButtons;

    bool m_isErrorState;
    bool m_lastFocusState;
};

#endif // COLORPICKERWIDGET_H
