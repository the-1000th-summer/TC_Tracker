import numpy as np
from netCDF4 import Dataset

class ext:
    """_"""

    def __init__(self):
        self.fileDir = '/wind1/home/16dengh/wrf/domain/haimaReport/try1/chem_onlySo2_Persist/'
        self.filePath = self.fileDir + 'wrfout_d01_2016-10-19_00:00:00'
        self.outputFilePath = self.filePath + '_minForVor.nc'
        self.inFile = Dataset(self.filePath)
        self.inFile.set_always_mask(False)

    def copyAttrs(self, varName: str, var):
        """ 此方法从原文件拷贝某变量的参数 """
        inVar = self.inFile.variables[varName]
        attrNamesList = inVar.ncattrs()
        for attrName in attrNamesList:
            var.setncattr(attrName, inVar.getncattr(attrName))


    def excract(self):
        """ 此文件提取出计算相对涡度的所需变量数据 """

        DXAtt = self.inFile.DX
        DYAtt = self.inFile.DY
        MAPFAC_M_var = self.inFile.variables['MAPFAC_M']
        vorShape = MAPFAC_M_var[:].shape
        print(MAPFAC_M_var)
        

        outFile = Dataset(self.outputFilePath, 'w')
        outFile.DX = DXAtt
        outFile.DY = DYAtt
        outFile.createDimension('south_north', vorShape[1])
        outFile.createDimension('south_north_stag', vorShape[1]+1)
        outFile.createDimension('west_east', vorShape[2])
        outFile.createDimension('west_east_stag', vorShape[2]+1)
        outFile.createDimension('bottom_top', 1)                  # !!!!!!
        outFile.createDimension('Time', None)

        XTIMEVar = outFile.createVariable('XTIME', np.float32, ('Time',))
        XLATVar = outFile.createVariable('XLAT', np.float32, ('Time','south_north','west_east'))
        XLONGVar = outFile.createVariable('XLONG', np.float32, ('Time','south_north','west_east'))
        MAPFAC_MVar = outFile.createVariable('MAPFAC_M', np.float32, ('Time','south_north','west_east'))
        MAPFAC_UVar = outFile.createVariable('MAPFAC_U', np.float32, ('Time','south_north','west_east_stag'))
        MAPFAC_VVar = outFile.createVariable('MAPFAC_V', np.float32, ('Time','south_north_stag','west_east'))
        UVar = outFile.createVariable('U', np.float32, ('Time','bottom_top','south_north','west_east_stag'))
        VVar = outFile.createVariable('V', np.float32, ('Time','bottom_top','south_north_stag','west_east'))

        self.copyAttrs('XTIME',XTIMEVar)
        self.copyAttrs('XLAT',XLATVar)
        self.copyAttrs('XLONG',XLONGVar)
        self.copyAttrs('MAPFAC_M',MAPFAC_MVar)
        self.copyAttrs('MAPFAC_U',MAPFAC_UVar)
        self.copyAttrs('MAPFAC_V',MAPFAC_VVar)
        self.copyAttrs('U',UVar)
        self.copyAttrs('V',VVar)

        XTIMEVar[:] = self.inFile.variables['XTIME'][:]
        XLATVar[:] = self.inFile.variables['XLAT'][:]
        XLONGVar[:] = self.inFile.variables['XLONG'][:]
        MAPFAC_MVar[:] = self.inFile.variables['MAPFAC_M'][:]
        MAPFAC_UVar[:] = self.inFile.variables['MAPFAC_U'][:]
        MAPFAC_VVar[:] = self.inFile.variables['MAPFAC_V'][:]
        UVar[:] = self.inFile.variables['U'][:][:,6,:,:]
        VVar[:] = self.inFile.variables['V'][:][:,6,:,:]

        self.inFile.close()
        outFile.close()


if __name__ == '__main__':
    e = ext()
    e.excract()
