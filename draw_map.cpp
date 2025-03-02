#include "draw_map.hpp"
#include "a_star.hpp"
#include "bcd.hpp"



void DrawMap::ClearMapInit(int width, int height)
{
    gray_map = Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
    drawing_flag = false;
    erasing_flag = false;
 
}

void DrawMap::mouseHandler(int event, int x, int y, int flags) {
    if (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN) {
        // 清除区域显示相关状态
        if (showRegionWindow) {
            destroyWindow("BCD Regions");
            showRegionWindow = false;
            regionMap.clear();
        }
    }

    if (event == EVENT_LBUTTONDOWN) {
        drawing_flag = true;
        start_point = Point(x, y); // 记录起始点
    } else if (event == EVENT_LBUTTONUP) {
        drawing_flag = false;
    } else if (event == EVENT_RBUTTONDOWN) {
        erasing_flag = true;
        start_point = Point(x, y); // 记录擦除起始点
    } else if (event == EVENT_RBUTTONUP) {
        erasing_flag = false;
    }

    if (drawing_flag && (x >= 0 && x < gray_map.cols && y >= 0 && y < gray_map.rows)) {
        // 绘制黑色障碍物的直线
        line(gray_map, start_point, Point(x, y), Scalar(0, 0, 0), settings.lineThickness);  
        start_point = Point(x, y); // 更新起点
    }

    if (erasing_flag && (x >= 0 && x < gray_map.cols && y >= 0 && y < gray_map.rows)) {
        // 擦除障碍物的直线
        line(gray_map, start_point, Point(x, y), Scalar(255, 255, 255), settings.lineThickness);  
        start_point = Point(x, y); // 更新起点
    }
}


// 保存地图到文件
void DrawMap::saveMap(const string& filename) 
{
    imwrite(filename, gray_map);
    cout << "地图已保存为 " << filename << endl;
}

    // 获取当前地图
Mat DrawMap::getMap(void) {
    return gray_map;
}

void DrawMap::saveBinaryMapToVectorAndPrint() {
    Mat gray_image;
    cvtColor(gray_map, gray_image, COLOR_BGR2GRAY); // 转换为单通道灰度图

    Mat binary_map;
    threshold(gray_image, binary_map, 128, 255, THRESH_BINARY);

    // 修复：先调整binary_save_map的大小
    int rows = binary_map.rows;
    int cols = binary_map.cols;
    binary_save_map.clear();  // 清空原有数据
    binary_save_map.resize(rows);
    for (int i = 0; i < rows; ++i) {
        binary_save_map[i].resize(cols);
    }

    for (int i = 0; i < binary_map.rows; ++i) {
        for (int j = 0; j < binary_map.cols; ++j) {
            int pixel_value = binary_map.at<uchar>(i, j);
            // 将每个像素值存入二维 vector 中
            binary_save_map[i][j] = (pixel_value == 255 ? 0 : 1);
        }
    }

    // 打印二值化后的二维 vector
    for (size_t i = 0; i < binary_save_map.size(); ++i) {
        for (size_t j = 0; j < binary_save_map[i].size(); ++j) {
            cout << binary_save_map[i][j] << " ";
        }
        cout << endl;
    }
}

void DrawMap::fillEnclosedAreas(vector<vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) return;
    // 定义方向：上、右、下、左
    const int dx[] = {-1, 0, 1, 0};
    const int dy[] = {0, 1, 0, -1};
    int rows = grid.size();
    int cols = grid[0].size();
    
    // 标记矩阵：记录是否被访问过（边界连接的0）
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    queue<pair<int, int>> q;

    // Step 1: 标记所有边界上的0及其连通区域
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // 只处理边界上的点
            if ((i == 0 || i == rows - 1 || j == 0 || j == cols - 1) && grid[i][j] == 0 && !visited[i][j]) {
                q.push({i, j});
                visited[i][j] = true;
                // std::cout << " pushed ( " << i << ","<< j << " )" << std::endl;
                // BFS标记所有边界连接的0
                while (!q.empty()) {
                    auto [x, y] = q.front();
                    q.pop();
                    
                    for (int d = 0; d < 4; ++d) {
                        int nx = x + dx[d];
                        int ny = y + dy[d];
                        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && 
                            grid[nx][ny] == 0 && !visited[nx][ny]) {
                            visited[nx][ny] = true;
                            q.push({nx, ny});
                        }
                    }
                }
            }
        }
    }

    // Step 2: 将未被标记的内部0填充为1
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 0 && !visited[i][j]) {
                grid[i][j] = 1;
            }
        }
    }
    
    std::cout << " 地图洪水填充后: " << std::endl;
        // 打印二值化后的二维 vector
    for (size_t i = 0; i < binary_save_map.size(); ++i) {
        for (size_t j = 0; j < binary_save_map[i].size(); ++j) {
            cout << binary_save_map[i][j] << " ";
        }
        cout << endl;
    }
}

void DrawMap::setMapCallback(const string& windowName) {
    namedWindow(windowName, 0);
    setMouseCallback(windowName, [](int event, int x, int y, int flags, void* userdata) {
        static_cast<DrawMap*>(userdata)->mouseHandler(event, x, y, flags);
    }, this);
}

