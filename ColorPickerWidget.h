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
    void onHistoryColorClicked();

private:
    void setupUI();
    void setupConnections();
    void showColorDialog(bool isPrimary);
    ColorManager* m_colorManager;

    QPushButton* m_primaryColorButton;
    QPushButton* m_secondaryColorButton;
    QPushButton* m_swapButton;

    QWidget* m_colorHistoryWidget;
    QGridLayout* m_historyLayout;
    QList<QPushButton*> m_historyButtons;
};

#endif // COLORPICKERWIDGET_H
