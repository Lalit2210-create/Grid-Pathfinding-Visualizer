#pragma once
#include "grid/Cell.hpp"
#include "utils/Constants.hpp"
#include "StepResult.hpp"
#include "graph/Graph.hpp"
#include <queue>

struct AStarNodeDetails {
    float f = std::numeric_limits<float>::infinity();
    float g = std::numeric_limits<float>::infinity();
    float h = 0.0f;
    Pos parent{-1,-1};
};
class AStarRunner {
public:
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::vector<bool> closedList = std::vector(gridRows * gridCols,false);
    std::vector<AStarNodeDetails> nodeDetails{gridRows * gridCols };
    Graph* graph;
    Pos start, end;

    explicit AStarRunner(Graph *g);

    void initialize(Pos start, Pos end);

    StepResult step(std::vector<std::vector<Cell> > &grid);
};
