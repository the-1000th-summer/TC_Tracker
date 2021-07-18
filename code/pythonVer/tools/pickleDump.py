"""_"""
import pickle

def pickleDump(outBinFilePath, *contentVars):
    """ 此函数使用pickle库保存bin文件。

    Args:
        outBinFilePath (str): 新建的bin文件的绝对路径
        contentVars ((任意类型)): 需要保存的数据
    """
    with open(outBinFilePath ,'wb') as save_file:
        for cVar in contentVars:
            pickle.dump(cVar, save_file)

# if __name__ == "__main__":
#     pickleDump('dd', 'ff', 'as', 'df', 12)
