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
    explicit ColorManager(QObject* parent = nullptr);

    // Основной цвет
    QColor primaryColor() const { return m_primaryColor; }
    void setPrimaryColor(const QColor& color);

    // Вторичный цвет
    QColor secondaryColor() const { return m_secondaryColor; }
    void setSecondaryColor(const QColor& color);

    // История цветов
    const QVector<QColor>& colorHistory() const { return m_colorHistory; }
    void addToHistory(const QColor& color);
    void clearHistory();

    // Сохранение/загрузка
    void saveColors();
    void loadColors();

    // Вспомогательные методы
    QString colorToHex(const QColor& color) const;
    QColor hexToColor(const QString& hex) const;

    void swapColors();

signals:
    void primaryColorChanged(const QColor& color);
    void secondaryColorChanged(const QColor& color);
    void colorHistoryChanged();

private:
    QColor m_primaryColor;
    QColor m_secondaryColor;
    QVector<QColor> m_colorHistory;
    static const int MAX_HISTORY_SIZE = 10;
};

#endif // COLORMANAGER_H
