#include "LayerView.h"
#include <QPaintEvent>
#include <QDebug>
#define viewOffset 10

LayerView::LayerView(LayerManager* layerManager, QWidget* parent)
    : QWidget(parent)
    , m_layerManager(layerManager)
{
    setMinimumSize(400, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Устанавливаем фон, чтобы видеть границы виджета
    setStyleSheet("background-color: #2d2d2d;");
}

QSize LayerView::sizeHint() const
{
    return QSize(800, 600);
}

void LayerView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Заливаем фон
    painter.fillRect(rect(), Qt::white);

    // Вычисляем область для отрисовки (с отступами)
    QRect targetRect = rect().adjusted(viewOffset, viewOffset, -viewOffset, -viewOffset);

    if (m_layerManager) {
        m_layerManager->renderLayers(painter, targetRect);
    }
}
