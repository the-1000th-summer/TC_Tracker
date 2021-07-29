""" 此文件的函数是有关台风运动情况和位置情况的 """
from netCDF4 import Dataset

def always_move_east(points):
    """ 此程序判断“台风”是否一直向东移动 """
    xArr = [ point[0] for point in points ]
    # xBefore = xArr[0]
    for i in range(1,len(xArr)):
        # print(xArr[i] - xArr[i-1])
        if xArr[i] - xArr[i-1] < 0:
            return False
    return True

def always_move_west(points):
    """ 此程序判断“台风”是否一直向西移动 """
    xArr = []
    for point in points:
        xArr.append(point[0])
    # xBefore = xArr[0]
    for i in range(1,len(xArr)):
        if xArr[i] - xArr[i-1] >= 0:
            return False
    if points[-1][1] <= points[0][1]:
        return False
    return True

def is_on_land(point):
    """ 此程序识别一个点是否在陆地上。
    直接使用Basemap的判断陆海函数，不再使用landmask，此函数弃用。
    
    Args:
        point ([[int, int]]): 点
    Returns:
        bool: 是否在陆地上
    """
    lm_file = Dataset('E:/University/NWPdata/typhoon/generated/recognition/DAILY_part_original/landmask/landmask.nc')   
    lm_file.set_auto_mask(False)
    landmask = lm_file.variables['landmask'][:,:]
    lm_file.close()
    
    on_land_num = landmask[point[1], point[0]]
    if on_land_num == 1:             ### 注意坐标变换！！！
        return True
    elif on_land_num == 0:
        return False
    else:
        raise Exception('is_on_land KNOWN!!!')


def all_on_land(points):
    """ 判断是否所有的点都在陆地上 """
    for point in points:
        if not is_on_land(point):
            return False
    return True
