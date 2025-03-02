#ifndef DRAW_MAP_HPP
#define DRAW_MAP_HPP

#include "bcd.hpp"
#include <opencv2/opencv.hpp>   // 包含 OpenCV 头文件
#include <iostream>
#include <string>                 // 包含 C++ 标准库的 string 头文件
#include <thread>
#include <mutex>

using namespace cv;
using namespace std;

typedef struct {
    int lineThickness;
}MapDrawerSetting;

class DrawMap {
private:
    Mat gray_map;
    std::vector<std::vector<int>> binary_save_map;
    bool drawing_flag;
    bool erasing_flag;
    Point start_point;
    MapDrawerSetting settings;
    
    std::vector<std::vector<RegionPoint>> regionMap;
    bool showRegionWindow;      // 控制区域窗口显示
    Mat regionDisplay;          // 区域显示专用图像
    const int pixelScale = 10;  // 像素显示缩放倍数
public:
        // 构造函数中通过初始化列表初始化 settings
    DrawMap() : settings{1} , showRegionWindow(false) {  // lineThickness 默认初始化为 5
        // 可以在此处初始化其他成员
        drawing_flag = false;
        erasing_flag = false;
    }
    void ClearMapInit(int width, int height);
    void mouseHandler(int event, int x, int y, int flags);
    void saveMap(const string& filename);
    Mat getMap(void); 
    void saveBinaryMapToVectorAndPrint();
    void fillEnclosedAreas(vector<vector<int>>& grid);
    void setMapCallback(const string& windowName);
    void mapDrawerThread(void); 
    void mapDrawThreadStart(void);
    void drawPath(const vector<pair<int, int>>& path);
    void findAndDrawPath(Point start, Point end);
    void updateRegionDisplay() ;

    std::vector<std::pair<int, int>> generateBoustrophedonPath();
    void drawCowPath(const vector<pair<int, int>>& path);
};


#endif /* DRAW_MAP_HPP */
