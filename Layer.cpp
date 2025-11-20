#include "Layer.h"
#include <QPainter>

Layer::Layer(const QSize& size, const QString& name)
    : m_name(name)
    , m_image(size, QImage::Format_ARGB32_Premultiplied)
{
    m_image.fill(Qt::transparent);
}

void Layer::paint(QPainter& painter, const QRect& destRect)
{
    if (!m_visible || m_opacity <= 0.0f)
        return;

    if (m_opacity < 1.0f) {
        painter.setOpacity(m_opacity);
    }

    painter.drawImage(destRect, m_image, m_image.rect());

    if (m_opacity < 1.0f) {
        painter.setOpacity(1.0f);
    }
}
