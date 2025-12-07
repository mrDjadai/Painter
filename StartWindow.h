#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>

class StartWindow : public QWidget
{
    Q_OBJECT
public:
    explicit StartWindow(QWidget* parent = nullptr);

signals:
    void openFileRequested(const QString& filename);
    void createNewCanvasRequested(int width, int height);

private slots:
    void onOpenFile();
    void onCreateCanvas();

private:
    QPushButton* m_openButton;
    QPushButton* m_createButton;
    QSpinBox* m_widthSpin;
    QSpinBox* m_heightSpin;
};
#endif // STARTWINDOW_H
