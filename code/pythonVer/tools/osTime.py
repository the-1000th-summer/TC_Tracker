"""_"""
from datetime import datetime

def nowTime():
    """ 此函数用于返回字符串的现在时间 """
    return 'Now the time is: '+datetime.now().strftime('%Y.%m.%d %H:%M')+'. '
