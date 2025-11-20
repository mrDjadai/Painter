#include "LayerWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <qpainter.h>

// Реализация кастомного QListWidget для Drag and Drop
LayerListWidget::LayerListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
}

void LayerListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QListWidget::mousePressEvent(event);
}

void LayerListWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QListWidgetItem* item = itemAt(m_dragStartPosition);
    if (!item) {
        return;
    }

    // Создаем drag
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;

    // Сохраняем индекс перемещаемого элемента
    int dragIndex = row(item);
    mimeData->setData("application/x-layer-index", QByteArray::number(dragIndex));

    drag->setMimeData(mimeData);

    // Создаем визуальное представление для перетаскивания
    QPixmap pixmap(150, 25);
    pixmap.fill(QColor(100, 100, 200, 200));

    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, item->text());
    painter.end();

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(10, 10));

    // Начинаем перетаскивание
    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    if (dropAction == Qt::MoveAction) {
        // Обработка завершена в dropEvent
    }
}

void LayerListWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-layer-index")) {
        event->acceptProposedAction();
    }
}

void LayerListWidget::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-layer-index")) {
        event->acceptProposedAction();

        // Подсвечиваем позицию, куда будет вставлен элемент
        QListWidgetItem* item = itemAt(event->pos());
        if (item) {
            setCurrentItem(item);
        }
    }
}

void LayerListWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-layer-index")) {
        int sourceIndex = event->mimeData()->data("application/x-layer-index").toInt();
        QListWidgetItem* targetItem = itemAt(event->pos());

        if (!targetItem) {
            event->ignore();
            return;
        }

        int targetIndex = row(targetItem);

        // Испускаем сигнал о перемещении
        emit currentRowChanged(targetIndex); // Для обновления выделения

        // Испускаем кастомный сигнал или обрабатываем через родителя
        LayerWidget* parentWidget = qobject_cast<LayerWidget*>(parent());
        if (parentWidget) {
            parentWidget->handleLayerMove(sourceIndex, targetIndex);
        }

        event->acceptProposedAction();
    }
}

LayerWidget::LayerWidget(LayerManager* layerManager, QWidget* parent)
    : QWidget(parent)
    , m_layerManager(layerManager)
{
    setupUI();
    setupConnections();

    if (m_layerManager) {
        connect(m_layerManager, &LayerManager::layersChanged,
                this, &LayerWidget::updateLayerList);
        connect(m_layerManager, &LayerManager::activeLayerChanged,
                this, &LayerWidget::updateLayerList);
    }
}

void LayerWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // Кнопки управления
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_addButton = new QToolButton();
    m_addButton->setText("+");
    m_addButton->setToolTip("Add New Layer");
    m_addButton->setFixedSize(30, 25);

    m_removeButton = new QToolButton();
    m_removeButton->setText("-");
    m_removeButton->setToolTip("Remove Selected Layer");
    m_removeButton->setFixedSize(30, 25);

    m_duplicateButton = new QToolButton();
    m_duplicateButton->setText("⧉");
    m_duplicateButton->setToolTip("Duplicate Selected Layer");
    m_duplicateButton->setFixedSize(30, 25);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_duplicateButton);
    buttonLayout->addStretch();

    // Список слоев
    m_layerList = new LayerListWidget();
    m_layerList->setAlternatingRowColors(true);
    m_layerList->setStyleSheet(
        "QListWidget { background-color: #f0f0f0; border: 1px solid #ccc; }"
        "QListWidget::item { border-bottom: 1px solid #ddd; padding: 2px; }"
        "QListWidget::item:selected { background-color: #d0e3ff; }"
        "QListWidget::item:hover { background-color: #e8f0ff; }"
        );

    // Слайдер прозрачности
    QHBoxLayout* opacityLayout = new QHBoxLayout();
    QLabel* opacityLabel = new QLabel("Opacity:");
    opacityLabel->setFixedWidth(50);

    opacityLayout->addWidget(opacityLabel);

    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    m_opacitySlider->setEnabled(false);
    opacityLayout->addWidget(m_opacitySlider);

    // Собираем layout
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_layerList, 1); // Растягиваем список
    mainLayout->addLayout(opacityLayout);
}

