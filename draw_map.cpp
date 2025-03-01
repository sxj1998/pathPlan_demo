#include "draw_map.hpp"
#include "a_star.hpp"



void DrawMap::ClearMapInit(int width, int height)
{
    gray_map = Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
    drawing_flag = false;
    erasing_flag = false;
 
}

void DrawMap::mouseHandler(int event, int x, int y, int flags) {
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

// 在draw_map.cpp中添加实现：
std::vector<std::pair<int, int>> DrawMap::generateBoustrophedonPath() {
    std::vector<std::pair<int, int>> path;
    if (binary_save_map.empty()) return path;

    int rows = binary_save_map.size();
    int cols = binary_save_map[0].size();
    bool leftToRight = true;

    for (int y = 0; y < rows; ++y) {
        // 跳过完全障碍行
        bool isObstacleRow = true;
        for (int x = 0; x < cols; ++x) {
            if (binary_save_map[y][x] == 0) {
                isObstacleRow = false;
                break;
            }
        }
        if (isObstacleRow) continue;

        // 生成当前行路径
        if (leftToRight) {
            for (int x = 0; x < cols; ++x) {
                if (binary_save_map[y][x] == 0) {
                    path.emplace_back(x, y);
                }
            }
        } else {
            for (int x = cols-1; x >= 0; --x) {
                if (binary_save_map[y][x] == 0) {
                    path.emplace_back(x, y);
                }
            }
        }
        leftToRight = !leftToRight;
    }
    return path;
}

void DrawMap::mapDrawerThread(void) 
{
    ClearMapInit(50, 50);  // 初始化地图大小
    setMapCallback("Interactive Map");

    Point startPoint(1, 1);  // 假定起点 (5,5)
    Point endPoint(49, 49);  // 假定终点 (45,45)

    while (true) {
        imshow("Interactive Map", getMap());  // 显示地图

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
            saveBinaryMapToVectorAndPrint();
            auto path = generateBoustrophedonPath();
            drawCowPath(path);
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
