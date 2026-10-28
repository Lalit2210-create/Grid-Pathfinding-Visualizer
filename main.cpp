/*
* Program made by Jagrit Thukral
*/
#include <iostream>

#include "grid/Cell.hpp"
#include "graph/Graph.hpp"
#include "graph/AStarRunner.hpp"
#include "graph/DijkstraRunner.hpp"
#include "utils/Constants.hpp"
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <utils/RoundedRectangle.hpp>
#include "ui/UIManager.hpp"

void drawWindow(sf::RenderWindow &window, tgui::Gui &gui, const std::vector<std::vector<Cell> > &grid,
                const RoundedRectangle &gridBackground) {
    window.clear(BACKGROUND_COLOR);
    window.draw(gridBackground);
    for (auto &row: grid)
        for (auto &cell: row)
            window.draw(cell.rect);
    gui.draw();
    window.display();
}

// calculate cell from mouse pos
Cell *posToCell(const sf::Vector2f &mousePos, std::vector<std::vector<Cell> > &grid) {
    const float mx = mousePos.x;
    const float my = mousePos.y;
    // if out of grid cords return
    if (mx < gridPos.x || mx > gridPos.x + gridSize.x) return nullptr;;
    if (my < gridPos.y || my > gridPos.y + gridSize.y) return nullptr;
    // check if mouse is within any cell's bound
    auto c = static_cast<int>((mx - gridPos.x) / (cellSize.x + cellPadding));
    auto r = static_cast<int>((my - gridPos.y) / (cellSize.y + cellPadding));
    if (isValidGridIndex({r, c})) {
        return &grid[r][c];
    }
    return nullptr;;
}

