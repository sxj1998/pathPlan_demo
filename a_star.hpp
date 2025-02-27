#ifndef A_STAR_HPP
#define A_STAR_HPP

#include <vector>
#include <queue>
#include <cmath>
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

    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> current) {
        std::vector<std::pair<int, int>> neighbors;
        int x = current.first;
        int y = current.second;
        std::cout << "-------------------------------------------------" << std::endl; // 修正坐标
        std::cout << "坐标：" << x << ", " << y << std::endl; // 修正坐标
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;

                // 统一检查范围和障碍物
                if (nx >= 0 && nx < map[0].size() && 
                    ny >= 0 && ny < map.size()) {

                    // std::cout << "map: " << nx << ", " << ny << " : is "  <<  map[nx][ny] << std::endl; // 修正坐标
                    if (map[ny][nx] == 0) { // 可通行区域
                        neighbors.emplace_back(nx, ny);
                        // std::cout << "可通过：" << nx << ", " << ny << std::endl; // 修正坐标
                    } else if (map[ny][nx] == 1) { // 障碍物
                        std::cout << "障碍物：" << nx << ", " << ny << std::endl; // 修正坐标
                    }
                }
            }
        }
        return neighbors;
    }

    std::vector<std::pair<int, int>> findPath() {
        // 优先队列（按 f 值排序）
        std::priority_queue<AStarNode*, std::vector<AStarNode*>, 
                           std::function<bool(AStarNode*, AStarNode*)>> 
            openList([](AStarNode* a, AStarNode* b) { return a->f > b->f; });
        
        // 关闭列表（使用哈希集合）
        std::unordered_set<std::pair<int, int>, PairHash> closedList;

        // 初始化起点
        AStarNode* startNode = new AStarNode(
            start.first, start.second, 
            0, 
            heuristic(start, end), 
            nullptr
        );
        openList.push(startNode);

        while (!openList.empty()) {
            AStarNode* currentNode = openList.top();
            openList.pop();
            std::cout << "poped :" << currentNode->x << ", " << currentNode->y << std::endl; // 修正坐标
            // 到达终点
            if (currentNode->x == end.first && currentNode->y == end.second) {
                // 回溯路径
                AStarNode* pathNode = currentNode;
                while (pathNode != nullptr) {
                    path.emplace_back(pathNode->x, pathNode->y);
                    pathNode = pathNode->parent;
                }
                std::reverse(path.begin(), path.end());
                break;
            }

            // 跳过已处理节点
            if (closedList.find({currentNode->x, currentNode->y}) != closedList.end()) {
                continue;
            }
            closedList.insert({currentNode->x, currentNode->y});

            // 生成邻居
            auto neighbors = getNeighbors({currentNode->x, currentNode->y});

            for (const auto& neighbor : neighbors) {
                int nx = neighbor.first;
                int ny = neighbor.second;

                // 计算移动代价
                int dx = abs(nx - currentNode->x);
                int dy = abs(ny - currentNode->y);
                float moveCost = (dx + dy == 2) ? 2.0f : 1.0f; // 对角线√2
                float new_g = currentNode->g + moveCost;
                float new_h = heuristic(neighbor, end);
                float new_f = new_g + new_h;
                std::cout << "neighbor: (" << nx << ", " << ny << ") " << " F "<< new_f << " G "<< new_g <<" H "<< new_h <<std::endl; // 修正坐标
                // 创建新节点
                AStarNode* neighborNode = new AStarNode(nx, ny, new_g, new_h, currentNode);
                openList.push(neighborNode);
            }
        }

        // 内存清理（示例代码，实际需优化）
        while (!openList.empty()) {
            delete openList.top();
            openList.pop();
        }

        return path;
    }

private:
    // 曼哈顿距离（四方向移动）
    float heuristic(std::pair<int, int> a, std::pair<int, int> b) {
        return std::abs(a.first - b.first) + std::abs(a.second - b.second);
    }
};








