#include "Graph.hpp"

#include "grid/Cell.hpp"


void Graph::loadGraphFromGrid(const std::vector<std::vector<Cell>> &grid) {
    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            // add valid neighbours of cell as nodes in graph
            for (auto [dr,dc]: directions) {
                int nr = row + dr;
                int nc = col + dc;
                if (isValidCell(nr, nc, grid)) {
                    addEdge({row, col}, {nr, nc});
                }
            }
        }
    }
}

void Graph::addEdge(const Pos& u, const Pos& v) {
    adjList[u].insert(v);
    adjList[v].insert(u);
}
void Graph::addNode(const Pos &u,  std::vector<std::vector<Cell>> &grid) {
    if (adjList.contains(u)) return;
    adjList[u] = {};
    // Connect to all valid neighbors
    for (const auto& dir : directions) {
        int nr = u.row + dir.row;
        int nc = u.col + dir.col;
        if (isValidCell(nr, nc, grid)) {
            Pos neighbor = {nr, nc};
            // Only connect to existing nodes in the graph
            if (adjList.contains(neighbor)) {
                addEdge(u, neighbor);
            }
        }
    }
}
// Remove a node entirely
void Graph::removeNode(const Pos& u) {
    if (!adjList.contains(u)) return;
    // remove node from all neighbour's adjList
    for (const auto& neighbor : adjList[u]) {
        adjList[neighbor].erase(u);
    }
    adjList.erase(u);
}

float Graph::heuristic(const Pos current, const Pos end) const {
        const auto dx = static_cast<float>(abs(current.row - end.row));
    const auto dy = static_cast<float>(abs(current.col - end.col));
    // Diagonal Distance (8 direction movement)
    return edgeLength * (dx + dy) + (diagonalEdgeLength - 2 * edgeLength) * std::min(dx, dy);
    // Manhattan  distance (4 direction movement)
    //return edgeLength * (dx + dy);
}
