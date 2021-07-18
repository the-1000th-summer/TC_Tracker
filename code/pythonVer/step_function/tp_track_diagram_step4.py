"""_"""
import pickle
import numpy as np
import matplotlib.pyplot as plt
# import matplotlib
from matplotlib.font_manager import FontProperties
from mpl_toolkits.basemap import Basemap
from tools.get_original_data import get_lonlat_data
from math_calculate.constants import STEP_FILE_DIR

def tp_track_diagram(o_filePath, i_binFileName):
    """ 第四步：画图。

    Args:
        o_filePath (str): 仅用于获取其经纬度list的nc文件的绝对路径
        i_binFileName (str): 第三步生成的bin文件的文件名
    """
    simsun = FontProperties(fname=r'C:\Windows\Fonts\simsun.ttc', size=18) # 宋体
    roman = FontProperties(fname=r'C:\Windows\Fonts\times.ttf', size=20)  # Times
    plt.rcParams["font.family"] = "Times New Roman"
    plt.rcParams['figure.figsize'] = (16.0, 9.0)
    # with open('E:/University/NWP_Research/typhoon/typhoon_Info/lonlat.bin','rb') as lonlat_file:
        # lon = pickle.load(lonlat_file)
        # lat = pickle.load(lonlat_file)

    lon, lat = get_lonlat_data(o_filePath)

    ### 读取第3步生成的二进制文件
    with open(STEP_FILE_DIR + i_binFileName,'rb') as real_tp_file:
        real_tps = pickle.load(real_tp_file)

    print('台风数量: ', len(real_tps))
    # real_tps = real_tps[10:]
    print('第四步(tp_track_diagram): 导入文件成功，开始画图。')

    m = Basemap(projection='cyl',lon_0=0.,lat_0=0.,lat_ts=0.,\
                llcrnrlat=-5.0,urcrnrlat=45.0,\
                llcrnrlon=90.0,urcrnrlon=200.0,\
                rsphere=6371200.,resolution='l',area_thresh=10000)
    m.drawcoastlines()
    m.drawcountries()

    parallels = np.arange(-5.,46.,5.)
    m.drawparallels(parallels,labels=[1,0,0,0], dashes=[5, 1200], fontsize=25)
    # draw meridians
    meridians = np.arange(90.,201.,10.)
    m.drawmeridians(meridians,labels=[0,0,0,1], dashes=[5, 500], fontsize=25)


    for tp_i, tp in enumerate(real_tps):
        # print(aTp.end_date)
        x, y, legend_time = [], [], ''
        # for i in tp.max_pts:
        #     x.append(lon[i[0]])
        #     y.append(lat[i[1]])
        for i in tp.allPCenterLLs:   ########## centerLonlats allPCenterLLs
            x.append(i[0])
            y.append(i[1])

        legend_time = tp.start_date.strftime('%m-%d')

        # mark = '.' if tp_i == 0 else ''
        lineColor, lineLabel = '', ''
        if y[-1] > 35:
            lineColor = 'darkgreen'
            lineLabel = 'North'
        elif y[-1] <= 35 and y[-1] > 23:
            lineColor = 'navy'
            lineLabel = 'Center'
        else:
            lineColor = 'darkred'
            lineLabel = 'South'
        plt.plot(x[0], y[0], marker='.', color=lineColor)
        # plt.plot(x, y, marker='', color=lineColor, label=str(tp_i) + ': ' + legend_time)
        # plt.plot(x[:-1], y[:-1], marker='', color=lineColor)
        plt.plot(x, y, marker='')#, color=lineColor)
        plt.plot(x[-1], y[-1], marker='x', color=lineColor)
        plt.annotate(legend_time,xy=(x[0], y[0]))
        # plt.arrow(x[-2],y[-2],x[-1]-x[-2],y[-1]-y[-2],color=lineColor,width=2e-3)
        # plt.annotate('',xy=(x[-2],y[-2]),xytext=(x[-1],y[-1]),arrowprops=dict(arrowstyle="<-"))
    ### 画图注
    plt.plot(0,0,color='darkgreen',label='North ( > 35°N )',)
    plt.plot(0,0,color='navy',label='Center ( 23°N ~ 35°N )')
    plt.plot(0,0,color='darkred',label='South ( < 23°N )')

    # plt.legend(loc='lower right',prop=roman)
    ### 相对涡度极大值位置 台风长轴两端点中心 台风长短轴四端点中心 台风几何中心
    plt.title('JRA-55：1980年台风路径集合（台风四端点中心）',y=1.05, fontdict={'family':'simhei', 'size':40})

    # plt.plot([100,110], [15, 20])

    plt.show()
    # plt.savefig('C:/Users/Bill/Desktop/twoPs.pdf')

# if __name__ == "__main__":
#     vor_file_dir = 'F:/JRA-55_Data/generated_hourly/Vorticity_JRA-55_hourly.nc'
#     tp_track_diagram(vor_file_dir, 'JRA-55_step3.bin')
