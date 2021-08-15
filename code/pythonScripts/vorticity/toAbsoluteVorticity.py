"""_"""
import numpy as np
from netCDF4 import Dataset
from wrf import getvar, ALL_TIMES

class vorConverter:
    """_"""
    def __init__(self):
        self.fileDir = '/mnt/e/University/TC_Tracker/data/wrfFile/'
        self.filePath = self.fileDir + 'wrfout_haimaReport_try1.nc'
        self.outputFilePath = self.fileDir + 'absVor.nc'
        self.ncFile = Dataset(self.filePath)
        self.ncFile.set_always_mask(False)
    
    def copyAttrs(self, varName: str, var):
        """ 此方法从原文件拷贝某变量的参数 """
        inVar = self.ncFile.variables[varName]
        attrNamesList = inVar.ncattrs()
        for attrName in attrNamesList:
            var.setncattr(attrName, inVar.getncattr(attrName))

    def convert(self):
        """_"""
        
        absoluteVor_850hPa = getvar(self.ncFile, 'avo', timeidx=ALL_TIMES).values[:,6,:,:]
        vorShape = absoluteVor_850hPa.shape
        print(self.ncFile.dimensions['Time'].isunlimited())
        print(vorShape)

        
        outNCFile = Dataset(self.outputFilePath, 'w')
        outNCFile.createDimension('south_north', vorShape[1])
        outNCFile.createDimension('west_east', vorShape[2])
        outNCFile.createDimension('Time', None)

        XTIMEVar = outNCFile.createVariable('XTIME', np.float32, ('Time',))
        XLATVar = outNCFile.createVariable('XLAT', np.float32, ('Time','south_north','west_east'))
        XLONGVar = outNCFile.createVariable('XLONG', np.float32, ('Time','south_north','west_east'))
        avoVar = outNCFile.createVariable('avo', np.float32, ('Time','south_north','west_east'))

        self.copyAttrs('XTIME',XTIMEVar)
        self.copyAttrs('XLAT',XLATVar)
        self.copyAttrs('XLONG',XLONGVar)
        avoVar.units = 's^-1'
        # self.copyAttrs('avo',avoVar)

        XTIMEVar[:] = self.ncFile.variables['XTIME'][:]
        XLATVar[:] = self.ncFile.variables['XLAT'][:]
        XLONGVar[:] = self.ncFile.variables['XLONG'][:]
        avoVar[:] = absoluteVor_850hPa
        
        outNCFile.close()

        # del absoluteVor.attrs['projection']
        # absoluteVor.attrs['coordinates'] = 'XLONG XLAT XTIME'
        # absoluteVor = absoluteVor[:,6,:,:]
        # absoluteVor.to_netcdf(path=self.outputFilePath)


if __name__ == '__main__':
    v = vorConverter()
    v.convert()
