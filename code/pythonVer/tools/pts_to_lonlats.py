"""_"""
import numpy as np

def pts_to_lonlats(lon, lat, points):
    """ 此程序将点的坐标转换为实际的地球坐标 """

    np_points = np.array(points)
    lonlat = np.zeros(np_points.shape)      ### 形状：点数(行)×2列
    lonlat[:,0] = np.array(lon)[np_points[:,0]]    ### 经度放数组左边
    lonlat[:,1] = np.array(lat)[np_points[:,1]]    ### 纬度放数组右边

    return lonlat

# if __name__ == "__main__":
    # pts_to_latlons(np.array([[1,2],[3,4],[5,6]]))
    # pts_to_latlons([[1,2],[3,4],[5,6]])
    # pass
