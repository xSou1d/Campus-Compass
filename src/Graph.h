#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class Graph {
public:
    struct Edge {
        int to;
        int time;
        bool closed;

        Edge(int t, int w, bool c = false) : to(t), time(w), closed(c) {}
    };

    // Add an undirected edge between u and v with weight w (time)
    void addEdge(int u, int v, int w);

    // Toggle an existing edge between u and v (open/closed)
    void toggleEdge(int u, int v);

    // "open", "closed", or "DNE"
    std::string edgeStatus(int u, int v) const;

    // Checks for path between start and goal using only open edges
    bool isConnected(int start, int goal) const;

    // Dijkstra: shortest paths from source using only open edges
    // dist[node] = min time from source
    // parent[node] = previous node on shortest path (or -1 for source)
    void dijkstra(int source,
                  std::unordered_map<int, int>& dist,
                  std::unordered_map<int, int>& parent) const;

    // Minimum spanning tree cost on subgraph induced by 'vertices'
    // Only open edges with both endpoints in 'vertices' are considered.
    // Assumes the subgraph is connected (as per project spec).
    int mstCost(const std::vector<int>& vertices) const;

private:
    // adjacency list: node id -> edges
    std::unordered_map<int, std::vector<Edge>> adj_;

    // helper: find edge index in adj_[u] that goes to v; returns -1 if not found
    int findEdgeIndex(int u, int v) const;
};