from math import dist
import numpy as np
import pickle
import matplotlib.pyplot as plt
from tp_class.Tp_bestT import Tp_bestT
from calcDegree import calcDegree, point_dist

def try1(pklFilePath, label):
    """_"""

    with open(pklFilePath, 'rb') as pklFile:
        allTCs = pickle.load(pklFile)
    # distances = []
    distAvgs = []
    degrees = []
    lats = []
    
    for tc in allTCs:
        tcPos = tc.positions
        for posI in range(1, len(tcPos)-1):
            lon1, lat1 = tcPos[posI-1]
            lon2, lat2 = tcPos[posI]
            lon3, lat3 = tcPos[posI+1]
            distAvgs.append((point_dist(lat1,lon1,lat2,lon2) + point_dist(lat2,lon2,lat3,lon3)) / 2)
            # distAvgs.append((point_dist(lat1,lon1,lat2,lon2)))
            degrees.append(calcDegree((lat1,lon1),(lat2,lon2),(lat3,lon3)))
            lats.append(lat2)
    
    print(len(distAvgs))
    plt.scatter(lats, degrees, s=3, label=label+': {} points'.format(len(distAvgs)))
    
    

if __name__ == '__main__':
    pklFileDir = '/mnt/e/University/NWP_Research/typhoon/typhoon_Info/best_track/'
    pklFilePath1 = pklFileDir + 'CMA_bt_4kinds_NoY_nameY.bin'
    pklFilePath2 = pklFileDir + 'JMA_best_track_onlyName.bin'
    fig, ax = plt.subplots()
    try1(pklFilePath1, "CMA")
    try1(pklFilePath2, 'JMA')
    plt.ylabel('lat (degree)', fontsize=20)
    plt.xlabel('TC velocity (km/6 hours)', fontsize=20)
    plt.xticks(fontsize=25)
    plt.yticks(fontsize=25)
    ax.yaxis.offsetText.set_fontsize(20)
    plt.legend(loc='lower right', prop={'size': 15})
    plt.show()