// struct AStarNode {
//     int x, y;
//     float g, h, f;  // g: 从起点到当前节点的代价, h: 预估的从当前节点到终点的代价, f: g + h
//     AStarNode* parent;

//     AStarNode(int x, int y, float g, float h, AStarNode* parent)
//         : x(x), y(y), g(g), h(h), f(g + h), parent(parent) {}

//     bool operator>(const AStarNode& other) const {
//         return f > other.f;
//     }
// };

// class AStar {
// private:
//     std::vector<std::vector<int>> map; // 地图数据，0代表可通行，1代表障碍物
//     std::pair<int, int> start, end;
//     std::vector<std::pair<int, int>> path;

//     std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> current) {
//         std::vector<std::pair<int, int>> neighbors;
//         int x = current.first;
//         int y = current.second;

//         for (int dx = -1; dx <= 1; ++dx) {
//             for (int dy = -1; dy <= 1; ++dy) {
//                 if (dx == 0 && dy == 0) continue;
//                 int nx = x + dx, ny = y + dy;
//                 // if (nx >= 0 && nx < map.size() && ny >= 0 && ny < map[0].size() && map[ny][nx] == 0) {
//                 if (nx >= 0 && nx < map[0].size() && ny >= 0 && ny < map.size() && map[nx][ny] == 0) {
//                     neighbors.push_back({nx, ny});
//                 }
//             }
//         }
//         std::cout << "Neighbors: ";
//         for (const auto& point : neighbors) {
//             std::cout << "(" << point.first << ", " << point.second << ") ";
//         }
//         std::cout << std::endl;
//         return neighbors;
//     }
    
    

//     float calculateHeuristic(std::pair<int, int> a, std::pair<int, int> b) {
//         return std::abs(a.first - b.first) + std::abs(a.second - b.second); // 曼哈顿距离
//     }

// public:
//     AStar(std::vector<std::vector<int>> map, std::pair<int, int> start, std::pair<int, int> end) 
//         : map(map), start(start), end(end) {}

//     std::vector<std::pair<int, int>> findPath() {
//         std::priority_queue<AStarNode*, std::vector<AStarNode*>, std::greater<AStarNode*>> openList;
//         std::vector<std::vector<bool>> closedList(map.size(), std::vector<bool>(map[0].size(), false));

//         AStarNode* startNode = new AStarNode(start.first, start.second, 0, calculateHeuristic(start, end), nullptr);
//         openList.push(startNode);

//         while (!openList.empty()) {

//             AStarNode* currentNode = openList.top();
//             openList.pop();

//             int x = currentNode->x;
//             int y = currentNode->y;

//             // 到达终点
//             if (x == end.first && y == end.second) {
//                 AStarNode* pathNode = currentNode;
//                 while (pathNode != nullptr) {
//                     path.push_back({pathNode->x, pathNode->y});
//                     pathNode = pathNode->parent;
//                 }
//                 std::reverse(path.begin(), path.end());
//                 break;
//             }

//             std::cout << "\n--- Current Node: (" << x << ", " << y << ") ---\n";
//             std::cout << "g=" << currentNode->g << ", h=" << currentNode->h << ", f=" << currentNode->f << "\n";
//             closedList[y][x] = true;
//             for (auto& neighbor : getNeighbors({x, y})) {
//                 int nx = neighbor.first;
//                 int ny = neighbor.second;

//                 if (closedList[ny][nx]) continue;

//                 float g = currentNode->g + 1;
//                 float h = calculateHeuristic(neighbor, end);
//                                 // 输出邻居节点信息

//                 float f = g * h;
//                 std::cout << "  Neighbor: (" << nx << ", " << ny << ")\n";
//                 std::cout << "    g = "  << g << "\n";
//                 std::cout << "    h = " << h << " (Octile distance)\n";
//                 std::cout << "    f = " << g << " + " << h << " = " << f << "\n\n";

//                 AStarNode* neighborNode = new AStarNode(nx, ny, g, h, currentNode);

//                 openList.push(neighborNode);
//             }
//         }
//         return path;
//     }
// };



#endif /* A_STAR_HPP */
