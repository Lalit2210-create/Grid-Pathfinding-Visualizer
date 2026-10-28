#include "DijkstraRunner.hpp"
#include <iostream>

DijkstraRunner::DijkstraRunner(Graph *g) : graph(g) {
}

void DijkstraRunner::initialize(Pos s, Pos e) {
    start = s;
    end = e;
    openList = {};
    closedList.assign(gridRows * gridCols, false);
    std::ranges::fill(nodeDetails, DijkstraNodeDetails{});
    openList.emplace(0.0f, start);
    nodeDetails[idx(start)] = {0.0f, start};
}

// do Dijkstra incrementally so the event loop remains responsive
StepResult DijkstraRunner::step(std::vector<std::vector<Cell> > &grid) {
    StepResult result{false, false, false};

    if (openList.empty()) {
        result.done = true;
        result.pathFound = false;
        return result;
    }
    bool changed = false;

    // get shortest dist node
    const Node current = openList.top();
    // remove current from openList
    openList.pop();
    const Pos currentPos = current.pos;
    if (closedList[idx(currentPos)]) return result;
    // mark current node in closedList
    closedList[idx(currentPos)] = true;

    // mark as visited unless it's start/end
    if (!isSpecialCell(grid[currentPos.row][currentPos.col].state)) {
        grid[currentPos.row][currentPos.col].changeState(CellState::Visited);
        changed = true;
    }

    // end reached: backtrack using parents to paint the shortest path
    if (currentPos == end) {
        std::cout << "Path found!\n";
        Pos trace = end;
        while (trace != start) {
            if (!isSpecialCell(grid[trace.row][trace.col].state)) {
                grid[trace.row][trace.col].changeState(CellState::Path);
                changed = true;
            }
            trace = nodeDetails[idx(trace)].parent;
        }
        result.dist = nodeDetails[idx(end)].g;
        result.done = true;
        result.pathFound = true;
        result.changed = changed;
        return result;
    }

    for (const Pos &successor: graph->adjList[currentPos]) {
        // if successor is in closedList ski[
        if (closedList[idx(successor)]) continue;
        const float moveCost = (abs(successor.row - currentPos.row) +
                                abs(successor.col - currentPos.col) == 2)
                                   ? graph->diagonalEdgeLength
                                   : graph->edgeLength;

        const float gNew = nodeDetails[idx(currentPos)].g + moveCost;
        // if node doesn't exist in open list add or if there is a better path update.
        // if node doesn't exist, its g will be infinity so current g will be greater than fNew
        if (gNew < nodeDetails[idx(successor)].g) {
            nodeDetails[idx(successor)] = {gNew, currentPos};
            openList.emplace(gNew, successor);

            if (grid[successor.row][successor.col].state == CellState::Empty) {
                grid[successor.row][successor.col].changeState(CellState::Discovered);
                changed = true;
            }
        }
    }

    result.changed = changed;
    result.done = false;
    result.pathFound = false;
    return result;
}