void LayerWidget::setupConnections()
{
    connect(m_addButton, &QToolButton::clicked,
            this, &LayerWidget::onAddLayerClicked);
    connect(m_removeButton, &QToolButton::clicked,
            this, &LayerWidget::onRemoveLayerClicked);
    connect(m_duplicateButton, &QToolButton::clicked,
            this, &LayerWidget::onDuplicateLayerClicked);
    connect(m_layerList, &QListWidget::currentRowChanged,
            this, &LayerWidget::onLayerSelectionChanged);
    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int value) {
        int listIndex = m_layerList->currentRow();
        if (listIndex >= 0 && m_layerManager) {
            int realIndex = getRealLayerIndex(listIndex);
            float opacity = value / 100.0f;
            Layer* layer = m_layerManager->layerAt(realIndex);
            if (layer) {
                layer->setOpacity(opacity);
                m_layerManager->layersChanged();
            }
        }
    });
}

void LayerWidget::handleLayerMove(int sourceListIndex, int targetListIndex)
{
    if (!m_layerManager) return;

    // Преобразуем индексы из списка в реальные индексы слоев
    int sourceRealIndex = getRealLayerIndex(sourceListIndex);
    int targetRealIndex = getRealLayerIndex(targetListIndex);

    qDebug() << "Moving layer from" << sourceRealIndex << "to" << targetRealIndex
             << "(list indices:" << sourceListIndex << "->" << targetListIndex << ")";

    // Перемещаем слой
    m_layerManager->moveLayer(sourceRealIndex, targetRealIndex);
}

int LayerWidget::getRealLayerIndex(int listIndex) const
{
    if (!m_layerManager || listIndex < 0) return -1;

    // В списке слои отображаются в обратном порядке (снизу вверх)
    // listIndex 0 = верхний элемент списка = последний слой в массиве
    int layerCount = m_layerManager->layerCount();
    return layerCount - 1 - listIndex;
}

int LayerWidget::getListIndexFromReal(int realIndex) const
{
    if (!m_layerManager || realIndex < 0) return -1;

    int layerCount = m_layerManager->layerCount();
    return layerCount - 1 - realIndex;
}

void LayerWidget::onAddLayerClicked()
{
    if (!m_layerManager) return;

    bool ok;
    QString name = QInputDialog::getText(this, "New Layer",
                                         "Layer name:", QLineEdit::Normal,
                                         "New Layer", &ok);
    if (!ok || name.isEmpty()) return;

    QSize size(800, 600);
    if (m_layerManager->layerCount() > 0) {
        const Layer* firstLayer = m_layerManager->layerAt(0);
        if (firstLayer) {
            size = firstLayer->image().size();
        }
    }

    m_layerManager->createNewLayer(size, name);
}

void LayerWidget::onRemoveLayerClicked()
{
    if (!m_layerManager) return;

    int listIndex = m_layerList->currentRow();
    if (listIndex < 0) return;

    int realIndex = getRealLayerIndex(listIndex);

    if (m_layerManager->layerCount() <= 1) {
        QMessageBox::warning(this, "Cannot Remove",
                             "Cannot remove the last layer");
        return;
    }

    QString layerName = m_layerManager->layerAt(realIndex)->name();

    int result = QMessageBox::question(this, "Remove Layer",
                                       QString("Remove layer '%1'?").arg(layerName),
                                       QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        m_layerManager->removeLayer(realIndex);
    }
}

void LayerWidget::onDuplicateLayerClicked()
{
    if (!m_layerManager) return;

    int listIndex = m_layerList->currentRow();
    if (listIndex < 0) return;

    int realIndex = getRealLayerIndex(listIndex);
    m_layerManager->duplicateLayer(realIndex);
}

