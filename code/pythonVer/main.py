""" 注意JRA-55日历类型是真实的！！！ """
# import sys
# from step_function.try_step import try_step
# from no_use.file_completion.createNumList import createNumList
# from step_function.steps_multi import threeSteps, steps_multi
from step_function.steps_notMulti import steps_notMulti
# from step_function.tp_track_diagram_step4 import tp_track_diagram

# from other_plots.tp_num_plot import tp_num_plot
# from other_plots.tp_num_plot_nc import tp_num_plot_nc
# from other_plots.plot_eachY_tp import plot_eachY_tp
# from other_plots.plot_1980_tp_nc import plot_1980_tp_nc
# from other_plots.plot_eachTp_inYear import plot_eachTp_inYear

# from other_plots.know_max_wind_speed import know_max_wind_speed
# from other_plots.fourKindsEnergy import fourKindsEnergy
# from other_plots.drawPDI import drawPDI
# from other_plots.drawMaxWS import drawMaxWS
# from other_plots.logistic_regression import logistic_regression
# from other_plots.logistic_regression_complicated import logistic_regression_complicated

# from no_use.print_tp_vormax import print_tp_vormax
# from tools.find_nearest_grid import find_nearest_grid
# from no_use.file_completion.getAllNumsCorrs import getAllNumsCorrs
# from no_use.file_completion.dealCorrs import dealCorrs

# from no_use.file_completion.corr_to_nc import corr_to_nc

# from bestT_supplement.add_grade import add_grade

####################### CONSTANTS ##########################
# from math_calculate.constants import VOR_FILE_PATH
### 所有台风（不包括副中心），有无PDI的文件的路径
# from math_calculate.constants import CMA_NOSUB_BIN_FP, CMA_NOSUB_hasPDI_BIN_FP
### 四种台风列表文件（不包括PDI）的路径
# from math_calculate.constants import CMA_NOSUB_NoY_nameY_BIN_FP, CMA_NOSUB_NoN_nameY_BIN_FP, CMA_NOSUB_NoY_nameN_BIN_FP, CMA_NOSUB_NoN_nameN_BIN_FP
### 四种台风列表文件（包括PDI）的路径
# from math_calculate.constants import CMA_NOSUB_NoY_nameY_hasPDI_BIN_FP, CMA_NOSUB_NoN_nameY_hasPDI_BIN_FD, CMA_NOSUB_NoY_nameN_hasPDI_BIN_FD, CMA_NOSUB_NoN_nameN_hasPDI_BIN_FD
# import tp_class.Tp_Day, tp_class.Typhoon

def main():
    """ 此程序为总程序，从这里开始！
    Args:
        vor_filePath (str): 涡度场nc文件的绝对路径（此参数已删除）
    """

    # binF_str = 'JRA-55'
    # binF_str = 'tryInLinux2_JRA_55-1979-2018'

    # createNumList()
    # threeSteps(vor_filePath, binF_str, [0,0,0,0,0,0])

    # if sys.argv.__len__() == 2:
    #     steps_multi(sys.argv[1])
    # elif sys.argv.__len__() == 3:
    #     steps_multi(sys.argv[1], sys.argv[2])
    # elif sys.argv.__len__() == 4:
    #     steps_multi(sys.argv[1], sys.argv[2], sys.argv[3])
    # else:
    #     raise ValueError('参数个数不正确！')
    print('startttt!')
    steps_notMulti()

    # getAllNumsCorrs()
    # dealCorrs()             ### HAS_TP_MIN_ReVOR参数需要再往大了试！！！
    # corr_to_nc()

    ########################### 第四步：画图 ##########################
    # tp_track_diagram(vor_filePath, binF_str+'_step3.bin')

    # plot_eachY_tp('JRA55_firstSteps_24263_5_8.00e-05_4_6.00e-05_340.0_79-18_step3.bin', drawStep3BinData=True, drawBTData=True)
    # plot_eachY_tp('JRA55_510.0_5_8.00e-05_4_6.00e-05_510.0_79-18_step3.bin', drawStep3BinData=True, drawBTData=True)
    # plot_1980_tp_nc('JRA55_firstSteps_24263_5_8.00e-05_4_6.00e-05_340.0_79-18_step3.bin')

    # plot_eachTp_inYear('JRA55_firstSteps_24263_5_8.00e-05_4_6.00e-05_340.0_79-18_step3.bin', '1979')

    # tp_num_plot('JRA55_firstSteps_24263_5_8.00e-05_4_6.00e-05_340.0_79-18_step3.bin')
    # tp_num_plot('JRA55_00340_5_8.00e-05_4_6.00e-05_340.0_79-18_step3.bin')

    # tp_num_plot_nc('JRA55_firstSteps_24263_5_8.00e-05_4_6.00e-05_340.0_79-18_step3.bin')

    # print_tp_vormax(1)
    # know_max_wind_speed(1)
    # for i in range(11,20):
        # fourKinds(i)
    # fourKindsEnergy(CMA_NOSUB_BIN_FP, CMA_NOSUB_hasPDI_BIN_FP)
    # tp_4kinds_list = CMA_NOSUB_NoY_nameY_BIN_FP, CMA_NOSUB_NoN_nameY_BIN_FP, CMA_NOSUB_NoY_nameN_BIN_FP, CMA_NOSUB_NoN_nameN_BIN_FP
    # tp_4kinds_pdi_list = CMA_NOSUB_NoY_nameY_hasPDI_BIN_FP, CMA_NOSUB_NoN_nameY_hasPDI_BIN_FD, CMA_NOSUB_NoY_nameN_hasPDI_BIN_FD, CMA_NOSUB_NoN_nameN_hasPDI_BIN_FD
    # for i, j in zip(tp_4kinds_list, tp_4kinds_pdi_list):
    #     fourKindsEnergy(i, j)

    # find_nearest_grid([210, 20.0])
    # drawPDI()
    # drawMaxWS()
    # logistic_regression()
    # logistic_regression_complicated()
    # add_grade()

if __name__ == "__main__":
    # main(VOR_FILE_PATH)
    main()
