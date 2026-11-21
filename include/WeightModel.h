#ifndef WEIGHT_MODEL_H
#define WEIGHT_MODEL_H

#include "Graph.h"
#include "ReverseIndex.h"

// Adjusts edge weights from congestion and learned blockage risk.
class WeightModel {
private:
    Graph& graph;
    const ReverseIndex& index;
    int congestionCost;    // seconds added per robot planning to use a corridor
    int riskScale;         // seconds added at risk = 1.0
    double alpha;          // learning rate of the moving average
    double decay;          // forgetting per tick

    void learn(int from, int to) {
        Edge* e = graph.findEdge(from, to);
        if (e != nullptr) e->risk = (1.0 - alpha) * e->risk + alpha;
    }

public:
    WeightModel(Graph& g, const ReverseIndex& idx)
        : graph(g), index(idx), congestionCost(5), riskScale(20), alpha(0.2), decay(0.98) {}

    // call when a corridor gets blocked: risk rises in both directions
    void recordBlock(int a, int b) {
        learn(a, b);
        learn(b, a);
    }

    // call once per tick: fade old risk, then recompute every live weight
    void tick() {
        for (int u = 0; u < graph.nodeCount(); u++) {
            DynamicArray<Edge>& edges = graph.neighbors(u);
            for (int i = 0; i < edges.size(); i++) {
                Edge& e = edges[i];
                e.risk *= decay;
                int robots = index.robotsOn(u, e.to).size();
                e.weight = e.baseWeight + congestionCost * robots + (int)(e.risk * riskScale);
            }
        }
    }

    double riskOf(int a, int b) const {
        const DynamicArray<Edge>& edges = graph.neighbors(a);
        for (int i = 0; i < edges.size(); i++) {
            if (edges[i].to == b) return edges[i].risk;
        }
        return 0.0;
    }
};

#endif