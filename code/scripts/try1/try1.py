"""_"""

from netCDF4 import Dataset

def try1():
    """_"""
    filePath = '/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/IBTrACS/IBTrACS.WP.v04r00.nc'
    f = Dataset(filePath)
    f.set_auto_mask(False)
    a = f.variables['tokyo_lon'][:][1999]
    print(a)

if __name__ == "__main__":
    try1()
