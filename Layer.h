#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QString>
#include <QRect>

class Layer
{
public:
    Layer(const QSize& size, const QString& name = "Layer");
    ~Layer() = default;

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }
    float opacity() const { return m_opacity; }

    void setName(const QString& name) { m_name = name; }
    QString name() const { return m_name; }

    QImage& image() { return m_image; }
    const QImage& image() const { return m_image; }

    void setImage(const QImage& image) { m_image = image; }

    void paint(QPainter& painter, const QRect& destRect);

private:
    QImage m_image;
    QString m_name;
    bool m_visible = true;
    float m_opacity = 1.0f;
};

#endif // LAYER_H
