import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import RegularPolygon
from math import sqrt, radians, sin, cos



def bcd_optimized(env: np.ndarray) -> np.ndarray:
    """
    优化后的BCD算法实现
    """
    rows, cols = env.shape
    print(f"环境的行数: {rows}, 列数: {cols}")
    
    cell_grid = np.zeros_like(env, dtype=int)
    current_id = 1
    prev_column = []
    
    for col in range(cols):
        print(f"\n正在处理第 {col + 1} 列")
        
        # 当前列的自由空间分割
        current_segments = []
        segment = []
        for row in range(rows):
            if env[row, col] == 1:
                segment.append(row)
            else:
                if segment:
                    current_segments.append(segment)
                    print(f"当前列自由空间分割添加: {segment}")
                    segment = []
        if segment:
            current_segments.append(segment)
            print(f"当前列自由空间分割添加: {segment}")
        
        print(f"当前列的分割结果: {current_segments}")
        
        # 与前一列进行匹配
        new_column = []
        for seg in current_segments:
            min_row, max_row = min(seg), max(seg)
            print(f"当前区域的行范围: {min_row} 到 {max_row}")

            matched = False
            for prev_seg in prev_column:
                p_min, p_max, p_id = prev_seg
                if (abs(p_min - min_row) <= 5 and abs(p_max - max_row) <= 5):
                    print(f"匹配上一列的区域: {prev_seg} 与当前区域: {seg}")
                    for r in seg:
                        cell_grid[r, col] = p_id
                    new_column.append((min_row, max_row, p_id))
                    matched = True
                    break

            if not matched:
                print(f"未能匹配到上一列的区域，分配新的ID {current_id}")
                for r in seg:
                    cell_grid[r, col] = current_id
                new_column.append((min_row, max_row, current_id))
                current_id += 1

        prev_column = new_column
        print(f"当前列的区域信息: {new_column}")
    
    # 打印前调整NumPy配置
    print("\n最终的cell_grid结果（完整输出）:")
    with np.printoptions(threshold=np.inf, linewidth=200):
        print(cell_grid)
    
    return cell_grid

def generate_complex_environment(shape=(30, 60), num_triangles=8, max_size=10):
    """
    生成包含复杂三角形障碍物的环境
    :param shape: 环境尺寸 (行, 列)
    :param num_triangles: 三角形数量
    :param max_size: 最大边长
    """
    env = np.ones(shape, dtype=np.uint8)
    np.random.seed(42)

    def draw_triangle(center, size, direction, rotation):
        """
        绘制带旋转的三角形
        :param center: 中心坐标 (x, y)
        :param size: 边长
        :param direction: 方向 ('up', 'down', 'left', 'right')
        :param rotation: 旋转角度（度数）
        """
        x, y = center
        h = size * sqrt(3)/2  # 三角形高度

        # 基础顶点坐标（未旋转）
        if direction == 'up':
            points = [(x, y-h/2), (x-size/2, y+h/2), (x+size/2, y+h/2)]
        elif direction == 'down':
            points = [(x, y+h/2), (x-size/2, y-h/2), (x+size/2, y-h/2)]
        elif direction == 'left':
            points = [(x+h/2, y), (x-h/2, y-size/2), (x-h/2, y+size/2)]
        else:  # right
            points = [(x-h/2, y), (x+h/2, y-size/2), (x+h/2, y+size/2)]

        # 应用旋转
        theta = radians(rotation)
        rotated = []
        for px, py in points:
            dx = px - x
            dy = py - y
            new_x = x + dx*cos(theta) - dy*sin(theta)
            new_y = y + dx*sin(theta) + dy*cos(theta)
            rotated.append((new_x, new_y))

        # 创建网格坐标系
        x_grid, y_grid = np.meshgrid(np.arange(shape[1]), np.arange(shape[0]))
        mask = np.zeros_like(env, dtype=bool)

        # 使用重心坐标法判断点是否在三角形内
        A, B, C = rotated
        for i in range(shape[0]):
            for j in range(shape[1]):
                P = (j, i)  # 注意坐标转换(x,y)->(列,行)
                area = 0.5 * (-B[1]*C[0] + A[1]*(-B[0] + C[0]) + A[0]*(B[1] - C[1]) + B[0]*C[1])
                s = 1/(2*area) * (A[1]*C[0] - A[0]*C[1] + (C[1] - A[1])*P[0] + (A[0] - C[0])*P[1])
                t = 1/(2*area) * (A[0]*B[1] - A[1]*B[0] + (A[1] - B[1])*P[0] + (B[0] - A[0])*P[1])
                if s > 0 and t > 0 and (1-s-t) > 0:
                    mask[i, j] = True

        env[mask] = 0

    # 添加随机三角形
    for _ in range(num_triangles):
        x = np.random.randint(0, shape[1])
        y = np.random.randint(0, shape[0])
        size = np.random.randint(5, max_size)
        direction = np.random.choice(['up', 'down', 'left', 'right'])
        rotation = np.random.uniform(0, 360)
        
        draw_triangle((x, y), size, direction, rotation)

    # 保留原有的矩形障碍
    env[10, 20:40] = 0
    env[20, 10:50] = 0
    env[5:25, 45] = 0
    env[15:30, 55] = 0

    for row in env:
        print("".join(map(str, row)))

    return env

