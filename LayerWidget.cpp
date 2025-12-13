#include "LayerWidget.h"
#include "Commands.h"
#include <QLabel>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QMouseEvent>
#include <QCheckBox>
#include <QPainter>
#include "Config.h"

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


    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;

    int dragIndex = row(item);
    mimeData->setData("application/x-layer-index", QByteArray::number(dragIndex));

    drag->setMimeData(mimeData);

    QPixmap pixmap(150, 25);
    pixmap.fill(QColor(100, 100, 200, 200));

    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, item->text());
    painter.end();

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(10, 10));

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
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

        emit layerMoved(sourceIndex, targetIndex);

        event->acceptProposedAction();
    }
}

LayerWidget::LayerWidget(LayerManager* layerManager, CommandManager* comManager, QWidget* parent)
    : QWidget(parent)
    , m_layerManager(layerManager)
    , m_commandManager(comManager)
    , m_layerList(nullptr)
    , m_addButton(nullptr)
    , m_removeButton(nullptr)
    , m_duplicateButton(nullptr)
    , m_opacitySlider(nullptr)
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
    mainLayout->setContentsMargins(LAYER_WIDGET_MARGIN, LAYER_WIDGET_MARGIN,
                                   LAYER_WIDGET_MARGIN, LAYER_WIDGET_MARGIN);
    mainLayout->setSpacing(LAYER_WIDGET_SPACING);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_addButton = new QToolButton();
    m_addButton->setText(LAYER_BUTTON_ADD_TEXT);
    m_addButton->setToolTip(LAYER_BUTTON_TOOLTIP_ADD);
    m_addButton->setFixedSize(LAYER_BUTTON_WIDTH, LAYER_BUTTON_HEIGHT);

    m_removeButton = new QToolButton();
    m_removeButton->setText(LAYER_BUTTON_REMOVE_TEXT);
    m_removeButton->setToolTip(LAYER_BUTTON_TOOLTIP_REMOVE);
    m_removeButton->setFixedSize(LAYER_BUTTON_WIDTH, LAYER_BUTTON_HEIGHT);

    m_duplicateButton = new QToolButton();
    m_duplicateButton->setText(LAYER_BUTTON_DUPLICATE_TEXT);
    m_duplicateButton->setToolTip(LAYER_BUTTON_TOOLTIP_DUPLICATE);
    m_duplicateButton->setFixedSize(LAYER_BUTTON_WIDTH, LAYER_BUTTON_HEIGHT);

    m_renameButton = new QToolButton();
    m_renameButton->setText(LAYER_BUTTON_RENAME_TEXT);
    m_renameButton->setToolTip(LAYER_BUTTON_TOOLTIP_RENAME);
    m_renameButton->setFixedSize(LAYER_BUTTON_WIDTH, LAYER_BUTTON_HEIGHT);

    m_mergeButton = new QToolButton();
    m_mergeButton->setText(LAYER_BUTTON_MERGE_TEXT);
    m_mergeButton->setToolTip(LAYER_BUTTON_TOOLTIP_MERGE);
    m_mergeButton->setFixedSize(LAYER_BUTTON_WIDTH, LAYER_BUTTON_HEIGHT);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_duplicateButton);
    buttonLayout->addWidget(m_renameButton);
    buttonLayout->addWidget(m_mergeButton);
    buttonLayout->addStretch();

    m_layerList = new LayerListWidget();
    m_layerList->setAlternatingRowColors(true);
    m_layerList->setStyleSheet(QString(
                                   "QListWidget { background-color: %1; border: 1px solid %2; }"
                                   "QListWidget::item { border-bottom: 1px solid %3; padding: 2px; }"
                                   "QListWidget::item:selected { background-color: %4; }"
                                   "QListWidget::item:hover { background-color: %5; }"
                                   )
                                   .arg(LAYER_LIST_BG_COLOR)
                                   .arg(LAYER_LIST_BORDER_COLOR)
                                   .arg(LAYER_LIST_ITEM_BORDER_BOTTOM)
                                   .arg(LAYER_LIST_ITEM_SELECTED_BG)
                                   .arg(LAYER_LIST_ITEM_HOVER_BG)
                               );

    QHBoxLayout* opacityLayout = new QHBoxLayout();
    QLabel* opacityLabel = new QLabel("Прозрачность:");
    opacityLabel->setFixedWidth(LAYER_OPACITY_LABEL_WIDTH);

    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(LAYER_OPACITY_SLIDER_MIN, LAYER_OPACITY_SLIDER_MAX);
    m_opacitySlider->setValue(LAYER_OPACITY_SLIDER_DEFAULT);
    m_opacitySlider->setEnabled(false);

    opacityLayout->addWidget(opacityLabel);
    opacityLayout->addWidget(m_opacitySlider);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_layerList, 1);
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
    connect(m_renameButton, &QToolButton::clicked,
            this, &LayerWidget::onRenameLayerClicked);

    connect(m_mergeButton, &QToolButton::clicked,
            this, &LayerWidget::onMergeWithNextClicked);


    connect(m_layerList, &QListWidget::currentRowChanged,
            this, &LayerWidget::onLayerSelectionChanged);
    connect(m_layerList, &LayerListWidget::layerMoved,
            this, &LayerWidget::onLayerMoved);

    connect(m_opacitySlider, &QSlider::sliderPressed,
            this, &LayerWidget::onOpacitySliderPressed);
    connect(m_opacitySlider, &QSlider::valueChanged,
            this, &LayerWidget::onOpacitySliderValueChanged);
    connect(m_opacitySlider, &QSlider::sliderReleased,
            this, &LayerWidget::onOpacitySliderReleased);

    connect(m_layerManager, &LayerManager::layersChanged,
            this, &LayerWidget::updateOpacitySlider);

}

