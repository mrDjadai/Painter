#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <QObject>
#include <QSize>
#include <vector>
#include <memory>
#include "Layer.h"

class LayerManager : public QObject
{
    Q_OBJECT

public:
    explicit LayerManager(QObject* parent = nullptr);

    void addLayer(std::unique_ptr<Layer> layer);
    void removeLayer(int index);
    void moveLayer(int fromIndex, int toIndex);
    void duplicateLayer(int index);
    void insertLayer(int index, std::unique_ptr<Layer> layer);


    Layer* createNewLayer(const QSize& size, const QString& name = "New Layer");
    Layer* createBackgroundLayer(const QSize& size, Qt::GlobalColor color);

    Layer* layerAt(int index);
    const Layer* layerAt(int index) const;
    int layerCount() const { return m_layers.size(); }

    void setActiveLayer(int index);
    Layer* activeLayer() { return m_activeLayer; }
    const Layer* activeLayer() const { return m_activeLayer; }
    int activeLayerIndex() const;

    QImage compositeImage(const QSize& size) const;
    void renderLayers(QPainter& painter, const QRect& destRect) const;

    bool saveProject(const QString& filename) const;
    bool loadProject(const QString& filename);

    void ClearLayers();

signals:
    void layersChanged();
    void activeLayerChanged(int index);

private:
    std::vector<std::unique_ptr<Layer>> m_layers;
    Layer* m_activeLayer = nullptr;
    QSize m_canvasSize;
};

#endif // LAYERMANAGER_H
