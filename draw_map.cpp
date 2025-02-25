#include "draw_map.hpp"

void DrawMap::ClearMapInit(int width, int height)
{
    gray_map = Mat(height, width, CV_8UC1, Scalar(255)); // 初始全白地图
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
        line(gray_map, start_point, Point(x, y), Scalar(0), settings.lineThickness);  
        start_point = Point(x, y); // 更新起点
    }

    if (erasing_flag && (x >= 0 && x < gray_map.cols && y >= 0 && y < gray_map.rows)) {
        // 擦除障碍物的直线
        line(gray_map, start_point, Point(x, y), Scalar(255), settings.lineThickness);  
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

void DrawMap::mapDrawerThread(void) 
{
    while (true) {
        imshow("Interactive Map", getMap());  // 显示地图

        char key = waitKey(1);
        if (key == 's') {
            saveMap("gray_map.png");
        } else if (key == 'q') {
            // 自动保存地图并退出
            saveMap("gray_map.png");
            cout << "程序已退出，地图已保存为 gray_map.png" << endl;
            break;
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
