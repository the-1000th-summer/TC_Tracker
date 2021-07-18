"""_"""

import numpy as np
from netCDF4 import Dataset
from tools.get_typhoon_points import get_typhoon_points
from math_calculate.get_e import get_e
from math_calculate.constants import WIND_10M_FP, WIND_ALLLEVS_FP
from tp_class.Tp_Day import Tp_Day

def fffnum_tp_today(lon, lat, VField, tp_num_Y, date, now_date_i, TMTN_inF, HTMR_inF, TMP_inF, TME_inF, RMR_inF, get_max_wnd_10, get_max_wnd_all):
    """ 此函数识别某天是否有台风以及台风的个数，
    返回当天台风的个数和当天台风组成的列表（台风的类为Tp_Day）。

    Args:
        lon (list): 数据的经度数组
        lat (list): 数据的纬度数组
        VField (numpy.ndarray): 这个时次的二维相对涡度场
        tp_num_Y (int): 前一个时次的涡旋数量
        date (datetime.datetime): 这个时次的日期
        now_date_i (int): 这个时次离数据第一个时次的时次数
        TMTN_inF (float): 一天中最多会出现的涡旋数量
        HTMR_inF (float): 查看相对涡度场时认为可能有台风的最小相对涡度值
        TMP_inF (float): 递归找出“台风”包含的所有空间点，如果点数少于这个值则认为这不是台风
        TME_inF (float): 认为这个是台风的最大偏心率（偏心率计算方法仍需改进）
        RMR_inF (float): 找出所有点的递归中限定相对涡度的最小值
        get_max_wnd_10 (bool): 是否从10m风场获取其最大风速
        get_max_wnd_all (bool): 是否从整层风场获取其最大风速

    """

    tp_num = 0           ### 今天台风的个数
    all_tp = []          ### 今天台风组成的列表
    VF_today = VField.copy()
    
    ### 读取10m风速数据
    # wind10mFile = Dataset(WIND_10M_FP)
    # wind10mFile.set_auto_mask(False)
    ### 读取整层风速数据
    # allLevFile = Dataset(WIND_ALLLEVS_FP)
    # allLevFile.set_auto_mask(False)

    for _ in range(int(TMTN_inF)):                      ### 一天最多记录5个台风

        V_max = np.max(VF_today)               ### 该场的最大值
        if V_max < HTMR_inF:  ### 查看每天的相对涡度场，如果场中最大值太小则忽略这天
            break
        max_pt = np.unravel_index(VF_today.argmax(), VF_today.shape)  ### 找到场中的最大值对应的索引
        max_pt = [max_pt[1], max_pt[0]]              ### 注意坐标的顺序！！！
        allP = get_typhoon_points(VF_today, max_pt, RMR_inF)    ### 递归获取代表台风的点的坐标

        # if not allP:          ### allP为空数组，说明最大值在边界上，不考虑
        #     print('not allP from num_tp_today') # （以解决最大值在边界上的问题）
        #     break
        if not allP:     ### 数组是空的
            break
        if len(allP) <= TMP_inF and tp_num_Y == 0: ### 如果点太少且前一天没台风则排除
            # print('point too few and no tp yesterday. Today: ', date)
            break
        e, twoP_cen_ll, fourP_cen_ll = get_e(lon, lat, allP)  ### 偏心率与 台风“中心”真实经纬度
        if e > TME_inF and tp_num_Y == 0 :     ### 如果形状不很圆且前一天没台风则排除
            # print('e too large and no tp yesterday. Today: ', date)
            break

        max_windSpeed, max_windSpeed_allLevs = 0, 0
        # if get_max_wnd_10:
            # 处理风速数据得到整层最大风速和10m最大风速
            # max_windSpeed = np.max(wind10mFile.variables['wind10m'][now_date_i,:,:][changePtsToYXs(allP)])
        # if get_max_wnd_all:
            ### 这里不使用changePtsToYXs的原因是考虑计算速度的原因。
            # max_windSpeed_allLevs = np.max(allLevFile.variables['wind'][now_date_i,23:,[y[1] for y in allP],[x[0] for x in allP]])


        tp_num += 1                         ### 此为台风，台风数加1
        allP_cen_ll = allP_center(allP, lon, lat)   ### 台风的几何中心
        all_tp.append(Tp_Day(max_pt, max_windSpeed, max_windSpeed_allLevs, twoP_cen_ll, fourP_cen_ll, allP_cen_ll, date))   ### 储存当日的台风对象，数组长度应等于tp_num
        VF_today = remove_high(VF_today, allP)     ### 排除已检测出的台风，进行下一轮的检测

    # wind10mFile.close()
    # allLevFile.close()
    return tp_num, all_tp

def changePtsToYXs(pts):
    """ 此函数接受一个由坐标点组成的列表，返回一个由所有点的y坐标的列表和所有点的x坐标的列表组成的列表 """
    return [y[1] for y in pts], [x[0] for x in pts]

def allP_center(allP, lon, lat):
    """ 此函数计算allP的经纬度平均值，即“台风”几何中心，返回一个以经纬度表示的点的列表 """
    ave_lon, ave_lat = 0.0, 0.0
    for p in allP:
        ave_lon += lon[p[0]]
        ave_lat += lat[p[1]]
    ave_lon, ave_lat = ave_lon / len(allP), ave_lat / len(allP)
    return [ave_lon, ave_lat]

def remove_high(VField, points):
    """ 此函数消除大值中心，为识别多个台风服务，替换为1e-6 """
    VF_out = VField.copy()
    np_pts = np.array(points)
    VF_out[np_pts[:,1], np_pts[:,0]] = 1e-6     ### 注意坐标的顺序！！！
    return VF_out