void LayerWidget::onOpacitySliderValueChanged(int value)
{
    int listIndex = m_layerList->currentRow();
    if (listIndex < 0 || !m_layerManager) return;

    int realIndex = getRealLayerIndex(listIndex);
    Layer* layer = m_layerManager->layerAt(realIndex);
    if (!layer) return;

    layer->setOpacity(value / 100.0f);
    m_layerManager->layersChanged();
    updateLayerList();
}

void LayerWidget::onOpacitySliderPressed()
{
    int listIndex = m_layerList->currentRow();
    if (listIndex < 0 || !m_layerManager) return;

    int realIndex = getRealLayerIndex(listIndex);
    Layer* layer = m_layerManager->layerAt(realIndex);
    if (!layer) return;

    m_startOpacity = layer->opacity();
}

void LayerWidget::onOpacitySliderReleased()
{
    int listIndex = m_layerList->currentRow();
    if (listIndex < 0 || !m_layerManager || !m_commandManager) return;

    int realIndex = getRealLayerIndex(listIndex);
    Layer* layer = m_layerManager->layerAt(realIndex);
    if (!layer) return;

    float newOpacity = layer->opacity();


    if (qAbs(newOpacity - m_startOpacity) > 0.001f)
    {
        ChangeLayerOpacityCommand* command =
            new ChangeLayerOpacityCommand(m_layerManager, realIndex,m_startOpacity, newOpacity);
        m_commandManager->ExecuteCommand(command);
    }
}

void LayerWidget::updateOpacitySlider()
{
    int listIndex = m_layerList->currentRow();
    if (listIndex < 0 || !m_layerManager) return;

    int realIndex = getRealLayerIndex(listIndex);
    Layer* layer = m_layerManager->layerAt(realIndex);
    if (!layer) return;

    m_opacitySlider->blockSignals(true);
    m_opacitySlider->setValue(static_cast<int>(layer->opacity() * 100));
    m_opacitySlider->blockSignals(false);
}

void LayerWidget::onAddLayerClicked()
{
    if (!m_layerManager || !m_commandManager) return;

    bool ok;
    QString name = QInputDialog::getText(this, "Новый слой",
                                         "Имя слоя:", QLineEdit::Normal,
                                         "Новый слой", &ok);
    if (!ok || name.isEmpty()) return;

    QSize size(800, 600);
    if (m_layerManager->layerCount() > 0) {
        const Layer* firstLayer = m_layerManager->layerAt(0);
        if (firstLayer) {
            size = firstLayer->image().size();
        }
    }

    AddLayerCommand* command = new AddLayerCommand(m_layerManager, size, name);
    m_commandManager->ExecuteCommand(command);
}

void LayerWidget::onRemoveLayerClicked()
{
    if (!m_layerManager || !m_commandManager) return;

    int listIndex = m_layerList->currentRow();
    if (listIndex < 0) return;

    int realIndex = getRealLayerIndex(listIndex);

    if (m_layerManager->layerCount() <= 1)
    {
        return;
    }

    QString layerName = m_layerManager->layerAt(realIndex)->name();

    DeleteLayerCommand* command = new DeleteLayerCommand(m_layerManager, realIndex);
    m_commandManager->ExecuteCommand(command);
}

void LayerWidget::onDuplicateLayerClicked()
{
    if (!m_layerManager || !m_commandManager) return;

    int listIndex = m_layerList->currentRow();
    if (listIndex < 0) return;

    int realIndex = getRealLayerIndex(listIndex);

    DuplicateLayerCommand* command = new DuplicateLayerCommand(m_layerManager, realIndex);
    m_commandManager->ExecuteCommand(command);
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
    if (!m_layerManager || !m_commandManager) return;
    Layer* layer = m_layerManager->layerAt(realIndex);
    if (layer && layer->isVisible() != visible) {
        ToggleLayerVisibilityCommand* command = new ToggleLayerVisibilityCommand(m_layerManager, realIndex);
        m_commandManager->ExecuteCommand(command);
    }
}

void LayerWidget::onLayerMoved(int fromListIndex, int toListIndex)
{
    if (!m_layerManager || !m_commandManager) return;

    int fromRealIndex = getRealLayerIndex(fromListIndex);
    int toRealIndex = getRealLayerIndex(toListIndex);

    MoveLayerCommand* command = new MoveLayerCommand(m_layerManager, fromRealIndex, toRealIndex);
    m_commandManager->ExecuteCommand(command);
}

