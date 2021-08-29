""" 此文件画出相对涡度分布图 """
import numpy as np
import matplotlib.pyplot as plt
from netCDF4 import Dataset

class drawer:
    """ 此类画出相对涡度分布图 """

    def __init__(self) -> None:
        pass

    def draw(self, filePath):
        """_"""
        ncFile = Dataset(filePath)
        vorData = ncFile.variables['avo'][:]
        # vorData = ncFile.variables['Vorticity'][:]
        vor1d = vorData.ravel()
        # vor1d = np.array([216,251,613,680,421,412,397,299,435,420,493,365,239,561,341,633,408,148,567,399,358,171,545,363,528,384,777,569,417,241,469,250,237,465,421,437,362,513,236,337,350,253,417,564,484,328])
        ncFile.close()
        print('max value:', np.max(vor1d))
        print('min value:', np.min(vor1d))
        print('min avg value', np.average(np.min(vorData, axis=(1,2))))

        pointNum = len(vor1d)
        # print('num', pointNum)
        xAvg = np.average(vor1d)
        x_s = np.std(vor1d)
        xMxAvg = vor1d - xAvg
        g1 = np.sqrt(1/(6*pointNum)) * np.sum((xMxAvg / x_s)**3)
        g2 = np.sqrt(pointNum/24) * ((1/pointNum) * np.sum((xMxAvg / x_s)**4) - 3)
        print('g1:', g1, 'g2:', g2)

        plt.xticks(fontsize=25)
        plt.yticks(fontsize=25)
        plt.ylim(0, 200)
        plt.xlabel('Relative Vorticity',fontsize=20)
        plt.ylabel('Frequency Number',fontsize=20)
        plt.hist(vor1d, bins=10000, density=True)

        

if __name__ == '__main__':
    ncFileDir = '/mnt/e/University/TC_Tracker/data/'
    ncFilePath = ncFileDir + 'wrfFile/relVor_cxx.nc'
    ncFilePath2 = ncFileDir + 'wrfFile/relVor_9km.nc'
    #ncFilePath = self.ncFileDir + 'Vorticity_JRA-55_hourly.nc'

    d = drawer()
    d.draw(ncFilePath)
    # d.draw(ncFilePath2)
    plt.show()

    # a = np.array([[[1, 2, 3],[4, 5, 6]],[[7,8,9],[10,11,12]]])
    # f = np.array([56,1, 2, 3, 4, 5, 6, 7,8,9,10,11,12])
    # ff = np.sort(f)
    # print(ff)
    # print(a**3)