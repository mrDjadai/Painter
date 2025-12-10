#ifndef CONFIG_H
#define CONFIG_H

//----------------Стартовое меню-------------------------------
#define MIN_CANVAS_SIZE 1
#define MAX_CANVAS_SIZE 16000
#define DEFAULT_CANVAS_WIDTH 1280
#define DEFAULT_CANVAS_HEIGHT 720

//----------------Основной интерфейс---------------------------

// Параметры пропорций интерфейса
#define TOP_PANEL_HEIGHT_PERCENT 10    // Высота верхней панели (% от общей высоты)
#define CENTRAL_AREA_HEIGHT_PERCENT 90 // Высота центральной области (% от общей высоты)

#define CANVAS_WIDTH_PERCENT 85        // Ширина холста (% от ширины центральной области)
#define LAYERS_PANEL_WIDTH_PERCENT 15  // Ширина панели слоев (% от ширины центральной области)

#define SPLITTER_HANDLE_WIDTH 4        // Ширина разделителя
#define SPLITTER_COLLAPSIBLE false     // Можно ли сворачивать панели

// Минимальные размеры (в пикселях)
#define MIN_TOP_PANEL_HEIGHT 40
#define MIN_CANVAS_WIDTH 400
#define MIN_CANVAS_HEIGHT 300
#define MIN_LAYERS_PANEL_WIDTH 200

// Цвета интерфейса
#define TOP_PANEL_BACKGROUND "#2b2b2b"
#define TOP_PANEL_BORDER "#555"
#define TOP_PANEL_TEXT_COLOR "white"

#define LAYERS_PANEL_BACKGROUND "#f8f8f8"
#define LAYERS_PANEL_BORDER "#ccc"
#define LAYERS_PANEL_LEFT_BORDER "#ddd"
#define LAYERS_PANEL_TEXT_COLOR "#333"

#define CANVAS_BACKGROUND "white"
#define CANVAS_BORDER "#666"
#define SPLITTER_HANDLE_COLOR "#cccccc"


#endif // CONFIG_H
