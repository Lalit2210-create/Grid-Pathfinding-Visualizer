#pragma once
#include <unordered_map>
#include <unordered_set>
#include "grid/Cell.hpp"
#include "utils/Constants.hpp"


class Graph {
public:
    std::unordered_map<Pos, std::unordered_set<Pos, PosHash>, PosHash> adjList;
    float edgeLength = 1.0f;
    // under root 2
    float diagonalEdgeLength = 1.41421356f;
    void loadGraphFromGrid(const std::vector<std::vector<Cell>> &grid);
    void addEdge(const Pos& u, const Pos& v);
    void addNode(const Pos &u, std::vector<std::vector<Cell>> &grid);
    void removeNode(const Pos& u);
    [[nodiscard]] float heuristic(Pos current, Pos end) const;
};
