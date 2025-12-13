#include "ColorManager.h"
#include "Config.h"

ColorManager::ColorManager(QObject* parent)
    : QObject(parent)
    , m_primaryColor(Qt::black)
    , m_secondaryColor(Qt::white)
{
    m_primaryColor = QColor((QString)PRIMARY_HEX);
    m_secondaryColor = QColor((QString)SECONDARY_HEX);
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
    for (int i = 0; i < m_colorHistory.size(); ++i) {
        if (m_colorHistory[i] == color) {
            m_colorHistory.move(i, 0);
            emit colorHistoryChanged();
            return;
        }
    }

    m_colorHistory.prepend(color);

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


void ColorManager::swapColors()
{
    QColor temp = m_primaryColor;
    m_primaryColor = m_secondaryColor;
    m_secondaryColor = temp;

    emit primaryColorChanged(m_primaryColor);
    emit secondaryColorChanged(m_secondaryColor);
}
