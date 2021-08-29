"""_"""

class Tp_bestT():
    """ 此类定义 JMA、CMA 最佳路径数据集读取出来的台风 """

    def __init__(self, serial_No, cn_No, name, positions, intensities, max_windSpeeds, mws_10ms, mws_allLss, min_press, start_date, end_date, tp_kind, real_PDI, mws_PDI, mws_allLs_PDI):
        self.serial_No = serial_No     ### DDDD. 格式：年份(4位数)+编号 如 197901
        self.cn_No = cn_No            ### 我国对该热带气旋的编号（EEEE），格式同上
        self.name = name              ### 台风的名字
        ### 相对涡度最大的点，为一个内含经纬度（数组）的数组！！！
        self.positions = positions    ### 经度在前，纬度在后
        self.intensities = intensities    ### 气旋强度
        ### 近中心最大风速（单位：m/s）  (numpy)[float]
        self.max_windSpeeds = max_windSpeeds
        self.mws_10ms = mws_10ms
        self.mws_allLss = mws_allLss
        ### 中心最低气压（单位：hPa）    (numpy)[float]
        self.min_press = min_press
        self.start_date = start_date     ### <class 'datetime.datetime'>
        self.end_date = end_date         ### <class 'datetime.datetime'>
        ### 台风类型('NoY_nameY'、'NoN_nameY'、'NoY_nameN'、'NoN_nameN')
        self.tp_kind = tp_kind
        ### 三种PDI
        self.real_PDI = real_PDI          ### float
        self.mws_PDI = mws_PDI         ### float
        self.mws_allLs_PDI = mws_allLs_PDI     ### float
