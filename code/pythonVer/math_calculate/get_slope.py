"""_"""
def get_slope(lonlat1, lonlat2):
    """ 计算两点斜率，每个点以[x, y]储存在变量中 """
    if lonlat2[0] - lonlat1[0] == 0.0:
        return float("inf")
    return (lonlat2[1] - lonlat1[1]) / (lonlat2[0] - lonlat1[0])
