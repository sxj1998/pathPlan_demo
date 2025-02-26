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
    Mat binary_map;
    // 将地图二值化，阈值设置为 128，低于128的像素设置为255（白色），高于128的像素设置为0（黑色）
    threshold(gray_map, binary_map, 128, 255, THRESH_BINARY);

    // 打印二值化地图到 vector 中
    vector<int> mapVector;
    for (int i = 0; i < binary_map.rows; ++i) {
        for (int j = 0; j < binary_map.cols; ++j) {
            int pixel_value = binary_map.at<uchar>(i, j);
            // 将 255（白色）转换为 0，0（黑色）转换为 1
            mapVector.push_back(pixel_value == 255 ? 0 : 1);
        }
    }

    // 打印二值化后的 vector
    for (size_t i = 0; i < mapVector.size(); ++i) {
        cout << mapVector[i] << " ";
        if ((i + 1) % binary_map.cols == 0) {
            cout << endl;
        }
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
    waitKey(0);
}



void DrawMap::findAndDrawPath(Point start, Point end) {
    // 将 OpenCV Mat 转换为二维数组
    vector<vector<int>> mapData(gray_map.rows, vector<int>(gray_map.cols, 0));
    for (int y = 0; y < gray_map.rows; ++y) {
        for (int x = 0; x < gray_map.cols; ++x) {
            if (gray_map.at<uchar>(y, x) == 0) {
                mapData[y][x] = 1;  // 设置障碍物
            }
        }
    }

    AStar astar(mapData, {start.x, start.y}, {end.x, end.y});  // 使用A*算法
    vector<pair<int, int>> path = astar.findPath();  // 获取路径
    drawPath(path);  // 绘制路径
}

void DrawMap::mapDrawerThread(void) 
{
    ClearMapInit(20, 20);  // 初始化地图大小
    setMapCallback("Interactive Map");

    Point startPoint(1, 1);  // 假定起点 (5,5)
    Point endPoint(19, 19);  // 假定终点 (45,45)

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
            findAndDrawPath(startPoint, endPoint);  // 执行 A* 搜索并绘制路径
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
