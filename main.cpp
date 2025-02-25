#include "draw_map.hpp"

int main() {
    DrawMap mapDrawer;
    mapDrawer.ClearMapInit(50, 50);  // 初始化地图大小

    // 设置鼠标回调
    mapDrawer.setMapCallback("Interactive Map");

    // 启动线程处理地图显示和保存
    mapDrawer.mapDrawThreadStart();

    mapDrawer.saveBinaryMapToVectorAndPrint();

    return 0;
}