void LayerWidget::onLayerSelectionChanged()
{
    int listIndex = m_layerList->currentRow();

    if (m_layerManager && listIndex >= 0) {
        int realIndex = getRealLayerIndex(listIndex);
        m_layerManager->setActiveLayer(realIndex);

        Layer* layer = m_layerManager->layerAt(realIndex);
        if (layer) {
            m_opacitySlider->setEnabled(true);
            m_opacitySlider->setValue(static_cast<int>(layer->opacity() * 100));
        }
    } else {
        m_opacitySlider->setEnabled(false);
    }
}

void LayerWidget::onLayerVisibilityChanged(int realIndex, bool visible)
{
    if (m_layerManager && realIndex >= 0 && realIndex < m_layerManager->layerCount()) {
        Layer* layer = m_layerManager->layerAt(realIndex);
        if (layer) {
            layer->setVisible(visible);
            m_layerManager->layersChanged();
        }
    }
}

void LayerWidget::updateLayerList()
{
    if (!m_layerManager) return;

    // Сохраняем текущее выделение
    int currentListRow = m_layerList->currentRow();

    // Блокируем сигналы чтобы избежать рекурсии
    m_layerList->blockSignals(true);
    m_layerList->clear();

    // Заполняем список слоев (снизу вверх - как в Photoshop)
    for (int i = m_layerManager->layerCount() - 1; i >= 0; --i) {
        const Layer* layer = m_layerManager->layerAt(i);
        if (!layer) continue;

        QListWidgetItem* item = new QListWidgetItem();

        // Создаем виджет для элемента списка
        QWidget* itemWidget = new QWidget();
        QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(4, 2, 4, 2);
        itemLayout->setSpacing(4);

        // Иконка перетаскивания (рука)
        QLabel* dragIcon = new QLabel("☰");
        dragIcon->setStyleSheet("color: #666; font-size: 12px;");
        dragIcon->setFixedSize(16, 16);
        dragIcon->setAlignment(Qt::AlignCenter);

        // Чекбокс видимости
        QCheckBox* visibilityCheck = new QCheckBox();
        visibilityCheck->setChecked(layer->isVisible());
        visibilityCheck->setFixedSize(20, 20);

        connect(visibilityCheck, &QCheckBox::toggled, this, [this, i](bool visible) {
            onLayerVisibilityChanged(i, visible);
        });

        // Название слоя
        QLabel* nameLabel = new QLabel(layer->name());
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        // Прозрачность
        QLabel* opacityLabel = new QLabel(QString("%1%").arg(int(layer->opacity() * 100)));
        opacityLabel->setStyleSheet("color: #666; font-size: 10px;");
        opacityLabel->setFixedWidth(30);

        itemLayout->addWidget(dragIcon);
        itemLayout->addWidget(visibilityCheck);
        itemLayout->addWidget(nameLabel, 1); // Растягиваем название
        itemLayout->addWidget(opacityLabel);
        itemLayout->addStretch();

        itemWidget->setLayout(itemLayout);

        // Устанавливаем размер
        item->setSizeHint(QSize(200, 30));

        // Включаем перетаскивание
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        m_layerList->addItem(item);
        m_layerList->setItemWidget(item, itemWidget);

        // Выделяем активный слой
        if (i == m_layerManager->activeLayerIndex()) {
            item->setSelected(true);
            // Подсветка будет через CSS
        }
    }

    // Восстанавливаем выделение
    if (currentListRow >= 0 && currentListRow < m_layerList->count()) {
        m_layerList->setCurrentRow(currentListRow);
    }

    m_layerList->blockSignals(false);

    // Обновляем состояние кнопок
    bool hasLayers = m_layerManager->layerCount() > 0;
    bool hasSelection = m_layerList->currentRow() >= 0;

    m_removeButton->setEnabled(hasLayers && hasSelection && m_layerManager->layerCount() > 1);
    m_duplicateButton->setEnabled(hasLayers && hasSelection);
}
