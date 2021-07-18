"""_"""

from math_calculate.point_dist import point_dist

### temp_tp指temp_tps数组中的一个台风，today_tps指今天的所有台风的数组
### today_tps: (Tp_Day)
### temp_tp:   (Typhoon)
def find_nearest_tp(lon, lat, temp_tp, today_tps):  
    """ 此函数找出距离temp_tp最近的台风 """
    nearest_tp = today_tps[0]
    nearest_dist = 800.0          ### 800没有什么意义
    for today_tp in today_tps:
        dist = point_dist(lon, lat, temp_tp.max_pts[-1], today_tp.max_pt)
        if dist < nearest_dist:
            nearest_dist = dist
            nearest_tp = today_tp

    return nearest_tp, nearest_dist

def find_nearest_real_tp(lon, lat, temp_tps, today_tp):
    """ 此函数找出距离today_tp最近的台风，其实可以直接使用上面的函数，但为了意义清楚还是写了这个函数 """
    nearest_tp = temp_tps[0]
    nearest_dist = 800.0           ### 800没有什么意义
    for temp_tp in temp_tps:
        dist = point_dist(lon, lat, temp_tp.max_pts[-1], today_tp.max_pt)
        if dist < nearest_dist:
            nearest_dist = dist
            nearest_tp = temp_tp
    return nearest_tp, nearest_dist
