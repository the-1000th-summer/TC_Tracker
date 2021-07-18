def generateSNum(nums):
    """ 此函数生成step bin文件中的编号

    Args:
        nums ([5float, 1int]): 阈值参数的一对组合
    Returns:
        str: 参数组合的字符串形式
    """
    out_str = '%d_%.2e_%d_%.2e_%.1f' % (nums[0], nums[1], nums[2], nums[3], nums[4])
    return out_str