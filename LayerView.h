#ifndef LAYERVIEW_H
#define LAYERVIEW_H

#include <QWidget>
#include <QPainter>
#include "LayerManager.h"

class LayerView : public QWidget
{
    Q_OBJECT

public:
    explicit LayerView(LayerManager* layerManager, QWidget* parent = nullptr);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    LayerManager* m_layerManager;
};

#endif // LAYERVIEW_H
