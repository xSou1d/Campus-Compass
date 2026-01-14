#include "Graph.h"

#include <queue>
#include <limits>
#include <unordered_set>
#include <functional>

void Graph::addEdge(int u, int v, int w) {
    adj_[u].emplace_back(v, w, false);
    adj_[v].emplace_back(u, w, false);
}

int Graph::findEdgeIndex(int u, int v) const {
    auto it = adj_.find(u);
    if (it == adj_.end()) return -1;
    const auto& edges = it->second;
    for (std::size_t i = 0; i < edges.size(); ++i) {
        if (edges[i].to == v) return static_cast<int>(i);
    }
    return -1;
}

void Graph::toggleEdge(int u, int v) {
    int idxUV = findEdgeIndex(u, v);
    int idxVU = findEdgeIndex(v, u);

    if (idxUV != -1) {
        adj_[u][idxUV].closed = !adj_[u][idxUV].closed;
    }
    if (idxVU != -1) {
        adj_[v][idxVU].closed = !adj_[v][idxVU].closed;
    }
}

std::string Graph::edgeStatus(int u, int v) const {
    int idxUV = findEdgeIndex(u, v);
    if (idxUV == -1) {
        return "DNE";
    }
    const Edge& e = adj_.at(u)[idxUV];
    return e.closed ? "closed" : "open";
}

bool Graph::isConnected(int start, int goal) const {
    if (start == goal) return true;

    std::unordered_set<int> visited;
    std::queue<int> q;

    if (adj_.find(start) == adj_.end() ||
        adj_.find(goal)  == adj_.end()) {
        return false;
    }

    visited.insert(start);
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        auto it = adj_.find(u);
        if (it == adj_.end()) continue;

        for (const auto& e : it->second) {
            if (e.closed) continue;
            int v = e.to;
            if (!visited.count(v)) {
                if (v == goal) return true;
                visited.insert(v);
                q.push(v);
            }
        }
    }
    return false;
}

void Graph::dijkstra(int source,
                     std::unordered_map<int, int>& dist,
                     std::unordered_map<int, int>& parent) const {
    const int INF = std::numeric_limits<int>::max();

    dist.clear();
    parent.clear();

    // Initialize distances to INF
    for (const auto& [u, edges] : adj_) {
        (void)edges;
        dist[u] = INF;
        parent[u] = -1;
    }

    if (adj_.find(source) == adj_.end()) {
        return;
    }

    dist[source] = 0;
    parent[source] = -1;

    using Node = std::pair<int, int>;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;

        auto it = adj_.find(u);
        if (it == adj_.end()) continue;

        for (const auto& e : it->second) {
            if (e.closed) continue; // skip closed edges
            int v = e.to;
            int w = e.time;
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
}

int Graph::mstCost(const std::vector<int>& vertices) const {
    if (vertices.empty()) return 0;

    std::unordered_set<int> vset(vertices.begin(), vertices.end());

    // Prim's algorithm
    std::unordered_set<int> inMST;

    // Start from first vertex
    int start = vertices[0];
    if (!vset.count(start)) return 0;

    using EdgeState = std::tuple<int,int,int>; // (weight, from, to)
    std::priority_queue<EdgeState,
                        std::vector<EdgeState>,
                        std::greater<EdgeState>> pq;

    auto addEdgesFrom = [&](int u) {
        auto it = adj_.find(u);
        if (it == adj_.end()) return;
        for (const auto& e : it->second) {
            if (e.closed) continue;
            int v = e.to;
            if (!vset.count(v)) continue;
            if (inMST.count(v)) continue;
            pq.emplace(e.time, u, v);
        }
    };

    int totalCost = 0;
    inMST.insert(start);
    addEdgesFrom(start);

    while (!pq.empty() && inMST.size() < vset.size()) {
        auto [w, from, to] = pq.top();
        pq.pop();
        (void)from;

        if (inMST.count(to)) continue;

        inMST.insert(to);
        totalCost += w;
        addEdgesFrom(to);
    }

    return totalCost;
}