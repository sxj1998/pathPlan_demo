#ifndef BCD_HPP
#define BCD_HPP

#include <iostream>
#include <vector>

typedef struct {
    int x;
    int y;
    int id;
}RegionPoint;

struct SegmentInfo {
    int min_row;
    int max_row;
    int id;
};

class BcdPlanner {
private:
    static constexpr int BCD_MAP_FREE = 0;  // 使用constexpr代替宏定义
    static constexpr int BCD_MAP_OCCUPIED = 1;
    std::vector<std::vector<RegionPoint>> RegionMap;
public:
    BcdPlanner() {};
    std::vector<std::vector<RegionPoint>> BcdPlannerHandle(const std::vector<std::vector<int>>& map);
};

#endif /* BCD_HPP */
