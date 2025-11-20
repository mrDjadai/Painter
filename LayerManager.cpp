#include "LayerManager.h"
#include <QPainter>
#include <QFile>
#include <QDataStream>
#include <QBuffer>

LayerManager::LayerManager(QObject* parent)
    : QObject(parent)
{
}

void LayerManager::addLayer(std::unique_ptr<Layer> layer)
{
    if (!layer) return;

    m_layers.push_back(std::move(layer));
    if (!m_activeLayer) {
        setActiveLayer(static_cast<int>(m_layers.size()) - 1);
    }

    emit layersChanged();
}

void LayerManager::removeLayer(int index)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return;

    bool wasActive = (m_activeLayer == m_layers[index].get());

    m_layers.erase(m_layers.begin() + index);

    if (wasActive || !m_activeLayer) {
        if (m_layers.empty()) {
            m_activeLayer = nullptr;
        } else {
            setActiveLayer(qMin(index, static_cast<int>(m_layers.size()) - 1));
        }
    }

    emit layersChanged();
}

void LayerManager::moveLayer(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= static_cast<int>(m_layers.size()) ||
        toIndex < 0 || toIndex >= static_cast<int>(m_layers.size()) ||
        fromIndex == toIndex)
        return;

    auto layer = std::move(m_layers[fromIndex]);
    m_layers.erase(m_layers.begin() + fromIndex);
    m_layers.insert(m_layers.begin() + toIndex, std::move(layer));

    emit layersChanged();
}

void LayerManager::duplicateLayer(int index)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return;

    const Layer* sourceLayer = m_layers[index].get();
    auto newLayer = std::make_unique<Layer>(sourceLayer->image().size(),
                                            sourceLayer->name() + " Copy");

    newLayer->setImage(sourceLayer->image().copy());
    newLayer->setOpacity(sourceLayer->opacity());
    newLayer->setVisible(sourceLayer->isVisible());

    addLayer(std::move(newLayer));
}

Layer* LayerManager::createNewLayer(const QSize& size, const QString& name)
{
    auto layer = std::make_unique<Layer>(size, name);
    Layer* result = layer.get();
    addLayer(std::move(layer));
    return result;
}

Layer* LayerManager::createBackgroundLayer(const QSize& size, Qt::GlobalColor color)
{
    auto layer = std::make_unique<Layer>(size, "Background");
    layer->image().fill(color);
    Layer* result = layer.get();
    addLayer(std::move(layer));
    return result;
}

Layer* LayerManager::layerAt(int index)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return nullptr;
    return m_layers[index].get();
}

const Layer* LayerManager::layerAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return nullptr;
    return m_layers[index].get();
}

void LayerManager::setActiveLayer(int index)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size())) {
        m_activeLayer = nullptr;
        return;
    }

    Layer* newActive = m_layers[index].get();
    if (m_activeLayer != newActive) {
        m_activeLayer = newActive;
        emit activeLayerChanged(index);
    }
}

int LayerManager::activeLayerIndex() const
{
    if (!m_activeLayer) return -1;

    for (int i = 0; i < static_cast<int>(m_layers.size()); ++i) {
        if (m_layers[i].get() == m_activeLayer) {
            return i;
        }
    }
    return -1;
}

QImage LayerManager::compositeImage(const QSize& size) const
{
    QImage result(size, QImage::Format_ARGB32_Premultiplied);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    renderLayers(painter, QRect(QPoint(0, 0), size));

    return result;
}

void LayerManager::renderLayers(QPainter& painter, const QRect& destRect) const
{
    // Рендерим снизу вверх (от первого к последнему)
    for (const auto& layer : m_layers) {
        layer->paint(painter, destRect);
    }
}

bool LayerManager::saveProject(const QString& filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_15);

    // Заголовок
    stream << QString("LAYER_PROJECT") << static_cast<qint32>(m_layers.size());

    // Сохраняем каждый слой
    for (const auto& layer : m_layers) {
        stream << layer->name()
        << layer->isVisible()
        << static_cast<qreal>(layer->opacity());

        // Сохраняем изображение
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        layer->image().save(&buffer, "PNG");
        stream << imageData;
    }

    return true;
}

bool LayerManager::loadProject(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_15);

    // Проверяем заголовок
    QString header;
    qint32 layerCount;
    stream >> header >> layerCount;

    if (header != "LAYER_PROJECT" || layerCount < 0)
        return false;

    // Очищаем текущие слои
    m_layers.clear();
    m_activeLayer = nullptr;

    // Загружаем слои
    for (int i = 0; i < layerCount; ++i) {
        QString name;
        bool visible;
        qreal opacity;
        QByteArray imageData;

        stream >> name >> visible >> opacity >> imageData;

        if (stream.status() != QDataStream::Ok)
            return false;

        // Создаем изображение из данных
        QImage image;
        if (!image.loadFromData(imageData, "PNG"))
            continue;

        auto layer = std::make_unique<Layer>(image.size(), name);
        layer->setImage(image);
        layer->setVisible(visible);
        layer->setOpacity(static_cast<float>(opacity));

        addLayer(std::move(layer));
    }

    if (!m_layers.empty()) {
        setActiveLayer(0);
    }

    emit layersChanged();
    return true;
}
