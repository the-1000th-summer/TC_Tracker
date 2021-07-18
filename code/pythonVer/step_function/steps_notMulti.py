"""_"""

import os  ###, time, random
import pickle
import numpy as np
from netCDF4 import Dataset

from tools.get_original_data import get_time_data, get_lonlat_data
from step_function.recognize_typhoon_step1 import recognize_typhoon
from step_function.get_real_Tp_step2 import get_real_Tp
# from step_function.remove_noise_step3 import remove_noise
from tools.osTime import nowTime
# from step_function.steps_multi import loadAllNums, generateSNum
from tools.generateSNum import generateSNum
####################### CONSTANTS ##########################
from math_calculate.constants import VOR_FILE_PATH, ALLNUMS_BIN_FP

def steps_notMulti():
    """ 此函数用于单进程执行算法。

    Args:
        vor_filePath (str): 相对涡度场文件绝对路径（此参数已删除）
    """
    print('This is JRA55_1k')
    print('Start steps_multi function.'+nowTime())
    print('Parent process is '+str(os.getpid())+'.')

    # allNums = loadAllNums()
    # allNums = allNums[8000:9000]
    # allNums = [allNums[918], allNums[941], allNums[959]]
    # allNums = [allNums[985]]
    # allNums = [[5, 8e-5, 4, 6e-5, 340*1.5]]
    allNums = [[5, 8e-5, 4, 6e-5, 340]]

    time = get_time_data(VOR_FILE_PATH)
    lon, lat = get_lonlat_data(VOR_FILE_PATH)
    VFile = Dataset(VOR_FILE_PATH)
    VFile.set_auto_mask(False)
    vor_allField = VFile.variables['Vorticity'][:,:,:]
    VFile.close()

    for nums in allNums:
        aStr = generateSNum(nums)
        aNum = str(nums[-1]).zfill(5)
        print('start ' + aNum + ' ' + nowTime())
        binF_str = 'JRA55_' + aNum + '_' + aStr + '_79-18'
        # threeSteps(binF_str, [tp_day_num,0,0,0,0,0])
        threeSteps(time, lon, lat, vor_allField, binF_str, [nums[0],nums[1],nums[2],0,nums[3],nums[4]], aNum)

        print('end ' + aNum + ' ' + nowTime())

    print('Waiting for all subprocesses done...'+nowTime())


    print('All subprocesses done.'+nowTime())

def threeSteps(time, lon, lat, vorField_all, binF_str, con_l, numstr):
    """ 此函数为单进程执行算法，con_l为常数的列表。

    Args:
        vor_filePath (str): 输入的相对涡度场nc文件绝对路径（已删除此参数）
        binF_str (str): 生成的step bin文件的前缀字符串，完整文件名为"binF_str+'_stepX.bin'"
        con_l ([5float, int]): 参数的5个阈值和一个从1开始的int文件索引组成的列表
        numstr (str): 从1开始的文件索引（补零，共5位数）字符串
    """
    print(nowTime())
    print('Run task in function: threeSteps (pid: '+str(os.getpid())+')...')
    assert len(con_l) == 6, '常数列表元素数量不正确！！！！！'
    ###################### 第一步：每个时次找台风 ####################
    recognize_typhoon(binF_str+'_step1.bin', numstr, time, lon, lat, vorField_all, TMTN=con_l[0], HTMR=con_l[1], TMP=con_l[2], TME=con_l[3], RMR=con_l[4], start_year=1979, end_year=2018, get_max_wnd_10=True, get_max_wnd_all=False)
    ###### 第二步：连接台风。以下一行函数中的 nearest_dist 阈值是经验的 #########
    get_real_Tp(binF_str+'_step1.bin', binF_str+'_step2.bin', LTMD=con_l[5])
    ######################## 第三步：去除噪声 ######################
    # remove_noise(binF_str+'_step2.bin', binF_str+'_step3.bin', remove=True)

