"""_"""

import matplotlib.pyplot as plt
import numpy as np

def scatter_diagram(points):
    """此函数用于画散点图"""
    nd_pts = np.array(points)
    plt.plot(nd_pts[:,0], nd_pts[:,1], 'ro')
    plt.show()


if __name__ == "__main__":
    # scatter_diagram([])
    pass
