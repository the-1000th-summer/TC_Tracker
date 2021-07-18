""" 此程序输出代表台风的点中两点间的最大距离及相应的点， """
import numpy as np
import math
import itertools
from numpy import argmax, unravel_index
from netCDF4 import Dataset
from scipy.spatial import distance
from tools.pts_to_lonlats import pts_to_lonlats
from math_calculate.get_slope import get_slope
from math_calculate.point_dist import point_dist


def get_max_distance(lon, lat, points):  ### points非地球坐标
    """ 此程序输出代表台风的点中两点间的最大距离及相应的点 """

    lonlats = pts_to_lonlats(lon, lat, points)   ### 实际地球坐标
    # lonlats = points
    # dists = np.array([point_dist(lon,lat,pt1,pt2) for (pt1,pt2) in itertools.product(points, points)]).reshape(len(points), len(points))
    dists = distance.cdist(lonlats, lonlats, 'euclidean')     ### 距离矩阵
    max_points_index = unravel_index( argmax(dists), dists.shape )   ### 距离矩阵中最大值在矩阵中的坐标
    # print(points[max_points_index[0]], points[max_points_index[1]])

    return np.max(dists), points[max_points_index[0]], points[max_points_index[1]]

def get_min_distance(lon, lat, min_K, cen_pt, points):  ### points非地球坐标
    """ 此函数返回短轴长度 """
    valid_pts = []
    lonlats = pts_to_lonlats(lon, lat, points)   ### 实际地球坐标
    min_rad = math.atan(min_K)   ### 短轴轴线对应的弧度
    for i, ll in enumerate(lonlats):         ### 挑选处于短轴线左右30度所包括的所有点
        pt_rad = math.atan(get_slope(cen_pt, ll))       ### 椭圆中心点与某点的斜率
        if abs(min_rad - pt_rad) <= math.pi / 4:
            valid_pts.append(points[i])
    if len(valid_pts) <= 1:               ### 如果没有一个点或只有一个点符合条件，说明太窄，排除
        return 0.0, cen_pt, cen_pt
    ### 取这些点中两个距离最远的点
    appro_min_dists, appro_min_pt1, appro_min_pt2 = get_max_distance(lon, lat, valid_pts)
    ### 两个距离最远的点连线的弧度
    appro_min_rad = math.atan(get_slope(appro_min_pt1, appro_min_pt2))
    ### 估计线与真实线夹角余弦
    cosAlpha = math.cos(abs(appro_min_rad - min_rad))
    if cosAlpha < 0:                            ### 条带太窄导致两个点都在一边，
        return 0.0, cen_pt, cen_pt           ### 可能使计算出的斜率严重出错（即与长轴有相同趋势）
#     min_dists = (appro_min_dists * cosAlpha + appro_min_dists / cosAlpha) / 2
    ### 利用差的角度修正长度
    min_dists = appro_min_dists / cosAlpha

    min_pt1_lonlat = [lon[appro_min_pt1[0]],lat[appro_min_pt1[1]]]
    min_pt2_lonlat = [lon[appro_min_pt2[0]],lat[appro_min_pt2[1]]]
    return min_dists, min_pt1_lonlat, min_pt2_lonlat

# if __name__ == "__main__":
#     coords = [[73, 45], [72, 45], [71, 45], [70, 45], [69, 45], [68, 45], [67, 45], [66, 44], [65, 44], [64, 44], [63, 43], [62, 43], [61, 43], [60, 43], [59, 42], [58, 42], [57, 41], [58, 41], [60, 42], [61, 42], [62, 42], [64, 43], [65, 43], [67, 44], [68, 44], [69, 44], [70, 44], [71, 44], [72, 44], [73, 44], [74, 45], [74, 44], [75, 45], [75, 44], [76, 45], [76, 44], [77, 44], [78, 44]]
#     # print(len(coords))
#     VFile = Dataset('E:/University/NWPdata/typhoon/generated/DAILY_part_original/Vorticity_daily_1pt5degC_20060101-21001231.nc')
#     VFile.set_auto_mask(False)
#     lat = VFile.variables['lat'][:]
#     lon = VFile.variables['lon'][:]
#     VFile.close()
#     get_max_distance(lon, lat, coords)
