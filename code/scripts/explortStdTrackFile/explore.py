""" 此文件用于探索IBTrACS台风路径数据nc文件 """

from netCDF4 import Dataset

def explore():
    """_"""
    filePath = '/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/IBTrACS.WP.v04r00.nc'
    ncFile = Dataset(filePath)
    ncFile.set_auto_mask(False)

    # lat
    tlon = ncFile.variables['tokyo_lon']
    print(tlon[:][4003])
    lon = ncFile.variables['lon']
    print(lon[:][4003])

    # pressure (tokyo)
    # pres = ncFile.variables['tokyo_pres']
    # print(pres[:][4000])

    # t id
    # sid = ncFile.variables['sid']
    # print(sid[:][:10])

if __name__ == '__main__':
    explore()
