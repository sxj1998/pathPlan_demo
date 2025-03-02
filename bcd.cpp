#include "bcd.hpp"
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>  // 用于格式化输出


// std::vector<std::vector<RegionPoint>> BcdPlanner::BcdPlannerHandle(const std::vector<std::vector<int>>& map)
// {
//     int rows = map.size();
//     int cols = map[0].size();
//     int current_id = 1;
//     std::vector<std::vector<int>> result_ceil = {};
//     std::vector<std::pair<int, int>> segments = {};
//     std::vector<std::pair<int, int>> prev_column = {};
//     std::vector<std::pair<int, int>> new_column = {};
//     std::vector<std::vector<std::pair<int, int>>> free_segments = {{}};

//     if (map.empty()) {
//         std::cerr << "WARNING: Input map is empty." << std::endl;
//         return {};
//     }
//     // 对当前列进行区域分割
//     std::cout << "正在BCD处理 行数: " << rows << ", 列数: " << cols << std::endl;
//     for(int col = 0; col < cols; col++) {
//         // std::cout << "正在处理第 " << col << "列" << std::endl;
//         for(int row = 0; row < rows; row++) {
//             // std::cout << "正在处理第 " << row << "行" << std::endl;
//             if(map[row][col] == BCD_MAP_FREE){
//                 std::pair<int, int> point = {row, col};
//                 segments.push_back(point);
//             }else{
//                 if(!segments.empty()){
//                     free_segments.push_back(segments);
//                     segments.clear();
//                 }
//             }
//         }
//         if(!segments.empty()){
//             free_segments.push_back(segments);
//             segments.clear(); // 确保残留数据被清除
//         }

//         // 打印 free_segments 内容
//         std::cout << "当前列的分割结果 : " << std::endl;
//         for (size_t i = 0; i < free_segments.size(); ++i) {
//             for (const auto& point : free_segments[i]) {
//                 std::cout << point.first << ", " ;
//             }
//             std::cout << std::endl;
//         }

//         // 对当前一列进行比较
//         std::cout << "第" << col << "分割结果" << std::endl;
//         std::cout << "上一列自由区域范围" << std::endl;
//         for (const auto& col : prev_column) {
//             std::cout << "[" << col.first << ", " << col.second << "] ";
//         }
//         std::cout << std::endl;

//         bool matched = false;
//         // 与前一列进行配对
//         for (const auto& seg : free_segments) { // 直接遍历所有段
//             if (seg.empty()) continue;

//             auto min_it = std::min_element(seg.begin(), seg.end(),
//                 [](const auto& a, const auto& b) { return a.first < b.first; });
//             auto max_it = std::max_element(seg.begin(), seg.end(),
//                 [](const auto& a, const auto& b) { return a.first < b.first; });
//             int min_row = min_it->first;
//             int max_row = max_it->first;

//             new_column.emplace_back(min_row, max_row);
//             if(!prev_column.size()){
//                 std::cout << "上次未匹配到自由区域 !!!" << std::endl;
//                 continue;
//             }
//             for(const auto& prev_seg : prev_column){
//                 if( (std::abs(prev_seg.first - min_it) <= 2 ) && (std::abs(prev_seg.second - max_it) <= 2 ) ){
//                     matched = true;
//                 }
//             }

//             if( !matched ){

//             }
//         }
//         std::cout << "当前列自由区域范围" << std::endl;
//         for (const auto& col : new_column) {
//             std::cout << "[" << col.first << ", " << col.second << "] ";
//         }


//         prev_column = new_column;
//         new_column.clear();
//         free_segments.clear();
//         std::cout << std::endl;
//         std::cout << std::endl;
//     }
    




//     for (size_t i = 0; i < map.size(); ++i) {
//         for (size_t j = 0; j < map[i].size(); ++j) {
//             std::cout << map[i][j] << " ";
//         }
//         std::cout << std::endl;
//     }

//     // 与前一列进行匹配
//     std::vector<std::vector<RegionPoint>> result;
//     return result;
// }



// 辅助函数：打印列区域信息
void printColumnSegments(const std::vector<SegmentInfo>& segments, const std::string& prefix = "") {
    std::cout << prefix;
    for (const auto& seg : segments) {
        std::cout << "[" << seg.min_row << "-" << seg.max_row << "](id:" << seg.id << ") ";
    }
    if (segments.empty()) std::cout << "<空>";
    std::cout << std::endl;
}

