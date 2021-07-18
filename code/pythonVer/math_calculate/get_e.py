"""_"""

from math import sqrt
from math_calculate.get_minmax_distance import get_max_distance, get_min_distance
from math_calculate.get_slope import get_slope
# from numpy import argmax, unravel_index
# from scatter_diagram import scatter_diagram

def get_e(lon, lat, points):   ### points非地球坐标
    """此程序计算“椭圆”的偏心率。lat从5S~45N, lon......"""

    max_dist, max_pt1, max_pt2 = get_max_distance(lon, lat, points) ### 长轴及两个点的坐标
    # print('max points:', max_pt1, max_pt2)
    A = max_dist / 2      ### 半长轴（单位：度）
    max_ll1 = [lon[max_pt1[0]], lat[max_pt1[1]]]     ### 长轴端点1经纬度
    max_ll2 = [lon[max_pt2[0]], lat[max_pt2[1]]]     ### 长轴端点2经纬度
    max_K = get_slope(max_ll1, max_ll2)   ### 长轴斜率
    min_K = float('inf') if max_K == 0 else -1 / max_K                ### 短轴斜率
    twoP_cen_lonlat = center_point(max_ll1, max_ll2)   ### 椭圆中点坐标
    min_dist, min_ll1, min_ll2 = get_min_distance(lon, lat, min_K, twoP_cen_lonlat, points)  ### 短轴
    cen_lonlat2 = center_point(min_ll1, min_ll2)
    fourP_cen_lonlat = center_point(twoP_cen_lonlat, cen_lonlat2) ### 台风“中心”真实经纬度

    B = min_dist / 2                             ### 半短轴
    # assert A >= B, 'A应大于等于B'
    if B > A:                              ### B可能大于A的情况是台风非常圆但点有限所造成的误差
        A, B = B, A
        # print('发现一次 B > A')
        # scatter_diagram(points)
    E = 0.99 if A == 0 else sqrt(A**2 - B**2) / A                ### 偏心率
        
    # print('e:', E)
    return E, twoP_cen_lonlat, fourP_cen_lonlat    ###  fourP_cen_lonlat: 四个点  twoP_cen_lonlat: 两个长轴端点

def center_point(lonlat1, lonlat2):
    """ 此函数返回两个点的中点的经纬度 """
    return [(lonlat1[0]+lonlat2[0])/2, (lonlat1[1]+lonlat2[1])/2]
def center_lonlat(lonlat1, lonlat2, lon, lat):
    """ 计算台风“中心”真实经纬度，使用长轴两个端点经纬度的平均值 """
    centerLon = (lon[lonlat1[0]] + lon[lonlat2[0]]) / 2
    centerLat = (lat[lonlat1[1]] + lat[lonlat2[1]]) / 2
    return [centerLon, centerLat]
