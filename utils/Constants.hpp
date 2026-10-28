#pragma once
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

constexpr int gridCols = 40;
constexpr int gridRows = 20;
struct Pos {
    int row,col;
    bool operator==(const Pos & pos) const = default;
};
struct Node {
    float f;
    Pos pos;
    bool operator>(const Node& other) const {
        return f > other.f;
    }
};
constexpr std::array<Pos,8> directions { {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},
// diagonals
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
}};
struct PosHash {
    // overloading function call operator
    std::size_t operator()(const Pos& p) const noexcept {
        return p.row * gridCols + p.col;
    }
};


inline int idx(const int r, const int c, const int cols = gridCols) {
    assert(r >= 0 && c >= 0 && c < cols);
    return r * cols + c;
}
inline int idx(const Pos pos,const int cols = gridCols) {
    assert(pos.row >= 0 && pos.col >= 0 && pos.col < cols);
    return pos.row * cols + pos.col;
}
// ui related constants
constexpr float borderRadius = 16.0;
constexpr float gridPadding = 16.f;

constexpr sf::Vector2f gridBackgroundPos(40.f, 120.f);
constexpr sf::Vector2f gridBackgroundSize(1120.f, 820.f);

// Grid: the actual cell area inside the background
constexpr sf::Vector2f gridPos(
    gridBackgroundPos.x + gridPadding,
    gridBackgroundPos.y + gridPadding
);
constexpr sf::Vector2f gridSize(
    gridBackgroundSize.x - 2 * gridPadding,
    gridBackgroundSize.y - 2 * gridPadding
);

const tgui::Layout2d headerPos(gridBackgroundPos.x, 20.f);
const tgui::Layout2d headerSize("100% - 80", 80.f);

const tgui::Layout2d controlPanelPos(40,gridBackgroundPos.y+gridBackgroundSize.y + 20);
const tgui::Layout2d controlPanelSize(gridBackgroundSize.x,100);
const tgui::Layout2d btnSize(150,48);

const tgui::Layout2d speedPanelSize( "100%", "100%");

const tgui::Layout2d sidebarPos(gridBackgroundPos.x + gridBackgroundSize.x + 120, headerPos.y + headerSize.y + 20);
const tgui::Layout2d sidebarSize(420, gridBackgroundSize.y + controlPanelSize.y + 40);

constexpr float legendPanelHeight = 240;
constexpr float cellDistPanelHeight = 180;
constexpr float howItWorksPanelHeight = 220;
constexpr float resultsPanelHeight = 150;

// 2 border pixel adjustment + 1 padding
constexpr float cellPadding = 3.0f;
constexpr sf::Vector2f cellSize(
    (gridSize.x - (gridCols + 1) * cellPadding) / gridCols,
    (gridSize.y - (gridRows + 1) * cellPadding) / gridRows
);
// colors
constexpr sf::Color BACKGROUND_COLOR(15, 25, 35);
constexpr sf::Color ACCENT_COLOR(43, 108, 238);
constexpr sf::Color BTN_COLOR(40, 46, 57);
constexpr sf::Color CARD_COLOR(27, 36, 46);
constexpr sf::Color COLOR_START(34, 197, 94);
constexpr sf::Color COLOR_END(249, 115, 22);
constexpr sf::Color COLOR_WALL(0, 0, 0);
constexpr sf::Color COLOR_VISITED(176, 58, 61);
constexpr sf::Color COLOR_DISCOVERED(255, 220, 140);
constexpr sf::Color COLOR_PATH(59, 130, 246);
constexpr sf::Color COLOR_DEFAULT(28, 31, 39);

// general functions
inline bool isValidGridIndex(const Pos index) {
    const auto [row,col] = index;
    return row >= 0 && row < gridRows &&
           col >= 0 && col < gridCols;
}
