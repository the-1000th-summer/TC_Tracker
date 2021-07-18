"""_"""

class Typhoon():
    """ 此类定义真正的台风 """
    def __init__(self, serial_No, max_pts, mws_10ms, mws_allLss, twoPCenLLs, fourPCenLLs, allPCenterLLs, start_date, end_date):
        self.serial_No = serial_No
        self.max_pts = max_pts   ### 相对涡度最大的点，为一个内含坐标（数组）的数组！！！
        ### 近中心最大风速（单位：m/s）  (numpy)[float]
        self.mws_10ms = mws_10ms
        self.mws_allLss = mws_allLss

        self.twoPCenLLs = twoPCenLLs  ### 台风长轴中心经纬度[ [lon, lat] ]
        self.fourPCenLLs = fourPCenLLs   ### 台风四端点中心经纬度[ [lon, lat] ]
        self.allPCenterLLs = allPCenterLLs    ### 台风几何中心真实经纬度[ [lon, lat] ]
        self.start_date = start_date     ### <class 'datetime.datetime'>
        self.end_date = end_date         ### <class 'datetime.datetime'>
