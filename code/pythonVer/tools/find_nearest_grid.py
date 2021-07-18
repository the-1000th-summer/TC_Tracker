"""_"""
import numpy as np
from tools.get_original_data import get_lonlat_data
from math_calculate.constants import VOR_FILE_PATH
from math_calculate.point_dist import haversine
def find_nearest_grid(ll):
    """ 此函数接受一个用经纬度数组表示的点，输出离此点最近的格点 """
    lon, lat = get_lonlat_data(VOR_FILE_PATH)
    lon_big_i, lat_big_i = len(lon)-1, len(lat)-1   ### 这里有个技巧，以简化代码
    round_4_lls = np.zeros((4,2))   ### 左上，左下，右上，右下。用坐标（非经纬度）表示四个点
    ### 寻找ll点右上角的格点的索引，如果ll点在格点外（右边或上边），索引仍维持-100
    for eachLon_i, eachLon in enumerate(lon):
        if eachLon >= ll[0]:
            lon_big_i = eachLon_i
            break
    for eachLat_i, eachLat in enumerate(lat):
        if eachLat >= ll[1]:
            lat_big_i = eachLat_i
            break
    ### 索引(lon_big_i等)为零时，-1会索引到数组最后一个数字，此时计算出的距离会过大，正好被排除。
    ### ll点在格点外（右或上）时，会把这个点当成在格点内最右（上）一列（排）中的点，
    ### 虽然计算出的最小值对应的索引不是正确的索引（即得到的最近的点在ll点的哪个方位），
    ### 但得到的最近的点的绝对坐标是正确的！我们在这里只需要得到正确的绝对坐标就行了
    round_4_lls[0] = [lon[lon_big_i-1], lat[lat_big_i]]   ### 左上
    round_4_lls[1] = [lon[lon_big_i-1], lat[lat_big_i-1]]     ### 左下
    round_4_lls[2] = [lon[lon_big_i], lat[lat_big_i]]     ### 右上
    round_4_lls[3] = [lon[lon_big_i], lat[lat_big_i-1]]   ### 右下

    # print(round_4_lls)
    dist_list = [haversine(ll[0], ll[1], eachll[0], eachll[1]) for eachll in round_4_lls]
    # print(dist_list)
    return_list = [[lon_big_i-1,lat_big_i], [lon_big_i-1,lat_big_i-1], [lon_big_i,lat_big_i], [lon_big_i,lat_big_i-1]]
    # print(return_list[dist_list.index(min(dist_list))])
    return return_list[dist_list.index(min(dist_list))]


    # lon_first, lat_first = lon[0], lat[0]
    # if lon_big == 0.0:
    #     if lat_big == -50.0:       ### 格点外：右上
    #         return [len(lon)-1, len(lat)-1]
    #     elif lat_big == lat_first:      ### 格点外：右下
    #         return [len(lon)-1, 0]      
    # elif lon_big == lon_first:
    #     if lat_big == -50.0:        ### 格点外：左上
    #         return [0, len(lat)-1]
    #     elif lat_big == lat_first:   ### 格点外：左下
    #         return [0, 0]
    # if lon_big == 0.0:        ### 格点外：右
    #     round_pts1 = pts_to_lonlats(lon, lat, [len(lon)-1, ])
    #     pt_dist_1 = haversine(lon, lat, )


    
