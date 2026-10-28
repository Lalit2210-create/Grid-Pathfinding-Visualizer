#include "AStarRunner.hpp"
#include "iostream"

AStarRunner::AStarRunner(Graph *g) : graph(g) {
}

void AStarRunner::initialize(Pos s, Pos e) {
    start = s;
    end = e;
    openList = {};
    closedList.assign(gridRows * gridCols, false);
    std::ranges::fill(nodeDetails, AStarNodeDetails{});
    openList.emplace(0.0f,start);
    const float startH = graph->heuristic(start,end);
    nodeDetails[idx(start)] = {startH,0,startH,start};
}

// do a* incrementally so we don't freeze our event loop
StepResult AStarRunner::step(std::vector<std::vector<Cell> > &grid) {
    StepResult result{false, false, false};

    if (openList.empty()) {
        result.done = true;
        result.pathFound = false;
        return result;
    }
    // track if made an ui update
    bool changed = false;
    // Get lowest f node
    const Node current = openList.top();
    openList.pop();
    Pos currentPos = current.pos;
    // mark current node in closed list
    closedList[idx(currentPos)] = true;
    // mark as visited unless it's start/end
    if (!isSpecialCell(grid[currentPos.row][currentPos.col].state)) {
        grid[currentPos.row][currentPos.col].changeState(CellState::Visited);
        changed = true;
    }
    // end reached
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
        result.dist = nodeDetails[idx(end.row,end.col)].g;
        result.done = true;
        result.pathFound = true;
        result.changed = changed;
        return result; // finished
    }

    // expand neighbours/successors
    for (const Pos &successor: graph->adjList[currentPos]) {
        // if already in closed list, ignore
        if (closedList[idx(successor)]) continue;
        const float moveCost = (abs(successor.row - currentPos.row) +
                                abs(successor.col - currentPos.col) == 2)
                                   ? graph->diagonalEdgeLength
                                   : graph->edgeLength;

        const float gNew = nodeDetails[idx(currentPos)].g + moveCost;
        const float hNew = graph->heuristic(successor, end);
        const float fNew = gNew + hNew;
        // if node doesn't exist in open list add or if there is a better path update.
        // if node doesn't exist, its f will be infinity so current f will be greater than fNew
        if (fNew < nodeDetails[idx(successor)].f) {
            nodeDetails[idx(successor)] = {fNew, gNew, hNew, currentPos};
            openList.emplace(fNew, successor);
            if (grid[successor.row][successor.col].state == CellState::Empty) {
                grid[successor.row][successor.col].changeState(CellState::Discovered);
                changed = true;
            }
        }
    }


    result.changed = changed;
    result.done = false;
    result.pathFound = false;
    return result; // still running
}
