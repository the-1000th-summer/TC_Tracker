""" 此文件用于探索IBTrACS台风路径数据nc文件 """

from netCDF4 import Dataset

def explore():
    """_"""
    fileDir = '/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/'
    filePath = fileDir + 'IBTrACS.WP.v04r00.nc'
    # filePath = fileDir + 'minForVor_track.nc'
    ncFile = Dataset(filePath)
    # ncFile.set_auto_mask(False)

    # print(ncFile.dimensions['date_time'].isunlimited())
    print(ncFile.variables['tokyo_pres'].filters().get('complevel', False))
    # print(ncFile.variables['serialNo'][:])

    # lat
    # tlon = ncFile.variables['tokyo_lon']
    # print(tlon[:][4003])
    # lon = ncFile.variables['lon']
    # print(lon[:][4003])

    # pressure (tokyo)
    # pres = ncFile.variables['tokyo_pres']
    # print(pres[:][4000])

    # t id
    # sid = ncFile.variables['sid']
    # print(sid[:][:10])

if __name__ == '__main__':
    explore()