// 修改 drawPath 函数中的绘制参数
void DrawMap::drawPath(const vector<pair<int, int>>& path) {
    // 绘制路径点
    for (size_t i = 0; i < path.size(); ++i) {
        pair<int, int> p = path[i];
        if (p.first < 0 || p.first >= gray_map.cols || p.second < 0 || p.second >= gray_map.rows)
            continue;

        Scalar color;
        if (i == 0) {
            color = Scalar(0, 0, 255);     // 起点：红色
        } else if (i == path.size() - 1) {
            color = Scalar(0, 255, 0);      // 终点：绿色
        } else {
            color = Scalar(200, 50, 50);    // 路径：黄色
        }

        // 修改点半径从 4 调整为 2（关键修改）
        // circle(gray_map, Point(p.first, p.second), 2, color, -1);
        // line(gray_map, Point(p.first, p.second), Point(p.first, p.second), color, 1); // 更细的路径线
        rectangle(gray_map, Point(p.first, p.second), Point(p.first, p.second), color, 1);
    }

    imshow("Interactive Map", gray_map);
    waitKey(1);
}

void DrawMap::drawCowPath(const vector<pair<int, int>>& path) {
    Mat temp_map = gray_map.clone();  // 在临时地图上绘制
    
    for (size_t i = 0; i < path.size(); ++i) {
        pair<int, int> p = path[i];
        // 绘制路径线（连接相邻点）
        if (i > 0) {
            line(temp_map, 
                Point(path[i-1].first, path[i-1].second),
                Point(p.first, p.second),
                Scalar(255, 0, 255),  // 紫色路径线
                1);
        }
        // 绘制当前点
        circle(temp_map, Point(p.first, p.second), 2, Scalar(0, 165, 255), -1);  // 橙色点
    }

    imshow("Interactive Map", temp_map);
    waitKey(0);
}



void DrawMap::findAndDrawPath(Point start, Point end) {
    AStar astar(binary_save_map, {start.x, start.y}, {end.x, end.y});  // 使用A*算法
    vector<pair<int, int>> path = astar.findPath();  // 获取路径
    drawPath(path);  // 绘制路径
}

void DrawMap::updateRegionDisplay() {
    if (regionMap.empty()) return;

    // 创建基础显示图像
    int rows = regionMap.size();
    int cols = regionMap[0].size();
    regionDisplay = Mat(rows, cols, CV_8UC3, Scalar(255, 255, 255));

    // 预定义颜色表（可扩展）
    const vector<Scalar> colors = {
        Scalar(255, 0, 0),     // 蓝色
        Scalar(0, 255, 0),     // 绿色
        Scalar(0, 0, 255),     // 红色
        Scalar(0, 255, 255),   // 黄色
        Scalar(255, 0, 255),   // 品红
        Scalar(255, 255, 0)    // 青色
    };

    // 填充区域颜色
    for (int x = 0; x < rows; ++x) {
        for (int y = 0; y < cols; ++y) {
            const int regionId = regionMap[x][y].id;
            if (regionId > 0) {
                const Scalar color = colors[(regionId-1) % colors.size()];
                regionDisplay.at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
            } else {
                regionDisplay.at<Vec3b>(x, y) = Vec3b(0, 0, 0); // 障碍物显示黑色
            }
        }
    }

    // 放大显示（保持像素清晰）
    resize(regionDisplay, regionDisplay, Size(), pixelScale, pixelScale, INTER_NEAREST);
}
// 在draw_map.cpp中添加实现：

void DrawMap::mapDrawerThread(void) 
{
    int map_h = 10, map_w = 10;
    ClearMapInit(map_w, map_h);  // 初始化地图大小
    setMapCallback("Interactive Map");

    Point startPoint(1, 1);  // 假定起点 (5,5)
    Point endPoint(map_w-1, map_h-1);  // 假定终点 (45,45)

    while (true) {
        imshow("Interactive Map", getMap());  // 显示地图

        // 显示区域窗口（如果激活）
        if (showRegionWindow && !regionDisplay.empty()) {
            imshow("BCD Regions", regionDisplay);
        }

        char key = waitKey(1);
        if (key == 's') {
            saveMap("gray_map.png");
        } else if (key == 'q') {
            saveMap("gray_map.png");
            saveBinaryMapToVectorAndPrint();
            cout << "程序已退出，地图已保存为 gray_map.png" << endl;
            exit(0);
            break;
        } else if (key == 'p') {
            saveBinaryMapToVectorAndPrint();
            findAndDrawPath(startPoint, endPoint);  // 执行 A* 搜索并绘制路径
        }else if (key == 'b') {  // 新增牛耕法路径
            BcdPlanner planner;
            saveBinaryMapToVectorAndPrint();
            fillEnclosedAreas(binary_save_map);
            // 生成区域数据
            regionMap = planner.BcdPlannerHandle(binary_save_map);
            
            // 准备显示数据
            updateRegionDisplay();
            showRegionWindow = true;
            
            // 创建独立窗口
            namedWindow("BCD Regions", WINDOW_NORMAL);
            moveWindow("BCD Regions", gray_map.cols + 50, 100); // 偏移显示位置
        }

        // ESC键关闭区域窗口
        if (key == 27) { // ASCII 27 = ESC
            destroyWindow("BCD Regions");
            showRegionWindow = false;
        }


    }
}

void DrawMap::mapDrawThreadStart(void)
{
    // 启动线程处理地图显示和保存
    thread mapThread(&DrawMap::mapDrawerThread, this);
    // 等待线程结束
    mapThread.join();
}
