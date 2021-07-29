""" 此程序去除非台风（噪音） """

import pickle
from mpl_toolkits.basemap import Basemap
from tools.get_original_data import get_lonlat_data
from tools.get_tp_motion import always_move_east, always_move_west
from tools.get_tp_motion import is_on_land, all_on_land
from math_calculate.point_dist import point_dist
from math_calculate.constants import STEP2_FILE_DIR, STEP3_FILE_DIR, VOR_FILE_PATH

def remove_noise(i_binFileName, o_binFileName, remove=True):
    """ 第三步：去除非台风（噪音）。

    Args:
        file_dir (str): 涡度场nc文件的绝对路径（已删除此参数）
        i_binFileName (str): 第二步产生的bin文件的文件名
        o_binFileName (str): 这一步产生的bin文件的文件名
        remove (bool): 是否去除噪声的开关
    Notes:
        这个函数会产生一个bin文件，用于存储最终的台风list。list中的每个元素为Typhoon的实例
    """
    ### 读取二进制文件
    with open(STEP2_FILE_DIR + i_binFileName, 'rb') as real_tp_file:
        real_tps = pickle.load(real_tp_file)
        lon, lat = get_lonlat_data(VOR_FILE_PATH)

    print('第三步(remove_noise): 导入文件成功，开始去除噪声。')
    bm = Basemap()
    real_tps_copy = real_tps.copy()

    if remove:
        for i, real_tp in enumerate(real_tps):
            tp_start_lon, tp_start_lat = lon[real_tp.max_pts[0][0]], lat[real_tp.max_pts[0][1]]
            tp_end_lon, tp_end_lat = lon[real_tp.max_pts[-1][0]], lat[real_tp.max_pts[-1][1]]
            # pt_max_pts = real_tp.max_pts

            ### 去除不动的点（即包括单个点）
            if tp_start_lon == tp_end_lon and tp_start_lat == tp_end_lat:
                real_tps_copy.remove(real_tp)
                continue
            if len(real_tp.max_pts) <= 2:#13: ### 
                real_tps_copy.remove(real_tp)
                continue
            ### 去除一直在东边的轨迹
            if tp_start_lon >= 170 and tp_end_lon >= 170:
                real_tps_copy.remove(real_tp)
                continue
            ### 排除非热带生成的气旋
            if tp_start_lat >= 33 or tp_start_lat < 0:
                real_tps_copy.remove(real_tp)
                continue
            if always_move_east(real_tp.max_pts):
                ### 去除温带低压
                real_tps_copy.remove(real_tp)
                continue
            if bm.is_land(lon[real_tp.max_pts[0][0]], lat[real_tp.max_pts[0][1]]):
                ### 第一个点在陆地上
                real_tps_copy.remove(real_tp)
                continue
            ### 排除纬度太低的“台风”
            if tp_end_lat < 5 and points_lat_ave(lat, real_tp.max_pts) < 5:
                real_tps_copy.remove(real_tp)
                continue
            ### 排除印度洋的台风
            if points_lon_ave(lon, real_tp.max_pts) < 103:
                real_tps_copy.remove(real_tp)
                continue
            # if max(real_tp.mws_10ms) < 11.36:
            #     # print('remove')
            #     real_tps_copy.remove(real_tp)
            #     continue

            ### 去除等于3个点且不一直向西移动的轨迹
            # if len(real_tp.max_pts) == 3 and (not always_move_west(real_tp.max_pts)):
            #     real_tps_copy.remove(real_tp)
            #     continue

            # if tp_start_lon >= 150 and tp_start_lat >= 34:  ### 去除温带的一些低压

            # if tp_start_lon <= 119 and tp_start_lat >= 25 and tp_end_lon <= 119 and tp_end_lat >= 25:
            #     ### 去除只在大陆的低压
            #     real_tps_copy.remove(real_tp)
            #     continue

            # if all_on_land(real_tp.max_pts):   ### 所有的点都在陆地上
            #     real_tps_copy.remove(real_tp)
            #     continue

            ### 已经登陆的台风在最后一个点突然又回到海洋，且最后一个点和倒数第二个点隔得很远，需要去除掉最后一个点。
            # if (not is_on_land([pt_max_pts[-1][0]-1, pt_max_pts[-1][1]]) or not is_on_land(pt_max_pts[-1])) and is_on_land(real_tp.max_pts[-2]) and point_dist(lon, lat, pt_max_pts[-1], pt_max_pts[-2]) > 500.0:
            #     real_tps[i].max_pts.remove(pt_max_pts[-1])
            #     real_tps[i].end_date = real_tps[i].end_date + datetime.timedelta(days=-1)

    real_tps = real_tps_copy.copy()

    with open(STEP3_FILE_DIR + o_binFileName,'wb') as real_tp_nn_file:
        pickle.dump(real_tps, real_tp_nn_file)

def points_lat_ave(lats, points):
    """ 此函数返回一组点的平均纬度。

    Args:
        lats ([float]): 纬度list
        points ([经度int, 纬度int]): 点
    Returns:
        float: 平均纬度值
    """
    ave = 0
    for point in points:
        ave += lats[point[1]]
    ave /= len(points)
    return ave

def points_lon_ave(lons, points):
    """ 此函数返回一组点的平均经度。

    Args:
        lons ([float]): 经度list
        points ([经度int, 纬度int]): 点
    Returns:
        float: 平均经度值
    """
    ave = 0
    for point in points:
        ave += lons[point[0]]
    ave /= len(points)
    return ave

# if __name__ == "__main__":
#     vor_file_dir = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
#     remove_noise(vor_file_dir, 'JRA-55_step2.bin', 'JRA-55_step3.bin', remove=True)
