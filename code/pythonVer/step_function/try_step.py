"""_"""
from tools.try_tools import try_tools

def try_step(canshu1, canshu2=None, canshu3=None):
    """_"""
    print('this is try_step!')
    if canshu2 == None or canshu3 == None:
        print('NONE!')
    try_tools()

if __name__ == "__main__":
    try_step('fd')