std::vector<std::vector<RegionPoint>> BcdPlanner::BcdPlannerHandle(const std::vector<std::vector<int>>& map) {
    constexpr int COLUMN_PRINT_WIDTH = 3;  // 列号打印宽度
    
    std::cout << "\n===== 开始BCD处理 =====" << std::endl;
    std::cout << "地图尺寸: " << map.size() << "行 × " << (map.empty() ? 0 : map[0].size()) << "列\n";

    int rows = map.size();
    if (rows == 0) return {};
    int cols = map[0].size();

    // 初始化结果网格
    std::vector<std::vector<RegionPoint>> result(rows, std::vector<RegionPoint>(cols));
    for (int y = 0; y < cols; ++y) {
        for (int x = 0; x < rows; ++x) {
            result[x][y] = {x, y, 0};
        }
    }

    int current_id = 1;
    std::vector<SegmentInfo> prev_column;

    // 列处理主循环
    for (int col = 0; col < cols; ++col) {
        std::cout << "\n── 处理第 " << std::setw(COLUMN_PRINT_WIDTH) << col << " 列 ──" << std::endl;

        // 步骤1: 当前列自由区域分割
        std::vector<std::pair<int, int>> current_segments;
        int seg_start = -1;
        for (int row = 0; row < rows; ++row) {
            if (map[row][col] == BCD_MAP_FREE) {
                if (seg_start == -1) seg_start = row;
            } else {
                if (seg_start != -1) {
                    current_segments.emplace_back(seg_start, row - 1);
                    seg_start = -1;
                }
            }
        }
        if (seg_start != -1) {
            current_segments.emplace_back(seg_start, rows - 1);
        }

        // 打印当前列分割结果
        std::cout << "├─ 自由区域分割: ";
        if (current_segments.empty()) {
            std::cout << "<无自由区域>" << std::endl;
        } else {
            for (const auto& seg : current_segments) {
                std::cout << "[" << seg.first << "-" << seg.second << "] ";
            }
            std::cout << std::endl;
        }

        // 步骤2: 区域匹配
        std::vector<SegmentInfo> new_column;
        for (const auto& seg : current_segments) {
            const int min_row = seg.first;
            const int max_row = seg.second;
            int use_id = current_id;
            bool matched = false;

            std::cout << "├─ 处理区域 [" << min_row << "-" << max_row << "]" << std::endl;

            // 匹配前一列
            if (!prev_column.empty()) {
                std::cout << "│   ├─ 尝试匹配上一列区域: ";
                printColumnSegments(prev_column, "");
                
                for (const auto& prev_seg : prev_column) {
                    const bool row_match = (std::abs(prev_seg.min_row - min_row) <= 2) && 
                                          (std::abs(prev_seg.max_row - max_row) <= 2);
                    std::cout << "│   │   ├─ 对比区域 [" << prev_seg.min_row << "-" << prev_seg.max_row 
                              << "](id:" << prev_seg.id << "), 行距差: " 
                              << std::abs(prev_seg.min_row - min_row) << "/"
                              << std::abs(prev_seg.max_row - max_row)
                              << (row_match ? " → 匹配成功" : " → 不匹配") 
                              << std::endl;
                    
                    if (row_match) {
                        use_id = prev_seg.id;
                        matched = true;
                        break;
                    }
                }
            } else {
                std::cout << "│   ├─ 前一列为空，无需匹配" << std::endl;
            }

            // ID分配逻辑
            if (!matched) {
                std::cout << "│   ├─ 创建新区域 ID: " << current_id << std::endl;
                ++current_id;
            } else {
                std::cout << "│   ├─ 沿用区域 ID: " << use_id << std::endl;
            }

            // 填充当前区域
            for (int row = min_row; row <= max_row; ++row) {
                result[row][col].id = use_id;
            }
            new_column.push_back({min_row, max_row, use_id});
        }

        // 更新列信息
        std::cout << "└─ 当前列最终区域: ";
        printColumnSegments(new_column);
        prev_column = new_column;
    }


    std::cout << "\n===== 最终区域分布（按行打印） =====" << std::endl;
    constexpr int ROW_PRINT_WIDTH = 2;  // 行号打印宽度
    constexpr int CELL_WIDTH = 3;       // 单元格内容宽度

    // 打印列号标题
    std::cout << "     ";  // 对齐行号列
    for (int y = 0; y < cols; ++y) {
        std::cout << std::setw(CELL_WIDTH) << y << " ";
    }
    std::cout << "\n     ";  // 对齐分隔线
    for (int y = 0; y < cols; ++y) {
        std::cout << "----";
    }
    std::cout << std::endl;

    // 打印最终结果
    std::cout << "\n===== 最终区域分布 =====" << std::endl;
    // 按行打印内容
    for (int x = 0; x < rows; ++x) {
        std::cout << "行" << std::setw(ROW_PRINT_WIDTH) << x << " | ";
        for (int y = 0; y < cols; ++y) {
            std::cout << std::setw(CELL_WIDTH) << result[x][y].id << " ";
        }
        std::cout << std::endl;
    }

    return result;
}