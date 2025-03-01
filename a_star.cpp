
#include "a_star.hpp"

std::vector<std::pair<int, int>> AStar::findPath() 
{
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
            float moveCost = (dx + dy == 2) ? 3.0f : 2.0f; // 对角线√2
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


std::vector<std::pair<int, int>> AStar::getNeighbors(std::pair<int, int> current) 
{
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