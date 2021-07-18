""" 此程序用于识别台风，初步预定为从相对涡度场识别。"""
import pickle
from datetime import datetime
from tqdm import tqdm
# import numpy as np
from netCDF4 import Dataset

from tools.num_tp_today import num_tp_today
from tools.pickleDump import pickleDump
####################### CONSTANTS ##########################
from math_calculate.constants import STEP1_FILE_DIR, VOR_FILE_PATH
from math_calculate.constants import TODAY_MAX_TP_NUM,HAS_TP_MIN_ReVOR,TP_MIN_E,TP_MIN_PTS, RECURSION_MIN_ReVOR

def recognize_typhoon(binFileName, numstr, time, lon, lat, vorField_all, TMTN, HTMR, TMP, TME, RMR, start_year=None, end_year=None, get_max_wnd_10=True, get_max_wnd_all=True):
    """ 第一步：找出有台风的日期，记录日期与台风信息
    Args:
        filePath (str): 相对涡度场nc文件绝对路径（已删除此参数）
        binFileName (str): 这一步生成的bin文件的文件名
        numstr (str): 从1开始的文件索引（补零，共5位数）字符串
        TMTN (float): 一天中最多会出现的涡旋数量
        HTMR (float): 查看相对涡度场时认为可能有台风的最小相对涡度值
        TMP (float): 递归找出“台风”包含的所有空间点，如果点数少于这个值则认为这不是台风
        TME (float): 认为这个是台风的最大偏心率（偏心率计算方法仍需改进）
        RMR (float): 找出所有点的递归中限定相对涡度的最小值
        start_year (int): 数据开始的年份
        end_year (int): 数据结束的年份
        get_max_wnd_10 (bool): 是否从10m风场获取其最大风速
        get_max_wnd_all (bool): 是否从整层风场获取其最大风速
    """
    ### 常数处理 ###
    TMTN_inF = TMTN if TMTN != 0 else TODAY_MAX_TP_NUM
    HTMR_inF = HTMR if HTMR != 0 else HAS_TP_MIN_ReVOR
    TMP_inF = TMP if TMP != 0 else TP_MIN_PTS
    TME_inF = TME if TME != 0 else TP_MIN_E
    RMR_inF = RMR if RMR != 0 else RECURSION_MIN_ReVOR
    ###############

    ### start_year 和 end_year 填实际的年份整数数字

    
    print('第一步(recognize_typhoon): 导入文件成功，开始识别。')

    tp_dates = []     ### 记录所有有台风的日期的数组（每个元素为int：从基准日期起算的时次数）
                          ### 基准日期：文件第一个时次数据的日期，JRA-55: 1979年1月1日
    all_date_tps = []  ### 记录所有有台风的日期中所有的台风（会重复记录台风）（类型：[[Tp_Day]]）
    tp_num_yesterday = 0       ### 记录前一天的台风数目

    ### 得到程序要识别的数据的开始结束日期
    file_start_year, file_end_year = int(time[0].strftime('%Y')), int(time[-1].strftime('%Y'))
    if start_year is None or end_year is None:
        start_year, end_year = file_start_year, file_end_year
    else:
        check_year(start_year, end_year, file_start_year, file_end_year) ### 检查设定起始时间和结束时间的有效性
    start_index = time.index(datetime.strptime(str(start_year)+'010100','%Y%m%d%H'))
    end_index = time.index(datetime.strptime(str(end_year)+'123100','%Y%m%d%H'))
    # print(start_index,end_index)
    # print(type(lon))

    for i in tqdm(range(start_index, end_index+1), ncols=70, desc='task:'+numstr, mininterval=5):
        # print(i)
        now_date_i = i  ### 现在日期的时次离数据第一个时次的时次数
        today_vor_field = vorField_all[i].copy()   ### 获取某天的相对涡度场

        today_date = time[i]        ### 今天的日期，类型为 <class 'datetime.datetime'>
        typhoon_num, today_tps = num_tp_today(lon, lat, today_vor_field, tp_num_yesterday, today_date, now_date_i, TMTN_inF, HTMR_inF, TMP_inF, TME_inF, RMR_inF, get_max_wnd_10=get_max_wnd_10, get_max_wnd_all=get_max_wnd_all)      ### 判断当天有几个台风
        tp_num_yesterday = typhoon_num            ### 记录台风数，在下一次循环中用
        if typhoon_num >= 1:
            # tp_dates.append(time[i].strftime('%Y-%m-%d'))
            tp_dates.append(i)
            all_date_tps.append(today_tps)

    
    # tp_dates, all_date_tps = remove_single_day(tp_dates, all_date_tps)

    ### 保存此阶段（第一阶段）的二进制文件
    ### 顺序存入有台风的日期（数组）和 每天的台风对象（数组（中有数组））
    pickleDump(STEP1_FILE_DIR + binFileName, tp_dates, all_date_tps)

# def get_time_index(all_time_list: list, input_datetime: datetime):
    # """ 此函数寻找某个日期在日期列表中的位置 """
    # return np.argwhere(all_time_list == input_datetime)
    # return all_time_list.index(input_datetime)

def check_year(start_year, end_year, file_start_year, file_end_year):
    """ 检查设定起始时间和结束时间的有效性。

    Args:
        start_year (int): 设定的起始时间
        end_year (int): 设定的结束时间
        file_start_year (int): nc文件的起始时间
        file_end_year (int): nc文件的结束时间
    """
    if start_year < file_start_year:
        raise Exception("start_year 小于文件开始年份！")
    if end_year > file_end_year:
        raise Exception("end_year 大于文件结束年份！")
    if end_year < start_year:
        raise Exception('end_year 小于 start_year')

def remove_single_day(tp_dates, all_date_tps):
    """ 此函数移除单独的“台风”与日期。（此函数已弃用）

    Args:
        tp_dates ([int]): 记录所有有气旋的日期的数组（每个元素为int：从基准日期起算的时次数）
        all_date_tps ([[Tp_Day]]): 所有有台风的日期中所有的气旋，array中每个元素代表一个时次的气旋列表
    Returns:
        tp_dates_nsd ([int]): 记录所有有气旋的日期的数组（移除了单独一天的气旋）
        all_date_tps_nsd ([[Tp_Day]]): 所有有气旋的日期中所有气旋（移除了单独一天的气旋）
    """
    tp_dates_nsd = tp_dates.copy()            ### nsd: No single day，此数组将把单日的“台风日期”删去
    all_date_tps_nsd = all_date_tps.copy()    ### 此数组将把单日的“台风”删去

    not_contin, is_single = True, True             ### 不连续？ 单独？
    for i in range(len(tp_dates)-1):        ### 这是将单独的日期删去的算法，采用两个零一变量
        p_date, n_date = tp_dates[i], tp_dates[i+1]
        if p_date+1 != n_date:
            if not_contin:
                is_single = True
            else:
                not_contin = True
        else:
            not_contin, is_single = False, False
        if not_contin and is_single:
            tp_dates_nsd.remove(tp_dates[i])
            all_date_tps_nsd.remove(all_date_tps[i])

    return tp_dates_nsd, all_date_tps_nsd

# if __name__ == "__main__":
#     vor_file_dir = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
#     recognize_typhoon(vor_file_dir, 'JRA-55_step1.bin', 1979, 1979)
