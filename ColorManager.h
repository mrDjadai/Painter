#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QSettings>

class ColorManager : public QObject
{
    Q_OBJECT

public:
    ColorManager(QObject* parent = nullptr);

    QColor primaryColor() const { return m_primaryColor; }
    void setPrimaryColor(const QColor& color);

    QColor secondaryColor() const { return m_secondaryColor; }
    void setSecondaryColor(const QColor& color);

    const QVector<QColor>& colorHistory() const { return m_colorHistory; }
    void addToHistory(const QColor& color);
    void clearHistory();

    void swapColors();

signals:
    void primaryColorChanged(const QColor& color);
    void secondaryColorChanged(const QColor& color);
    void colorHistoryChanged();

private:
    QColor m_primaryColor;
    QColor m_secondaryColor;
    QVector<QColor> m_colorHistory;
};

#endif // COLORMANAGER_H
