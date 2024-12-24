# 專案名稱
影像處理的final project

「這是一個使用 UNet 模型進行水域切割的影像處理工具。」

## 環境需求

請先確保安裝以下軟體與套件：

- Python 版本：`3.x`
- 必須的套件：
  - `pytorch`
  - `numpy`
  - `matplotlib`
  - `segmentation_models_pytorch`


## 資料準備

### 輸入資料

- 將訓練影像放置於 `training_dataset/image/` 資料夾中。
- 將訓練影像的mask放置於 `training_dataset/mask/` 資料夾中。
- 將訓練影像的output放置於 `training_dataset/output/` 資料夾中。
- 將測試影像放置於 `testing_dataset/image/` 資料夾中。
- 將測試影像的mask放置於 `training_dataset/mask/` 資料夾中。
- 將測試影像的output放置於 `training_dataset/output/` 資料夾中。

### 資料格式
- 圖片格式應為合法的圖片檔案。
- 標籤應為二值化圖像（黑白）。


# 執行步驟

1 - 訓練model 
 -
 - 打開 unet_main.ipynb
 - 輸入正確的檔案 path (training dataset 和 testing dataset)
 - Run All 

2 - simulation 
 -
 - 打開 simulation.ipynb
 - 輸入正確的檔案 path (training dataset 和 testing dataset) 、存下來權重檔案(ex: unet_25_test1_iou_0632_origin)
 - Run All 

3 - 檢查圖片
  - 
  - 檢查圖片是否都在output 資料夾裡面。