"""
    此函数返回所有在台风内的点（阈值采用相对涡度）
"""
import numpy as np
from tools.get_original_data import get_lonlat_data, get_vor_field
from tools.scatter_diagram import scatter_diagram
from math_calculate.constants import RECURSION_MIN_ReVOR

def get_typhoon_points(field, point, RMR_inf):
    """ 此函数接受一个点，递归返回所有在台风内的点（阈值采用相对涡度）。

    Args:
        field (numpy.ndarray): 二维相对涡度场
        point ([经度float, 纬度float]): 点
        RMR_inf (float): 台风所包含的点的下限
    Returns:
        [[经度float, 纬度float]]: 点组成的list
    """

    all_points = []

    def addin_allPoint(point):
        """ 递归函数体：找出所有的点。

        Args:
            point ([经度float, 纬度float]): 点
        """
        # if point[0] == 0 or point[0] == field.shape[1]-1 or point[1] == 0 or point[1] == field.shape[0]-1:
            # print('fff')          ### 88                                                54
            # return          ### 边界点全部排除（注意这里没考虑到边界点为大值中心，若如此则使得次大值中心无法被检测到）

        if field[point[1], point[0]] >= RMR_inf:
            if point not in all_points:
                all_points.append(point)

                outer_points = getOuterPoints(field, point)

                for i in outer_points:
                    addin_allPoint(i)

    addin_allPoint(point)

    return all_points

def getOuterPoints(field, point):
    """ 此函数接受一个点，返回周围的8个点，如果在边缘则排除边缘外的点。

    Args:
        field (numpy.ndarray): 二维相对涡度场
        point ([经度float, 纬度float]): 点
    Returns:
        [[经度float, 纬度float]]: 点组成的list
    """
    outer_points = [
        [point[0]-1, point[1]+1], [point[0]-1, point[1]], [point[0]-1, point[1]-1],
        [point[0], point[1]+1], [point[0], point[1]-1],
        [point[0]+1, point[1]+1], [point[0]+1, point[1]], [point[0]+1, point[1]-1]
    ]
    if point[0] == 0:                     ### 点在左边缘
        for i in [ [point[0]-1,point[1]+1],[point[0]-1, point[1]],[point[0]-1, point[1]-1]]:
            if i in outer_points:
                outer_points.remove(i)
    if point[0] == field.shape[1]-1:      ### 点在右边缘
        for i in [ [point[0]+1,point[1]+1],[point[0]+1,point[1]],[point[0]+1, point[1]-1] ]:
            if i in outer_points:
                outer_points.remove(i)
    if point[1] == field.shape[0]-1:      ### 点在上边缘
        for i in [ [point[0]-1,point[1]+1],[point[0], point[1]+1],[point[0]+1, point[1]+1] ]:
            if i in outer_points:
                outer_points.remove(i)
    if point[1] == 0:                     ### 点在下边缘
        for i in [ [point[0]-1,point[1]-1],[point[0], point[1]-1],[point[0]+1, point[1]-1] ]:
            if i in outer_points:
                outer_points.remove(i)

    return outer_points

if __name__ == "__main__":
    vor_file_dir = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
    lon, lat = get_lonlat_data(vor_file_dir)
    aV = get_vor_field(vor_file_dir, 91)
    index = np.unravel_index(aV.argmax(), aV.shape)  ### 找到场中的最大值对应的坐标
    index = [index[1], index[0]]
    # print('maxPoint: ', index)
    allP = get_typhoon_points(aV, index, RMR_inf=0)
    print(allP)
    # print(num_tp_today(lon, lat, aV, 0))
    # print(get_e(lon, lat, allP))
    scatter_diagram(allP)
