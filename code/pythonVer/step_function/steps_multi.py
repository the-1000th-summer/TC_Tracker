""" 此模块将五个阈值参数排列组合成33000个组合运行 """

from multiprocessing import Pool
import os  ###, time, random
import pickle
import numpy as np
from netCDF4 import Dataset

from tools.get_original_data import get_time_data, get_lonlat_data
from tools.osTime import nowTime
from step_function.recognize_typhoon_step1 import recognize_typhoon
from step_function.get_real_Tp_step2 import get_real_Tp
from step_function.remove_noise_step3 import remove_noise
####################### CONSTANTS ##########################
from math_calculate.constants import VOR_FILE_PATH, ALLNUMS_BIGLTMD_BIN_FP, ALLNUMS_BIN_FP, FILE_COMP_FD

def steps_multi(kNumber, poolNum=28, startFromI=0):
    """ 此函数用于多进程执行算法

    Args:
        vor_filePath (str): 输入的相对涡度场nc文件绝对路径（已删除此参数）
        kNumber (str): 表示进行到第几千排列组合
        poolNum (int): 进程数
        startFromI (int): 表示从这1千个任务的第几个开始执行（当startFromI以前的任务已经执行完毕时使用此参数）
    Notes:
        此函数使用Pool进程池来进行多进程任务
    """

    print('This is JRA55_'+kNumber+'k')
    # print('This is JRA55_step3_completion')

    print('Start steps_multi function.'+nowTime())
    print('Parent process is '+str(os.getpid())+'.')

    toK = int(kNumber+'000')
    allNums = loadAllNums()[toK-1000+int(startFromI):toK]

    # allNums = loadStepLack(3)       ### 缺少的文件的编号列表
    # allNums = [loadAllNums()[27652], loadAllNums()[30296]]
    # print(len(allNums))

    time = get_time_data(VOR_FILE_PATH)
    lon, lat = get_lonlat_data(VOR_FILE_PATH)
    VFile = Dataset(VOR_FILE_PATH)
    VFile.set_auto_mask(False)
    vor_allField = VFile.variables['Vorticity'][:,:,:]
    VFile.close()

    p = Pool(int(poolNum))

    for nums in allNums:

        aStr = generateSNum(nums)
        aNum = str(nums[-1]).zfill(5)
        print('start ' + aNum + ' ' + nowTime())

        binF_str = 'JRA55_' + aNum + '_' + aStr + '_79-18'
        
        p.apply_async(threeSteps, args=(True, True, True, time, lon, lat, vor_allField, binF_str, [nums[0],nums[1],nums[2],0,nums[3],nums[4]],aNum,))
        # threeSteps(True, True, True, time, lon, lat, vor_allField, binF_str, [nums[0],nums[1],nums[2],0,nums[3],nums[4]],aNum)

        print('end ' + aNum + ' ' + nowTime())

    print('Waiting for all subprocesses done...'+nowTime())
    p.close()
    p.join()

    print('All subprocesses done.'+nowTime())




def threeSteps(runStep1, runStep2, runStep3, time, lon, lat, vorField_all, binF_str, con_l, numstr):
    """ 此函数为单进程执行算法，con_l为常数的列表

    Args:
        vor_filePath (str): 输入的相对涡度场nc文件绝对路径（已删除此参数）
        runstep1 (bool): 是否执行step1运算的开关
        runStep2 (bool): 是否执行step2运算的开关
        runStep3 (bool): 是否执行step3运算的开关
        binF_str (str): 输出的bin文件的文件名前缀，完整文件名为"binF_str+'_stepX.bin'"
        con_l ([5float, int]): 参数的5个阈值和一个从1开始的int文件索引组成的列表
        numstr (str): 从1开始的文件索引（补零，共5位数）字符串
    """
    print(nowTime())
    print('Run task in function: threeSteps (pid: '+str(os.getpid())+')...')
    assert len(con_l) == 6, '常数列表元素数量不正确！！！！！'

    

    ###################### 第一步：每个时次找台风 ####################
    if runStep1:
        recognize_typhoon(binF_str+'_step1.bin', numstr, time, lon, lat, vorField_all, TMTN=con_l[0], HTMR=con_l[1], TMP=con_l[2], TME=con_l[3], RMR=con_l[4], start_year=1979, end_year=2018, get_max_wnd_10=True, get_max_wnd_all=False)
    ###### 第二步：连接台风。以下一行函数中的 nearest_dist 阈值是经验的 #########
    if runStep2:
        get_real_Tp(binF_str+'_step1.bin', binF_str+'_step2.bin', LTMD=con_l[5])
    ######################## 第三步：去除噪声 ######################
    if runStep3:
        remove_noise(binF_str+'_step2.bin', binF_str+'_step3.bin', remove=True)


def loadAllNums(allNumFilePath):
    """ 此函数用于从bin文件读取数组列表（5个参数，33000个排列组合）

    Returns:
        [33000[5float, int]]: 每个元素为由TMTN、HTMR、TMP、TME、RMR、从1开始的int文件索引组成的列表
    """
    with open(allNumFilePath, 'rb') as save_file:
        allNums = pickle.load(save_file)
    return allNums

def loadStepLack(stepNum):
    """ 此函数用于载入缺少的文件对应的参数组合（文件编号从00001开始）

    Args:
        stepNum (int): 内含缺少文件的索引的bin文件的文件名中表示第几步的int数字
    Returns:
        [33000[5float], int]: 每个元素为由TMTN、HTMR、TMP、TME、RMR、从1开始的int文件索引组成的列表
    """
    with open(FILE_COMP_FD + 'step' + str(stepNum) + 'Lack_bigger.bin', 'rb') as save_file:
        step_lackNumStr = pickle.load(save_file)

    allNums = loadAllNums()
    ### 获取缺少的文件对应的参数组合
    stepLackNums = list(map(lambda x: allNums[int(x)-1], step_lackNumStr))
    return stepLackNums
