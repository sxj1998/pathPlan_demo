#ifndef A_STAR_HPP
#define A_STAR_HPP

#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

struct AStarNode {
    int x, y;
    float g, h, f;  // g: 从起点到当前节点的代价, h: 预估的从当前节点到终点的代价, f: g + h
    AStarNode* parent;

    AStarNode(int x, int y, float g, float h, AStarNode* parent)
        : x(x), y(y), g(g), h(h), f(g + h), parent(parent) {}

    bool operator>(const AStarNode& other) const {
        return f > other.f;
    }
};

class AStar {
private:
    std::vector<std::vector<int>> map; // 地图数据，0代表可通行，1代表障碍物
    std::pair<int, int> start, end;
    std::vector<std::pair<int, int>> path;

    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> current) {
        std::vector<std::pair<int, int>> neighbors;
        int x = current.first;
        int y = current.second;

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < map.size() && ny >= 0 && ny < map[0].size() && map[ny][nx] == 0) {
                    neighbors.push_back({nx, ny});
                }
            }
        }
        return neighbors;
    }

    float calculateHeuristic(std::pair<int, int> a, std::pair<int, int> b) {
        return std::abs(a.first - b.first) + std::abs(a.second - b.second); // 曼哈顿距离
    }

public:
    AStar(std::vector<std::vector<int>> map, std::pair<int, int> start, std::pair<int, int> end) 
        : map(map), start(start), end(end) {}

    std::vector<std::pair<int, int>> findPath() {
        std::priority_queue<AStarNode*, std::vector<AStarNode*>, std::greater<AStarNode*>> openList;
        std::vector<std::vector<bool>> closedList(map.size(), std::vector<bool>(map[0].size(), false));

        AStarNode* startNode = new AStarNode(start.first, start.second, 0, calculateHeuristic(start, end), nullptr);
        openList.push(startNode);

        while (!openList.empty()) {
            AStarNode* currentNode = openList.top();
            openList.pop();

            int x = currentNode->x;
            int y = currentNode->y;

            // 到达终点
            if (x == end.first && y == end.second) {
                AStarNode* pathNode = currentNode;
                while (pathNode != nullptr) {
                    path.push_back({pathNode->x, pathNode->y});
                    pathNode = pathNode->parent;
                }
                std::reverse(path.begin(), path.end());
                break;
            }

            closedList[y][x] = true;
            for (auto& neighbor : getNeighbors({x, y})) {
                int nx = neighbor.first;
                int ny = neighbor.second;

                if (closedList[ny][nx]) continue;

                float g = currentNode->g + 1;
                float h = calculateHeuristic(neighbor, end);
                AStarNode* neighborNode = new AStarNode(nx, ny, g, h, currentNode);

                openList.push(neighborNode);
            }
        }
        return path;
    }
};



#endif /* A_STAR_HPP */
