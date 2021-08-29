""" 此文件存储常数 """

cma_txt_dir = 'E:/University/NWPdata/best_track_data/CMA/'
CMA_NO1978_FD = cma_txt_dir + 'bst_CMA_1979-2018.txt'  ### 头行未整理，时间不均匀
### 头行整理，时间均匀(6h)，所有台风（包括副中心）（1208个）
CMA_MODIFY_FD = cma_txt_dir + 'bst_CMA_1979-2018_modify.txt'
### 仅头行组成的文件，所有台风（包括副中心）（1208个）
CMA_HEADLINE_FD = cma_txt_dir + 'bst_CMA_1979-2018_headline.txt'
### 仅副中心（15个）
CMA_SUBCENTER_FD = cma_txt_dir + 'bst_CMA_1979-2018_subcenter.txt'
### 仅副中心的主中心（14个）
CMA_MASTER_FD = cma_txt_dir + 'bst_CMA_1979-2018_master.txt'
### 除去副中心的所有台风（1193个）
CMA_NOSUB_FD = cma_txt_dir + 'bst_CMA_1979-2018_nosub.txt'
### 除去副中心的所有台风的头行（1193个）
CMA_NOSUB_HL_FD = cma_txt_dir + 'bst_CMA_1979-2018_nosub_hl.txt'
### 4种台风（不包含副中心）（1193个）
CMA_NOSUB_NoY_nameY_FD = cma_txt_dir + 'bst_CMA_1979-2018_4kinds_NoY_nameY.txt'
CMA_NOSUB_NoN_nameY_FD = cma_txt_dir + 'bst_CMA_1979-2018_4kinds_NoN_nameY.txt'
CMA_NOSUB_NoY_nameN_FD = cma_txt_dir + 'bst_CMA_1979-2018_4kinds_NoY_nameN.txt'
CMA_NOSUB_NoN_nameN_FD = cma_txt_dir + 'bst_CMA_1979-2018_4kinds_NoN_nameN.txt'
### 不包含无名无编号的台风（1032个）
CMA_NOSUB_noNN_FD = cma_txt_dir + 'bst_CMA_1979-2018_noNN.txt'

### 二进制文件
CMA_BIN_DIR = 'E:/University/NWP_Research/typhoon/typhoon_Info/best_track/'
CMA_BIN_FD = CMA_BIN_DIR + 'CMA_best_track.bin'
CMA_MODI_BIN_FD = CMA_BIN_DIR + 'CMA_bt_modify.bin'
CMA_SUBC_BIN_FD = CMA_BIN_DIR + 'CMA_bt_subcenter.bin'
CMA_MAST_BIN_FD = CMA_BIN_DIR + 'CMA_bt_master.bin'
CMA_NOSUB_BIN_FD = CMA_BIN_DIR + 'CMA_bt_nosub.bin'

CMA_NOSUB_NoY_nameY_BIN_FD = CMA_BIN_DIR + 'CMA_bt_4kinds_NoY_nameY.bin'
CMA_NOSUB_NoN_nameY_BIN_FD = CMA_BIN_DIR + 'CMA_bt_4kinds_NoN_nameY.bin'
CMA_NOSUB_NoY_nameN_BIN_FD = CMA_BIN_DIR + 'CMA_bt_4kinds_NoY_nameN.bin'
CMA_NOSUB_NoN_nameN_BIN_FD = CMA_BIN_DIR + 'CMA_bt_4kinds_NoN_nameN.bin'

CMA_NOSUB_noNN_BIN_FD = CMA_BIN_DIR + 'CMA_bt_noNN.bin'