int main() {
    enum class SelectedAlgorithm {
        AStar,
        Dijkstra
    };

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Pathfinding Algorithm", sf::Style::None, sf::State::Windowed,
                            settings);
    // draw only as fast as screen can output, dependent on refresh rate
    window.setVerticalSyncEnabled(true);
    bool needsRedraw = true;
    float animationDelayMs = 15;

    // Grid initialization
    std::vector grid(gridRows, std::vector<Cell>(gridCols));
    std::cout << "Initializing grid\n";
    // draw grid background with padding
    RoundedRectangle gridBackground;
    // account for size increase by all side padding
    gridBackground.setSize(gridBackgroundSize);
    gridBackground.setRadius(borderRadius);
    gridBackground.setPosition(gridBackgroundPos);
    gridBackground.setFillColor(CARD_COLOR);
    gridBackground.setCornerPointCount(12);
    for (int row = 0; row < grid.size(); ++row) {
        for (int col = 0; col < grid[row].size(); ++col) {
            Cell &cell = grid[row][col];
            // set cell position relative to grid and its row/col number
            // also account for number of padding accumulated due to prev row and col
            // and add its own padding
            cell.index = {row, col};
            cell.rect.setPosition({
                gridPos.x + (static_cast<float>(col) * (cellSize.x + cellPadding)) + cellPadding,
                gridPos.y + (static_cast<float>(row) * (cellSize.y + cellPadding)) + cellPadding
            });
        }
    }
    // Initialize default start and end
    Pos start = {0, 0}, end = {gridRows - 1, gridCols - 1};
    grid[start.row][start.col].changeState(CellState::Start);
    grid[end.row][end.col].changeState(CellState::End);

    // Loading graph
    Graph graph;
    graph.loadGraphFromGrid(grid);

    AStarRunner astar(&graph);
    DijkstraRunner dijkstra(&graph);
    astar.initialize(start, end);
    dijkstra.initialize(start, end);
    SelectedAlgorithm selectedAlgorithm = SelectedAlgorithm::AStar;
    bool runningPathfinding = false;
    bool gridLocked = false;

    Cell *draggedSpecialCell = nullptr;
    CellState draggedState = {};

    // initialize and setup tgui widgets
    tgui::Gui gui{window};
    UIManager uiManager(gui);
    uiManager.initialize();
    uiManager.visualizeButton->onClick([&] {
        draggedSpecialCell = nullptr;
        runningPathfinding = true;
        uiManager.visualizeButton->setEnabled(false);
        uiManager.resetButton->setEnabled(false);
        uiManager.algorithmSelector->setEnabled(false);
        uiManager.pathLengthLabel->setText("--");
        needsRedraw = true;
    });
    uiManager.resetButton->onClick([&] {
        // reset grid
        for (auto &row: grid)
            for (auto &cell: row)
                if (cell.state != CellState::Start && cell.state != CellState::End)
                    cell.changeState(CellState::Empty);
        //reset graph completely
        graph.adjList.clear();
        graph.loadGraphFromGrid(grid);

        // reset AStarRunner
        astar.initialize(start, end);
        dijkstra.initialize(start, end);

        gridLocked = false;
        uiManager.visualizeButton->setEnabled(true);
        uiManager.algorithmSelector->setEnabled(true);
        uiManager.pathLengthLabel->setText("--");
        needsRedraw = true;
    });
    uiManager.algorithmSelector->onItemSelect([&](const tgui::String &item) {
        selectedAlgorithm = (item == "Dijkstra") ? SelectedAlgorithm::Dijkstra : SelectedAlgorithm::AStar;
        needsRedraw = true;
    });
    uiManager.speedLabel->setText("Animation Speed: " + std::to_string(static_cast<int>(animationDelayMs)) + "Ms");
    uiManager.animationSpeedSlider->setValue(animationDelayMs);
    uiManager.animationSpeedSlider->onValueChange([&](const float value) {
        animationDelayMs = value;
        uiManager.speedLabel->setText("Animation Speed: " + std::to_string(static_cast<int>(animationDelayMs)) + "Ms");
        needsRedraw = true;
    });
    uiManager.bindRedrawOnInteraction(uiManager.visualizeButton, &needsRedraw);
    uiManager.bindRedrawOnInteraction(uiManager.resetButton, &needsRedraw);
    uiManager.bindRedrawOnInteraction(uiManager.algorithmSelector, &needsRedraw);
    uiManager.bindRedrawOnInteraction(uiManager.animationSpeedSlider, &needsRedraw);
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>()) window.close();

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (const auto &mouseBtnPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtnPressed->button != sf::Mouse::Button::Left || runningPathfinding || gridLocked) continue;

                std::cout << "Left mouse btn pressed\n";
                // get cell clicked by moused
                Cell *cell = posToCell(sf::Vector2f(mousePos), grid);
                if (!cell) continue;
                std::cout << "Cell (" << cell->index.row << "," << cell->index.col << ") clicked\n";
                if (!isSpecialCell(cell->state)) {
                    if (cell->state == CellState::Wall) {
                        cell->changeState(CellState::Empty);
                        graph.addNode(cell->index, grid);
                    } else if (cell->state == CellState::Empty) {
                        cell->changeState(CellState::Wall);
                        graph.removeNode(cell->index);
                    }
                    needsRedraw = true;
                } else {
                    // start the start/end cell drag
                    draggedSpecialCell = cell;
                    draggedState = cell->state;
                }
            } else if (const auto mouseBtnReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseBtnReleased->button == sf::Mouse::Button::Left) {
                    if (draggedSpecialCell) draggedSpecialCell = nullptr;
                }
            }
            // handling start and end cell drag movement logic
            else if (event->is<sf::Event::MouseMoved>()) {
                if (runningPathfinding || gridLocked) {
                    draggedSpecialCell = nullptr;
                    continue;
                }

                if (!draggedSpecialCell) continue;


                // Check current hovered cell
                Cell *currentCell = posToCell(sf::Vector2f(mousePos), grid);
                if (!currentCell) continue;

                // don't do any changes if mouse hasn't moved enough to leave prev cell
                if (currentCell == draggedSpecialCell) continue;

                // only set the old cell as empty if current cell is valid
                // prevents erasing start/end cells
                draggedSpecialCell->changeState(CellState::Empty);

                // Prevent swapping Start and End directly
                if (draggedState == CellState::Start && currentCell->state == CellState::End) continue;
                if (draggedState == CellState::End && currentCell->state == CellState::Start) continue;

                // If dropped on a wall, remove wall
                if (currentCell->state == CellState::Wall)
                    graph.addNode(currentCell->index, grid);

                // Apply new state
                currentCell->changeState(draggedState);

                // update start and end accordingly

                if (draggedState == CellState::Start)
                    start = currentCell->index;
                else if (draggedState == CellState::End)
                    end = currentCell->index;

                // reinitialize astar runner to reflect start and end change, resetting lists
                astar.initialize(start, end);
                dijkstra.initialize(start, end);

                // Update dragged pointer this cell is now the draggable one
                draggedSpecialCell = currentCell;

                needsRedraw = true;
            }
        }

        if (runningPathfinding) {
            StepResult stepResult =
                (selectedAlgorithm == SelectedAlgorithm::Dijkstra) ? dijkstra.step(grid) : astar.step(grid);

            if (stepResult.done) {
                runningPathfinding = false;
                gridLocked = true;
                uiManager.pathLengthLabel->setText(std::to_string(stepResult.dist));
                uiManager.resetButton->setEnabled(true);
                needsRedraw = true;
            }
            if (stepResult.changed) needsRedraw = true;
            sf::sleep(sf::milliseconds(static_cast<int>(animationDelayMs)));
        }

        if (needsRedraw) {
            drawWindow(window, gui, grid, gridBackground);
            needsRedraw = false;
        }
    }
}
