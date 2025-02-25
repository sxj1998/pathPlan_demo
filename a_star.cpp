#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class MapDrawer {
private:
    Mat grayscale_map;
    bool drawing;
    bool erasing;
    Point start_point;

public:
    MapDrawer(int width, int height) {
        grayscale_map = Mat(height, width, CV_8UC1, Scalar(255)); // 初始全白地图
        drawing = false;
        erasing = false;
    }

    // 获取当前地图
    Mat getMap() {
        return grayscale_map;
    }

    // 鼠标回调函数
    void mouseHandler(int event, int x, int y, int flags) {
        if (event == EVENT_LBUTTONDOWN) {
            drawing = true;
            start_point = Point(x, y); // 记录起始点
        } else if (event == EVENT_LBUTTONUP) {
            drawing = false;
        } else if (event == EVENT_RBUTTONDOWN) {
            erasing = true;
            start_point = Point(x, y); // 记录擦除起始点
        } else if (event == EVENT_RBUTTONUP) {
            erasing = false;
        }

        if (drawing && (x >= 0 && x < grayscale_map.cols && y >= 0 && y < grayscale_map.rows)) {
            // 绘制黑色障碍物的直线
            line(grayscale_map, start_point, Point(x, y), Scalar(0), 5);  
            start_point = Point(x, y); // 更新起点
        }

        if (erasing && (x >= 0 && x < grayscale_map.cols && y >= 0 && y < grayscale_map.rows)) {
            // 擦除障碍物的直线
            line(grayscale_map, start_point, Point(x, y), Scalar(255), 5);  
            start_point = Point(x, y); // 更新起点
        }
    }

    // 保存地图到文件
    void saveMap(const string& filename) {
        imwrite(filename, grayscale_map);
        cout << "地图已保存为 " << filename << endl;
    }
};

int main() {
    // 创建 MapDrawer 对象
    MapDrawer mapDrawer(500, 500);

    // 创建窗口
    namedWindow("Interactive Map");

    // 设置鼠标回调
    setMouseCallback("Interactive Map", [](int event, int x, int y, int flags, void* userdata) {
        static_cast<MapDrawer*>(userdata)->mouseHandler(event, x, y, flags);
    }, &mapDrawer);

    while (true) {
        imshow("Interactive Map", mapDrawer.getMap());  // 显示地图

        char key = waitKey(1);
        if (key == 's') {
            mapDrawer.saveMap("grayscale_map.png");
        } else if (key == 'q') {
            // 自动保存地图并退出
            mapDrawer.saveMap("grayscale_map.png");
            cout << "程序已退出，地图已保存为 grayscale_map.png" << endl;
            break;
        }
    }

    return 0;
}
