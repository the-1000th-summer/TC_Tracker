""" 此模块用于获取台风中的导入数据。 """

from netCDF4 import Dataset, num2date

def get_time_data(filePath):
    """ 此程序接受一个文件名，返回文件中时间变量列表。

    Args:
        filePath (str): 文件绝对路径
    Returns:
        [datetime.datetime]: 文件中时间变量列表
    """
    import_file = Dataset(filePath)
    import_file.set_auto_mask(False)

    time = import_file.variables['time']    # note that we do not cast to numpy array yet
    time_convert = num2date(time[:], time.units)    ### calendar类型是default/gregorian
    import_file.close()
    return time_convert.tolist()

def get_lonlat_data(filePath):
    """ 此函数接受一个文件名，返回该nc文件中的经纬度数据列表。

    Args:
        filePath (str): 文件绝对路径
    Returns:
        [float], [float]: 文件中经纬度list数据
    """
    import_file = Dataset(filePath)
    import_file.set_auto_mask(False)

    lon = import_file.variables['lon'][:].tolist()
    lat = import_file.variables['lat'][:].tolist()
    import_file.close()
    return lon, lat

def get_vor_field(filePath, vor_index):
    """ 此函数接受一个文件名，返回一个二维的涡度场

    Args:
        filePath (str): 文件绝对路径
        vor_index (int): 文件中第几个时次
    Returns:
        numpy.ndarray[numpy.float32,numpy.float32]: 二维的涡度场
    Notes:
        假设文件只有一个高度层
    """
    import_file = Dataset(filePath)
    import_file.set_auto_mask(False)
    vor_array = import_file.variables['Vorticity'][vor_index,:,:]
    import_file.close()
    return vor_array


if __name__ == "__main__":
    vor_filePath = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
    # time_convert = get_time_data(vor_file_dir)
    # print(type(time_convert[0]))
    vorF = get_vor_field(vor_filePath, 0)
    print(type(vorF[0,0]))
