""" 此文件画出相对涡度分布图 """
import numpy as np
import matplotlib.pyplot as plt
from netCDF4 import Dataset

class drawer:
    """ 此类画出相对涡度分布图 """

    def __init__(self) -> None:
        self.ncFileDir = '/mnt/e/University/TC_Tracker/data/'
        self.ncFilePath = self.ncFileDir + 'wrfFile/relVor_cxx.nc'
        # self.ncFilePath = self.ncFileDir + 'wrfFile/relVor_9km.nc'
        # self.ncFilePath = self.ncFileDir + 'Vorticity_JRA-55_hourly.nc'

    def draw(self):
        """_"""
        ncFile = Dataset(self.ncFilePath)
        vorData = ncFile.variables['avo'][:]
        # vorData = ncFile.variables['Vorticity'][:]
        vor1d = vorData.ravel()
        print(vor1d)
        print('max value:', np.max(vor1d))
        print('min value:', np.min(vor1d))
        print('min avg value', np.average(np.min(vorData, axis=(1,2))))
        plt.hist(vor1d, bins=10000)
        plt.show()

if __name__ == '__main__':
    d = drawer()
    d.draw()
    # a = np.array([[[1, 2, 3],[4, 5, 6]],[[7,8,9],[10,11,12]]])
    # print(np.min(a, axis=(1,2)))