# 修改后的可视化函数（添加障碍物边界显示）
def visualize_cells_enhanced(env, cell_grid):
    plt.figure(figsize=(15, 10))
    
    # 原始环境带障碍物边界
    plt.subplot(2, 1, 1)
    plt.imshow(env, cmap='binary', interpolation='nearest')
    plt.title('Complex Environment with Triangles')
    
    # 用轮廓显示障碍物边界
    contours = np.zeros_like(env)
    contours[:-1, :] |= env[1:, :] != env[:-1, :]  # 垂直边界
    contours[:, :-1] |= env[:, 1:] != env[:, :-1]  # 水平边界
    plt.contour(contours, colors='red', linewidths=0.5)
    plt.axis('off')

    # 分解结果
    plt.subplot(2, 1, 2)
    colors = np.random.rand(cell_grid.max()+1, 3)
    colors[0] = [1, 1, 1]
    plt.imshow(colors[cell_grid], interpolation='nearest')
    plt.title('BCD Decomposition Result')
    plt.axis('off')

    plt.tight_layout()
    plt.show()

def visualize_cells(env, cell_grid):
    """可视化结果"""
    plt.figure(figsize=(15, 8))
    
    # 原始环境
    plt.subplot(2, 1, 1)
    plt.imshow(env, cmap='binary', interpolation='nearest')
    plt.title('Original Environment (30x60)')
    plt.axis('off')
    
    # 分解结果
    plt.subplot(2, 1, 2)
    colors = np.random.rand(cell_grid.max()+1, 3)
    colors[0] = [1, 1, 1]  # 白色表示障碍物
    plt.imshow(colors[cell_grid], interpolation='nearest')
    plt.title('BCD Decomposition')
    plt.axis('off')
    
    plt.tight_layout()
    plt.show()


# # 生成测试环境
# env = generate_complex_environment(num_triangles=12, max_size=15)
# cell_grid = bcd_optimized(env)
# visualize_cells_enhanced(env, cell_grid)


def visualize_cells_enhanced(env, cell_grid, paths=None):
    """增强的可视化函数，包含路径绘制"""
    plt.figure(figsize=(15, 10))
    
    # 原始环境带障碍物边界
    plt.subplot(2, 1, 1)
    plt.imshow(env, cmap='binary', interpolation='nearest')
    plt.title('Complex Environment with Triangles')
    
    # 障碍物边界检测
    contours = np.zeros_like(env)
    contours[:-1, :] |= env[1:, :] != env[:-1, :]
    contours[:, :-1] |= env[:, 1:] != env[:, :-1]
    plt.contour(contours, colors='red', linewidths=0.5)
    plt.axis('off')

    # 分解结果可视化
    plt.subplot(2, 1, 2)
    colors = np.random.rand(cell_grid.max()+1, 3)
    colors[0] = [1, 1, 1]  # 障碍物保持白色
    plt.imshow(colors[cell_grid], interpolation='nearest')
    plt.title('BCD Decomposition with Boustrophedon Paths')
    
    # 绘制路径
    if paths is not None:
        for region_id, path in paths.items():
            if len(path) < 2:
                continue  # 跳过单点路径
            
            # 转换坐标为图像坐标系
            x = [c + 0.5 for (r, c) in path]
            y = [r + 0.5 for (r, c) in path]
            
            # 绘制路径线
            plt.plot(x, y, 'b-', linewidth=1, alpha=0.7)
            
            # 绘制箭头
            for i in range(len(path)-1):
                dx = x[i+1] - x[i]
                dy = y[i+1] - y[i]
                plt.arrow(x[i], y[i], dx*0.9, dy*0.9, 
                          head_width=0.3, head_length=0.3,
                          fc='blue', ec='blue', alpha=0.7)
    
    plt.axis('off')
    plt.tight_layout()
    plt.show()

def generate_boustrophedon_paths(cell_grid):
    """生成弓字形路径的核心函数"""
    regions = {}
    rows, cols = cell_grid.shape
    
    # 第一步：提取所有区域
    for i in range(rows):
        for j in range(cols):
            region_id = cell_grid[i, j]
            if region_id != 0:  # 0表示障碍物
                if region_id not in regions:
                    regions[region_id] = []
                regions[region_id].append((i, j))
    
    # 第二步：分析区域特征并生成路径
    paths = {}
    for region_id, cells in regions.items():
        # 计算区域边界
        rows_in_region = [r for r, c in cells]
        cols_in_region = [c for r, c in cells]
        min_row, max_row = min(rows_in_region), max(rows_in_region)
        min_col, max_col = min(cols_in_region), max(cols_in_region)
        
        # 根据宽高比确定路径方向
        width = max_col - min_col + 1
        height = max_row - min_row + 1
        direction = 'horizontal' if height >= width else 'vertical'
        
        # 生成路径
        path = []
        if direction == 'horizontal':
            sorted_rows = sorted(set(r for r, c in cells))
            for i, row in enumerate(sorted_rows):
                cols_in_row = sorted([c for (r, c) in cells if r == row])
                if i % 2 == 0:  # 偶数行正向
                    path.extend([(row, c) for c in cols_in_row])
                else:           # 奇数行反向
                    path.extend([(row, c) for c in reversed(cols_in_row)])
        else:
            sorted_cols = sorted(set(c for r, c in cells))
            for i, col in enumerate(sorted_cols):
                rows_in_col = sorted([r for (r, c) in cells if c == col])
                if i % 2 == 0:  # 偶数列正向
                    path.extend([(r, col) for r in rows_in_col])
                else:           # 奇数列反向
                    path.extend([(r, col) for r in reversed(rows_in_col)])
        
        paths[region_id] = path
    
    return paths

# 主程序流程
if __name__ == "__main__":
    # 生成测试环境
    env = generate_complex_environment(num_triangles=12, max_size=15)
    
    # 运行BCD分解
    cell_grid = bcd_optimized(env)
    
    # 生成弓字形路径
    paths = generate_boustrophedon_paths(cell_grid)
    
    # 可视化结果
    visualize_cells_enhanced(env, cell_grid, paths)