void LayerWidget::updateLayerList()
{
    if (!m_layerManager) return;

    int currentListRow = m_layerList->currentRow();

    m_layerList->blockSignals(true);
    m_layerList->clear();

    for (int i = m_layerManager->layerCount() - 1; i >= 0; --i) {
        const Layer* layer = m_layerManager->layerAt(i);
        if (!layer) continue;

        QListWidgetItem* item = new QListWidgetItem();

        QWidget* itemWidget = new QWidget();
        QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(4, 2, 4, 2);
        itemLayout->setSpacing(4);

        QLabel* dragIcon = new QLabel("☰");
        dragIcon->setStyleSheet("color: #666; font-size: 12px;");
        dragIcon->setFixedSize(16, 16);
        dragIcon->setAlignment(Qt::AlignCenter);

        QCheckBox* visibilityCheck = new QCheckBox();
        visibilityCheck->setChecked(layer->isVisible());
        visibilityCheck->setFixedSize(20, 20);

        connect(visibilityCheck, &QCheckBox::toggled, this, [this, i](bool visible) {
            onLayerVisibilityChanged(i, visible);
        });

        QLabel* nameLabel = new QLabel(layer->name());
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel* opacityLabel = new QLabel(QString("%1%").arg(int(layer->opacity() * 100)));
        opacityLabel->setStyleSheet("color: #666; font-size: 10px;");
        opacityLabel->setFixedWidth(30);

        itemLayout->addWidget(dragIcon);
        itemLayout->addWidget(visibilityCheck);
        itemLayout->addWidget(nameLabel, 1);
        itemLayout->addWidget(opacityLabel);
        itemLayout->addStretch();

        itemWidget->setLayout(itemLayout);

        item->setSizeHint(QSize(200, 30));

        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        m_layerList->addItem(item);
        m_layerList->setItemWidget(item, itemWidget);

        if (i == m_layerManager->activeLayerIndex()) {
            item->setSelected(true);
        }
    }

    if (currentListRow >= 0 && currentListRow < m_layerList->count()) {
        m_layerList->setCurrentRow(currentListRow);
    }

    SetRow(getRealLayerIndex(m_layerManager->activeLayerIndex()));
    m_layerList->blockSignals(false);

    bool hasLayers = m_layerManager->layerCount() > 0;
    bool hasSelection = m_layerList->currentRow() >= 0;

    m_removeButton->setEnabled(hasLayers && hasSelection && m_layerManager->layerCount() > 1);
    m_duplicateButton->setEnabled(hasLayers && hasSelection);

    bool canRename = hasSelection;
    bool canMerge = hasSelection && m_layerManager->layerCount() > 1 &&
                    getRealLayerIndex(m_layerList->currentRow()) > 0;

    m_renameButton->setEnabled(canRename);
    m_mergeButton->setEnabled(canMerge);
    m_opacitySlider->setEnabled(hasSelection);
}

int LayerWidget::getRealLayerIndex(int listIndex) const
{
    if (!m_layerManager || listIndex < 0) return -1;

    int layerCount = m_layerManager->layerCount();
    return layerCount - 1 - listIndex;
}

int LayerWidget::getListIndexFromReal(int realIndex) const
{
    if (!m_layerManager || realIndex < 0) return -1;

    int layerCount = m_layerManager->layerCount();
    return layerCount - 1 - realIndex;
}

void LayerWidget :: SetRow(int row)
{
    if(m_layerList->currentRow() != row)
    {
        m_layerList->setCurrentRow(row);
    }
}

void LayerWidget::onRenameLayerClicked()
{
    if (!m_layerManager || !m_commandManager) return;

    int listIndex = m_layerList->currentRow();
    if (listIndex < 0) return;

    int realIndex = getRealLayerIndex(listIndex);
    Layer* layer = m_layerManager->layerAt(realIndex);
    if (!layer) return;

    bool ok;
    QString newName = QInputDialog::getText(
        this, "Переименовать слой",
        "Новое имя:",
        QLineEdit::Normal,
        layer->name(),
        &ok
        );

    if (!ok || newName.isEmpty() || newName == layer->name())
        return;

    RenameLayerCommand* cmd =
        new RenameLayerCommand(m_layerManager, realIndex, layer->name(), newName);
    m_commandManager->ExecuteCommand(cmd);
}

void LayerWidget::onMergeWithNextClicked()
{
    if (!m_layerManager || !m_commandManager) return;

    int listIndex = m_layerList->currentRow();
    if (listIndex < 0) return;

    int realIndex = getRealLayerIndex(listIndex);

    if (realIndex <= 0) {
        QMessageBox::warning(this, "Объединение",
                             "Нельзя объединить: выбранный слой самый нижний.");
        return;
    }

    MergeLayerWithNextCommand* cmd =
        new MergeLayerWithNextCommand(m_layerManager, realIndex);
    m_commandManager->ExecuteCommand(cmd);
}
