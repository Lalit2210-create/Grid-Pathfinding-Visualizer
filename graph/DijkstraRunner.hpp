#pragma once
#include "Graph.hpp"
#include "StepResult.hpp"
#include "grid/Cell.hpp"
#include <queue>

struct DijkstraNodeDetails {
    float g = std::numeric_limits<float>::infinity();
    Pos parent{-1,-1};
};

class DijkstraRunner {
public:

    std::priority_queue<Node, std::vector<Node>, std::greater<>> openList;
    std::vector<bool> closedList = std::vector(gridRows * gridCols,false);
    std::vector<DijkstraNodeDetails> nodeDetails{gridRows * gridCols };
    Graph* graph;
    Pos start, end;

    explicit DijkstraRunner(Graph *g);

    void initialize(Pos start, Pos end);

    StepResult step(std::vector<std::vector<Cell> > &grid);
};
