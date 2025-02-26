#include "draw_map.hpp"
#include <unistd.h>

int main() {
    DrawMap mapDrawer;
    mapDrawer.mapDrawThreadStart();
    while (true) {
        sleep(1);
    }
    return 0;
}
