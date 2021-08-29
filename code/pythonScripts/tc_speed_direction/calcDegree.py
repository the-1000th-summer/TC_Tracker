import math
import numpy as np

def latlong_to_3d(latr, lonr):
    """Convert a point given latitude and longitude in radians to
    3-dimensional space, assuming a sphere radius of one."""
    return np.array((
        math.cos(latr) * math.cos(lonr),
        math.cos(latr) * math.sin(lonr),
        math.sin(latr)
    ))

def angle_between_vectors_degrees(u, v):
    """Return the angle between two vectors in any dimension space,
    in degrees."""
    temp = np.dot(u, v) / (np.linalg.norm(u) * np.linalg.norm(v))
    if temp < -1:
        temp = -1
    if temp > 1:
        temp = 1
    return np.degrees(math.acos(temp))



def calcDegree(A, B, C):
    # Convert the points to numpy latitude/longitude radians space
    # if (B[0]-A[0]) == (C[0]-B[0]) and (B[1]-A[1]) == (C[1]-B[1]):
    #     return 180
    a = np.radians(np.array(A))
    b = np.radians(np.array(B))
    c = np.radians(np.array(C))

    # Vectors in latitude/longitude space
    avec = a - b
    cvec = c - b

    # Adjust vectors for changed longitude scale at given latitude into 2D space
    lat = b[0]
    avec[1] *= math.cos(lat)
    cvec[1] *= math.cos(lat)

    # Find the angle between the vectors in 2D space
    angle2deg = angle_between_vectors_degrees(avec, cvec)

    return angle2deg
    # Print the results
    # print('\nThe angle ABC in 2D space in degrees:', angle2deg)


def point_dist(lat1, lon1, lat2, lon2):
    """ 此函数计算返回两个点的真实距离(km)（大圆距离）(haversine公式)。

    Args:
        lon ([float]): 数据格点的经度数组
        lat ([float]): 数据格点的纬度数组
        pt1 ([int, int]): 点
        pt2 ([int, int]): 另一个点
    Returns:
        float: 两点之间的距离，单位为km
    """

    # print(lon1, lat1, lon2, lat2)
    lon1, lat1, lon2, lat2 = map(math.radians, [lon1, lat1, lon2, lat2])

    # haversine公式
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = math.sin(dlat/2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon/2)**2
    c = 2 * math.asin(math.sqrt(a)) 
    r = 6371 # 地球平均半径，单位为公里
    return c * r

if __name__ == '__main__':
    # The points in tuple latitude/longitude degrees space
    # A = (12.92473, 77.6183)
    # B = (12.92512, 77.61923)
    # C = (12.92541, 77.61985)
    A = (12.2, 141.5)
    B = (12.3, 140.3)
    C = (12.4, 139.2)
    print(calcDegree(A,B,C))
