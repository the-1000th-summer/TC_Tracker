""" 此程序用来跟踪台风：读取bin文件 """
import pickle
# from netCDF4 import num2date
from datetime import datetime
import itertools
import numpy as np
from tp_class.Typhoon import Typhoon
# from tools.find_nearest_tp import find_nearest_tp, find_nearest_real_tp
from tools.get_original_data import get_lonlat_data, get_time_data
####################### CONSTANTS ##########################
from math_calculate.constants import LINK_TP_MAX_DIST, STEP1_FILE_DIR, STEP2_FILE_DIR, VOR_FILE_PATH
from math_calculate.point_dist import point_dist

def get_real_Tp(i_binFileName, o_binFileName, LTMD):
    """ 第二步：跟踪第一步生成的每天的台风，生成真正的台风对象。

    Args:
        file_dir (str): 相对涡度场nc文件绝对路径（已删除此参数）
        i_binFileName (str): 第一步生成的bin文件的文件名
        o_binFileName (str): 这一步生成的bin文件的文件名
        LTMD (float): 台风在一个时次内走的最远距离阈值参数
    Notes:
        此函数最终生成了一个bin文件
    """
    ### 常数处理 ###
    LTMD_inF = LTMD if LTMD != 0 else LINK_TP_MAX_DIST
    ###############
    time = get_time_data(VOR_FILE_PATH)
    lon, lat = get_lonlat_data(VOR_FILE_PATH)
    print('第二步(get_read_tp): 成功导入文件。开始跟踪。')
    ### 读取二进制文件
    with open(STEP1_FILE_DIR + i_binFileName,'rb') as tp_date_file:
        tp_dates = pickle.load(tp_date_file)
        all_date_tps = pickle.load(tp_date_file)

    start_year = all_date_tps[0][0].date.strftime('%Y')
    date_iter = time.index(datetime.strptime(start_year+'010100','%Y%m%d%H'))
        ### 在以下的while循环中使用：记录今天是哪天？
        ### (这里赋的值为从基准日期起算的 start_year 1月1日00时 对应的时次)
    last_date = tp_dates[-1]          ### 记录最后有台风是哪天
    is_tp_date_today, is_tp_date_yest = False, False   ### 记录今天和前一天是否有台风
    tp_No = 1                           ### 台风编号
    temp_tps = []                       ### 存储跟踪时仍未确定消亡日期的台风
    real_tps = []                       ### 存储跟踪完成的台风

    print('date_iter_first:',date_iter,'    last_date:',last_date)
    while date_iter <= last_date:
        # if date_iter == 7849:
        #     print('7849!')
        # print(date_iter)
        is_tp_date_today = True if (date_iter in tp_dates) else False  ### 今天是否有台风？

        if not is_tp_date_today:
            if not is_tp_date_yest:    ### 今天昨天都没台风，直接跳到下一天
                pass                   ### 此pass不用做更改
            else:                      ### 今天无台风，昨天有台风，确定台风消亡日期
                for i, _ in enumerate(temp_tps):
                    # temp_tps[i].end_date = num2date(date_iter-1, time_units, time_calendar)
                    temp_tps[i].end_date = time[date_iter-1]
                real_tps += temp_tps
                temp_tps = []               ### 所有台风完成跟踪，清空临时数组
        else:
            if not is_tp_date_yest:    ### 今天有台风，但昨天无台风，创建新的台风对象
                today_tps = all_date_tps[tp_dates.index(date_iter)] ### 今天的台风(Tp_Day)
                today_tp_num = len(today_tps)  ### 今天台风个数
                for i, tp_No_i in enumerate(range(tp_No, tp_No+today_tp_num)):
                    temp_tps.append(Typhoon(tp_No_i, [today_tps[i].max_pt], [today_tps[i].mws_10m], [today_tps[i].mws_allLs], [today_tps[i].twoPCenterLL], [today_tps[i].fourPCenterLL], [today_tps[i].allPCenterLL], today_tps[i].date, today_tps[i].date))

                tp_No += today_tp_num  ### 更新台风编号，注意更新后的编号所代表的台风仍未出现

            else:                      ### 今天昨天都有台风

                today_tps = all_date_tps[tp_dates.index(date_iter)] ### 今天的台风(Tp_Day)
                today_tp_num = len(today_tps)  ### 今天台风个数
                # yest_tps = all_date_tps[tp_dates.index(date_iter)-1] ### 昨天的台风(Tp_Day)
                # yest_tp_num = len(yest_tps)    ### 昨天台风个数

                today_tps_copy = today_tps.copy()   ### 今天的台风拷贝([Tp_Day])
                temp_tps_copy = temp_tps.copy()  ### 拷贝正在跟踪的台风 ([Typhoon])
                temp_tps_cForD = temp_tps.copy()  ### 拷贝正在跟踪的台风用于判断每个台风是否被处理

                ################################

                temp_tps_max_pt = [temp_tp.max_pts[-1] for temp_tp in temp_tps]
                today_tps_max_pt = [today_tp.max_pt for today_tp in today_tps]
                temp_today_dists = np.array([point_dist(lon,lat,pt1,pt2) for (pt1,pt2) in itertools.product(temp_tps_max_pt, today_tps_max_pt)]).reshape(len(temp_tps_max_pt), len(today_tps_max_pt))
                # print(temp_today_dists)
                while temp_tps_cForD:               ### 当昨日台风仍未全部对应完时

                    if not today_tps_copy:
                        ### 如果进入了这个条件，就说明今天的台风比昨天的少，今天的台风都对应成功
                        real_tps.append(temp_tps_cForD[0])        ### 该台风昨天就消亡了
                        temp_tps_copy.remove(temp_tps_cForD[0])    ### 从临时数组中移去已消亡的台风
                        temp_tps_cForD.remove(temp_tps_cForD[0])
                        continue
                    ### 寻找两个靠的最近的昨日台风与今日台风
                    min_dist_index = np.unravel_index(temp_today_dists.argmin(), temp_today_dists.shape)
                    min_dist = temp_today_dists[min_dist_index]
                    minD_tempTp, minD_todayTp = temp_tps[min_dist_index[0]], today_tps[min_dist_index[1]]

                    if min_dist > LTMD_inF:     ### 该台风昨天就消亡了
                        real_tps.append(minD_tempTp)
                        temp_tps_copy.remove(minD_tempTp)   ### 从临时数组中移去已消亡的台风
                        temp_tps_cForD.remove(minD_tempTp)

                        temp_today_dists[min_dist_index[0],:] = 1e5
                        # temp_today_dists[:,min_dist_index[1]] = 1e5
                    else:                      ### 对应成功，移除今天对应的台风
                        ### 更新台风移动到的位置
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].max_pts.append(minD_todayTp.max_pt)
                        ### 更新最大风速
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].mws_10ms.append(minD_todayTp.mws_10m)
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].mws_allLss.append(minD_todayTp.mws_allLs)
                        ### 更新台风长轴中心移动到的位置
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].twoPCenLLs.append(minD_todayTp.twoPCenterLL)
                        ### 更新台风四点中心移动到的位置
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].fourPCenLLs.append(minD_todayTp.fourPCenterLL)
                        ### 更新台风几何中心移动到的位置
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].allPCenterLLs.append(minD_todayTp.allPCenterLL)
                        temp_tps_copy[temp_tps_copy.index(minD_tempTp)].end_date = time[date_iter]

                        temp_tps_cForD.remove(minD_tempTp)
                        today_tps_copy.remove(minD_todayTp)  ### 移除对应成功的今天的台风

                        temp_today_dists[min_dist_index[0],:] = 1e5
                        temp_today_dists[:,min_dist_index[1]] = 1e5

                temp_tps = temp_tps_copy.copy()

                ### 昨天所有的台风对应完，today_tps_copy剩下新生成的、未被跟踪的台风
                if today_tps_copy:    ### 判断 today_tps_copy 是否为空
                    for i, tp_No_i in enumerate(range(tp_No, tp_No+len(today_tps_copy))):
                        today_tp_copy = today_tps_copy[i]
                        temp_tps.append(Typhoon(tp_No_i, [today_tp_copy.max_pt], [today_tp_copy.mws_10m], [today_tp_copy.mws_allLs], [today_tp_copy.twoPCenterLL], [today_tp_copy.fourPCenterLL], [today_tp_copy.allPCenterLL], today_tp_copy.date, today_tp_copy.date))

                    tp_No += len(today_tps_copy)  ### 更新台风编号，注意更新后的编号所代表的台风仍未出现

        is_tp_date_yest = is_tp_date_today     ### 到第二天之前更新第二天的昨天有无台风
        date_iter += 1

    real_tps += temp_tps
    temp_tps = []

    ### 保存此阶段（第二阶段）的二进制文件
    with open(STEP2_FILE_DIR + o_binFileName,'wb') as real_tp_save_file:
        pickle.dump(real_tps, real_tp_save_file)

# if __name__ == "__main__":
#     vor_file_dir = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
#     get_real_Tp(vor_file_dir, 'step1_2pt0_1.bin', 'step2_2pt0_1.bin')
