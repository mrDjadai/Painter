#include "ColorManager.h"
#include <QDebug>

ColorManager::ColorManager(QObject* parent)
    : QObject(parent)
    , m_primaryColor(Qt::black)
    , m_secondaryColor(Qt::white)
{
    loadColors();
}

void ColorManager::setPrimaryColor(const QColor& color)
{
    if (m_primaryColor != color) {
        m_primaryColor = color;
        addToHistory(color);
        emit primaryColorChanged(color);
    }
}

void ColorManager::setSecondaryColor(const QColor& color)
{
    if (m_secondaryColor != color) {
        m_secondaryColor = color;
        addToHistory(color);
        emit secondaryColorChanged(color);
    }
}

void ColorManager::addToHistory(const QColor& color)
{
    // Проверяем, есть ли уже такой цвет в истории
    for (int i = 0; i < m_colorHistory.size(); ++i) {
        if (m_colorHistory[i] == color) {
            // Цвет уже есть - перемещаем его в начало
            m_colorHistory.move(i, 0);
            emit colorHistoryChanged();
            return;
        }
    }

    // Добавляем новый цвет в начало
    m_colorHistory.prepend(color);

    // Ограничиваем размер истории
    if (m_colorHistory.size() > MAX_HISTORY_SIZE) {
        m_colorHistory.resize(MAX_HISTORY_SIZE);
    }

    emit colorHistoryChanged();
}

void ColorManager::clearHistory()
{
    m_colorHistory.clear();
    emit colorHistoryChanged();
}

void ColorManager::saveColors()
{
    QSettings settings;

    settings.setValue("colors/primary", m_primaryColor.name());
    settings.setValue("colors/secondary", m_secondaryColor.name());

    // Сохраняем историю
    settings.beginWriteArray("colors/history");
    for (int i = 0; i < m_colorHistory.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("color", m_colorHistory[i].name());
    }
    settings.endArray();
}

void ColorManager::loadColors()
{
    QSettings settings;

    // Загружаем основные цвета
    QString primaryHex = settings.value("colors/primary", "#000000").toString();
    QString secondaryHex = settings.value("colors/secondary", "#ffffff").toString();

    m_primaryColor = QColor(primaryHex);
    m_secondaryColor = QColor(secondaryHex);

    // Загружаем историю
    int size = settings.beginReadArray("colors/history");
    m_colorHistory.clear();
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString colorHex = settings.value("color").toString();
        m_colorHistory.append(QColor(colorHex));
    }
    settings.endArray();
}

QString ColorManager::colorToHex(const QColor& color) const
{
    return color.name();
}

QColor ColorManager::hexToColor(const QString& hex) const
{
    return QColor(hex);
}

void ColorManager::swapColors()
{
    QColor temp = m_primaryColor;
    m_primaryColor = m_secondaryColor;
    m_secondaryColor = temp;

    emit primaryColorChanged(m_primaryColor);
    emit secondaryColorChanged(m_secondaryColor);
}
