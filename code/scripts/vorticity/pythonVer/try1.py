import numpy as np
from netCDF4 import Dataset

def try1():
    """_"""
    fileDir = '/mnt/e/University/TC_Tracker/data/wrfFile/'
    filePath = fileDir + 'wrfout_d01_2016-10-19_00_00_00.nc'

    f = Dataset(filePath)
    lenDim = f.variables['Times']
    print(lenDim[:])



if __name__ == '__main__':
    try1()
