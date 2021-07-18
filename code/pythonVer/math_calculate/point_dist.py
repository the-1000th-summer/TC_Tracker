"""_"""

from math import radians, cos, sin, asin, sqrt
import numpy as np
from tools.pts_to_lonlats import pts_to_lonlats
from tools.get_original_data import get_lonlat_data, get_time_data

def haversine(lon1, lat1, lon2, lat2): # 经度1，纬度1，经度2，纬度2 （十进制度数）
    """ Calculate the great circle distance between two points
    on the earth (specified in decimal degrees)

    """
    # 将十进制度数转化为弧度
    lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])

    # haversine公式
    dlon = lon2 - lon1 
    dlat = lat2 - lat1 
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * asin(sqrt(a)) 
    r = 6371 # 地球平均半径，单位为公里
    return c * r   ### 返回距离，单位为公里


def point_dist(lon, lat, pt1, pt2):
    """ 此函数计算返回两个点的真实距离(km)（大圆距离）(haversine公式)。

    Args:
        lon ([float]): 数据格点的经度数组
        lat ([float]): 数据格点的纬度数组
        pt1 ([int, int]): 点
        pt2 ([int, int]): 另一个点
    Returns:
        float: 两点之间的距离，单位为km
    """
    points = np.array([pt1, pt2])
    lonlats = pts_to_lonlats(lon, lat, points)     ### 转换为点数(行)×2列 形状的以经纬度表示所有点的np数组

    # 将十进制度数转化为弧度
    lon1, lat1, lon2, lat2 = lonlats[0,0], lonlats[0,1], lonlats[1,0], lonlats[1,1]
    # print(lon1, lat1, lon2, lat2)
    lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])

    # haversine公式
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * asin(sqrt(a)) 
    r = 6371 # 地球平均半径，单位为公里
    return c * r

# def lonlatPtDist(lon, lat, lonlat, point):
#     pt_lon = lon[point[0]]
#     pt_lat = lat[point[1]]

#     return lonlatDist(lonlat[0], lonlat[1], pt_lon, pt_lat)

def lonlatDist(lonlat1, lonlat2):

    # 将十进制度数转化为弧度
    lon1, lat1, lon2, lat2 = lonlat1[0], lonlat1[1], lonlat2[0], lonlat2[1]
    lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])

    # haversine公式
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * asin(sqrt(a))
    r = 6371 # 地球平均半径，单位为公里
    return c * r

    # dist = sqrt((lonlats[0,0] - lonlats[1,0])**2 + (lonlats[0,1] - lonlats[1,1])**2)
    # return dist


# if __name__ == "__main__":
#     vor_file_dir = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
#     lon, lat = get_lonlat_data(vor_file_dir)
#     print(point_dist(lon, lat, [0,0], [1,0]))
