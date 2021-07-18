"""_"""
class Tp_Day():
    """ 此类定义每日的台风，一个台风会被重复地创建对象 """

    def __init__(self, max_pt, mws_10m, mws_allLs, twoPCenterLL, fourPCenterLL, allPCenterLL, date):
        # self.serial_No = serial_No
        self.max_pt = max_pt      ### 最大相对涡度的坐标
        ### 近中心最大风速（单位：m/s）  (numpy) float
        self.mws_10m = mws_10m          ### 10m
        self.mws_allLs = mws_allLs      ### 所有层

        self.twoPCenterLL = twoPCenterLL   ### 台风长轴中心真实经纬度[lon, lat]
        self.fourPCenterLL = fourPCenterLL  ### 台风四点中心真实经纬度[lon, lat]
        self.allPCenterLL = allPCenterLL   ### 台风几何中心真实经纬度[lon, lat]
        self.date = date          ### 日期，类型为 <class 'datetime.datetime'>
