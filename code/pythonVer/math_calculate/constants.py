""" 此文件记录经验性的参数！！！谨慎修改！ """
import platform

TODAY_MAX_TP_NUM = 5     ### 一天中最多会出现的涡旋数量
HAS_TP_MIN_ReVOR = 8e-5  ### 1.0e-4 8.5e-5 7e-5  ### 查看相对涡度场时认为可能有台风的
                                                ### 最小相对涡度值
TP_MIN_PTS = 4         ### 递归找出“台风”包含的所有空间点，如果点数少于这个值则认为这不是台风
TP_MIN_E = 0.90          ### 认为这个是台风的最大偏心率（偏心率计算方法仍需改进）
RECURSION_MIN_ReVOR = 6e-5    ### 4e-5, 6.9e-05  ### 找出所有点的递归中限定相对涡度的最小值
LINK_TP_MAX_DIST = 340.0   ### 台风在一个时次内走的最远距离，超过此距离认为是两个不同的台风
                            ### （这个最远距离阈值是否可以修改成随着纬度不同而不同？）

CMA_BT_BIN_DIR, JRA55DATA_DIR, JRA55CODE_DIR, STEP_FILE_DIR = '', '', '', ''
if platform.system().lower() == 'windows':
    CMA_BT_BIN_DIR = 'E:/University/NWP_Research/typhoon/typhoon_Info/best_track/'
    JRA55DATA_DIR = 'F:/JRA-55_Data/'
    JRA55CODE_DIR = 'E:/University/NWP_Research/typhoon/code/recognition/JRA-55/'
    ### 每一步函数生成的二进制文件放置的目录
    STEP_FILE_DIR = 'E:/University/NWP_Research/typhoon/typhoon_Info/JRA-55/step_bin_file/'
elif platform.system().lower() == 'linux':
    CMA_BT_BIN_DIR = '/wind1/home/16dengh/myWork/gradu_thesis/NWP_Research/typhoon/typhoon_Info/best_track/'
    JRA55DATA_DIR = '/mnt/e/University/TC_Tracker/data/'
    JRA55CODE_DIR = '/wind1/home/16dengh/myWork/gradu_thesis/NWP_Research/typhoon/code/recognition/JRA-55/'
    ### 每一步函数生成的二进制文件放置的目录
    STEP_FILE_DIR = JRA55DATA_DIR + 'stepFile/'

numKind = 'firstSteps'
DIFFKINDSBIN_DIR = STEP_FILE_DIR + 'diffKindsBin/'
BESTBIN_DIR = STEP_FILE_DIR + 'bestBin/'
DAILY_BESTBIN_DIR = STEP_FILE_DIR + 'daily_bestBin/'
STEP1_FILE_DIR, STEP2_FILE_DIR, STEP3_FILE_DIR = DAILY_BESTBIN_DIR, DAILY_BESTBIN_DIR, DAILY_BESTBIN_DIR
# STEP1_FILE_DIR = DIFFKINDSBIN_DIR + '{0}/{0}_step1/'.format(numKind)
# STEP2_FILE_DIR = DIFFKINDSBIN_DIR + '{0}/{0}_step2/'.format(numKind)
# STEP3_FILE_DIR = DIFFKINDSBIN_DIR + '{0}/{0}_step3/'.format(numKind)

### 涡度场数据的文件绝对路径
# VOR_FILE_PATH = JRA55DATA_DIR + 'generated_daily/Vorticity_JRA55_daily.nc'
# VOR_FILE_PATH = JRA55DATA_DIR + 'generated_hourly/Vorticity_JRA-55_hourly.nc'
VOR_FILE_PATH = JRA55DATA_DIR + 'Vorticity_JRA-55_hourly.nc'
### 离地10m风速数据文件绝对路径
WIND_10M_FP = JRA55DATA_DIR + 'wind/jra55_wind_10m_1979-2018.nc'
### 整层风速数据文件绝对路径
WIND_ALLLEVS_FP = JRA55DATA_DIR + 'wind/jra55_wind_allLev_1979-2018.nc'


### JRA-55 时间分辨率为1天的数据识别出的台风数据bin文件的绝对路径
JRA_1D_PROGRAM_FP = 'E:/University/NWP_Research/typhoon/typhoon_Info/model/steps/step3_1pt5_JRA.bin'

### JMA 最佳路径台风数据bin文件路径
JMA_BEST_TRACK_FP = CMA_BT_BIN_DIR + 'JMA_best_track.bin'
JMA_BT_ONLYNAME_FP = CMA_BT_BIN_DIR + 'JMA_best_track_onlyName.bin'
### CMA 最佳路径台风数据bin文件绝对路径
CMA_BEST_TRACK_FP = CMA_BT_BIN_DIR + 'CMA_best_track.bin'
### CMA无副中心台风bin文件绝对路径
CMA_NOSUB_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_nosub.bin'
### CMA无副中心台风bin文件绝对路径（4种）
CMA_NOSUB_NoY_nameY_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoY_nameY.bin'
CMA_NOSUB_NoN_nameY_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoN_nameY.bin'
CMA_NOSUB_NoY_nameN_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoY_nameN.bin'
CMA_NOSUB_NoN_nameN_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoN_nameN.bin'
### JTWC 最佳路径台风数据bin文件绝对路径
JTWC_BEST_TRACK_FP = CMA_BT_BIN_DIR + 'JTWC_best_track.bin'
### 无副中心台风，有潜在破坏力数组的bin文件（类型：[Tp_bestT]）
CMA_NOSUB_hasPDI_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_nosub_hasPDI.bin'
CMA_NOSUB_NoY_nameY_hasPDI_BIN_FP = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoY_nameY_hasPDI.bin'
CMA_NOSUB_NoN_nameY_hasPDI_BIN_FD = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoN_nameY_hasPDI.bin'
CMA_NOSUB_NoY_nameN_hasPDI_BIN_FD = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoY_nameN_hasPDI.bin'
CMA_NOSUB_NoN_nameN_hasPDI_BIN_FD = CMA_BT_BIN_DIR + 'CMA_bt_4kinds_NoN_nameN_hasPDI.bin'
### 不包含无名无编号、不包含副中心的台风（1032个）
CMA_NOSUB_noNN_BIN_FD = CMA_BT_BIN_DIR + 'CMA_bt_noNN.bin'
### 常数列表文件路径
ALLNUMS_BIN_FP = STEP_FILE_DIR + 'fix_allNums.bin'
ALLNUMS_BIGGER_BIN_FP = STEP_FILE_DIR + 'allNums_bigger.bin'
ALLNUMS_SMALLLTMD_BIN_FP = STEP_FILE_DIR + 'allNums_smallLTMD.bin'
ALLNUMS_BIGLTMD_BIN_FP = STEP_FILE_DIR + 'allNums_bigLTMD.bin'
### file_completion 文件夹
FILE_COMP_FD = JRA55CODE_DIR + 'no_use/file_completion/'
