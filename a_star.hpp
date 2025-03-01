#ifndef A_STAR_HPP
#define A_STAR_HPP

#include <vector>
#include <queue>
#include <cmath>
#include <functional> // 添加此行
#include <algorithm>
#include <iostream>
#include <unordered_set>

struct AStarNode {
    int x, y;
    float f, g, h;
    AStarNode* parent;

    AStarNode(int x, int y, float g, float h, AStarNode* parent)
        : x(x), y(y), g(g), h(h), f(g + h), parent(parent) {}

    bool operator>(const AStarNode& other) const {
        return f > other.f;
    }
};

class AStar {
private:
    std::vector<std::vector<int>> map;
    std::pair<int, int> start, end;
    std::vector<std::pair<int, int>> path;

    // 自定义哈希函数
    struct PairHash {
        size_t operator()(const std::pair<int, int>& p) const {
            return static_cast<size_t>(p.first) << 32 | p.second;
        }
    };

public:
    AStar(std::vector<std::vector<int>> map, std::pair<int, int> start, std::pair<int, int> end)
        : map(map), start(start), end(end) {}

    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> current);
    std::vector<std::pair<int, int>> findPath(); 

private:
    // 曼哈顿距离（四方向移动）
    float heuristic(std::pair<int, int> a, std::pair<int, int> b) {
        return std::abs(a.first - b.first) + std::abs(a.second - b.second);
    }
};

#endif /* A_STAR_HPP */
