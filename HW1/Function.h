#ifndef FLIP
#define FLIP  // 定義這個宏
#include<stdio.h>
#include<stdlib.h>
typedef struct _BITMAPFILEHEADER{ // (14 Bytes)
    unsigned short bfType;       // 文件類型，應該是 0x4D42 ("BM") (2 Bytes)
    unsigned int bfSize;         // 文件大小 (4 Bytes)
    unsigned short bfReserved1;  // 保留字，必須為 0 (2 Bytes)
    unsigned short bfReserved2;  // 保留字，必須為 0 (2 Bytes)
    unsigned int bfOffBits;      // 到像素數據的偏移量 (4 Bytes)
}__attribute__((packed))BITMAPFILEHEADER; // struct 內容是緊密地放在一起
typedef struct _BITMAPINFOHEADER{ // 40 Bytes
    unsigned int biSize;         // 此結構的大小 (4 Bytes)
    int biWidth;                 // 圖像的寬度 (4 Bytes)
    int biHeight;                // 圖像的高度 (4 Bytes)
    unsigned short biPlanes;     // 色彩平面數，必須為 1 (2 Bytes)
    unsigned short biBitCount;   // 每個像素的位數 (ex，24 = 24 bits = RGB) (2 Bytes)
    unsigned int biCompression;  // 壓縮類型，0 = 無壓縮 (4 Bytes)
    unsigned int biSizeImage;    // 圖像大小 (4 Bytes)
    int biXPelsPerMeter;         // X 軸像素每米 (4 Bytes)
    int biYPelsPerMeter;         // Y 軸像素每米 (4 Bytes)
    unsigned int biClrUsed;      // 多少顏色索引 (4 Bytes)
    unsigned int biClrImportant; // 重要的顏色索引數 (4 Bytes)
}__attribute__((packed))BITMAPINFOHEADER; 
typedef struct _BMPIMAGE{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    unsigned char ***pixelMatrix;
}__attribute__((packed)) BMPIMAGE;

BMPIMAGE* readBMP(const char *filename);
void writeBMP(const char* filename,const BMPIMAGE* BMPImg);
BMPIMAGE* CopyBMP(const BMPIMAGE* BMPImg);
void freeBMP(BMPIMAGE* img);



BMPIMAGE* readBMP(const char *filename){
    FILE *file = fopen(filename, "rb"); // rea with binary
    if (file == NULL) {
        printf("無法打開文件 %s\n", filename);
        return NULL;
    }
    BMPIMAGE *BMPImg = (BMPIMAGE *)malloc(sizeof(BMPIMAGE));

    fread(&BMPImg->fileHeader, sizeof(BMPImg->fileHeader), 1, file); // 讀取文件頭
    // printf("文件標識符 : %x \n",BMPImg->fileHeader.bfType);
    if (BMPImg->fileHeader.bfType != 0x4D42) { // 檢查 BMP 文件標誌 ("BM")
        printf("這不是一個合法的 BMP 文件。\n");
        freeBMP(BMPImg);
        fclose(file);
        return NULL;
    }
    
    // printf("==================== FILEHEADER ====================\n");
    // printf("文件標識符 : %x \n",BMPImg->fileHeader.bfType);
    // printf("BMP file 大小 (Bytes) : %d \n",BMPImg->fileHeader.bfSize);
    // printf("保留字段1 : %d \n",BMPImg->fileHeader.bfReserved1);
    // printf("保留字段2 : %d \n",BMPImg->fileHeader.bfReserved2);
    // printf("pixel data offset (Bytes) : %d \n",BMPImg->fileHeader.bfOffBits);
    // printf("====================================================\n");
    
    fread(&BMPImg->infoHeader, sizeof(BMPImg->infoHeader), 1, file); // 讀取信息頭
    
    // printf("==================== INFOHEADER ====================\n");
    // printf("INFOHEADER 大小(Bytes) : %d\n",infoHeader.biSize); //sizeof(BITMAPINFOHEADER)
    // printf("圖片寬度: %d\n", BMPImg->infoHeader.biWidth);
    // printf("圖片高度: %d\n", BMPImg->infoHeader.biHeight);
    // printf("色彩平面數 : %d\n",BMPImg->infoHeader.biPlanes); // 必須為 1
    // printf("位元深度: %d\n", BMPImg->infoHeader.biBitCount); //  24=（RGB，每個顏色 8 位） , 32（帶有 alpha 通道的 RGBA，每個顏色 8 位+alpha 8 位）。
    // printf("圖像壓縮類型 : %d\n", BMPImg->infoHeader.biCompression); // 0 代表無壓縮（最常見的），1 代表 RLE-8 壓縮，2 代表 RLE-4 壓縮。
    // printf("IMG DATA 大小(Bytes) : %d\n", BMPImg->infoHeader.biSizeImage); // 對於無壓縮的 BMP，設置為 0，可以通過寬度、高度和位深來計算。
    // printf("圖像的水平分辨率 : %d\n", BMPImg->infoHeader.biXPelsPerMeter);
    // printf("圖像的垂直分辨率 : %d\n", BMPImg->infoHeader.biYPelsPerMeter);
    // printf("實際使用的顏色數量 : %d\n", BMPImg->infoHeader.biClrUsed); // 對於 24 位或 32 位圖像，這個字段通常設置為 0，因為不使用調色板。
    // printf("圖像顯示重要的顏色數量 : %d\n", BMPImg->infoHeader.biClrImportant); // 0 表示所有顏色都是重要的。
    // printf("====================================================\n");
    
    int colSize;
    // ex : 1x3(9 Bytes) -> BGR BGR BGR -> padding to 12 Bytes BGR BGR BGR + 3Bytes(0x00)
    colSize = ((BMPImg->infoHeader.biBitCount * BMPImg->infoHeader.biWidth + 31) / 32) * 4; //每一個row必須是4bytes 的倍數
    int dataSize = colSize * BMPImg->infoHeader.biHeight;
    // printf("colSize : %d\n",colSize);
    // printf("dataSize : %d\n",dataSize);
    unsigned char *pixelData = (unsigned char*)malloc(dataSize);
    fseek(file, BMPImg->fileHeader.bfOffBits, SEEK_SET); // 跳到像素數據的偏移位置 = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
    fread(pixelData, dataSize, 1, file);
    
    if(BMPImg->infoHeader.biBitCount==24) BMPImg->pixelMatrix = (unsigned char ***)malloc(3 * sizeof(unsigned char **));
    else if(BMPImg->infoHeader.biBitCount==32) BMPImg->pixelMatrix = (unsigned char ***)malloc(4 * sizeof(unsigned char **));
    
    for (int i = 0; i < (BMPImg->infoHeader.biBitCount/8); i++) {
        BMPImg->pixelMatrix[i] = (unsigned char **)malloc(BMPImg->infoHeader.biHeight * sizeof(unsigned char *));
        for (int j = 0; j < BMPImg->infoHeader.biHeight; j++) {
            BMPImg->pixelMatrix[i][j] = (unsigned char *)malloc(BMPImg->infoHeader.biWidth * sizeof(unsigned char));
        }
    }
    
    // 將像素數據轉換為矩陣形式 
    for (int row = 0; row < BMPImg->infoHeader.biHeight; row++) {
        for (int col = 0; col < BMPImg->infoHeader.biWidth; col++) {
            int pixelIndex = row * colSize + col * (BMPImg->infoHeader.biBitCount/8); // 讀取順序 BGRA or BGR
            BMPImg->pixelMatrix[2][row][col] = pixelData[pixelIndex + 2]; // R
            BMPImg->pixelMatrix[1][row][col] = pixelData[pixelIndex + 1]; // G
            BMPImg->pixelMatrix[0][row][col] = pixelData[pixelIndex];     // B
            if(BMPImg->infoHeader.biBitCount==32) BMPImg->pixelMatrix[3][row][col] = pixelData[pixelIndex+3]; // A
        }
    }
    fclose(file);
    return BMPImg;
}

void writeBMP(const char* filename,const BMPIMAGE* BMPImg) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("無法創建文件！\n");
        return;
    }
    fwrite(&BMPImg->fileHeader, sizeof(BMPImg->fileHeader), 1, file); // 寫入 BITMAPFILEHEADER
    fwrite(&BMPImg->infoHeader, sizeof(BMPImg->infoHeader), 1, file); // 寫入 BITMAPINFOHEADER
    unsigned char padding[3]={0,0,0};
    int padding_size = (4 - (BMPImg->infoHeader.biWidth * (BMPImg->infoHeader.biBitCount / 8)) % 4) % 4;
    // int colSize = ((BMPImg->infoHeader.biBitCount * BMPImg->infoHeader.biWidth + 31) / 32) * 4; //每一個row必須是4bytes 的倍數
    for(int r=0;r<BMPImg->infoHeader.biHeight;r++){
        for(int c=0;c<BMPImg->infoHeader.biWidth;c++){
            for (int ch = 0; ch <(BMPImg->infoHeader.biBitCount/8); ch++){  // write B->G->R or B->G->R->A 
                fwrite(&BMPImg->pixelMatrix[ch][r][c], 1, 1, file);  // 每個像素3字節 (RGB)
            }
        }
        if(padding_size)  fwrite(&padding, 1, padding_size, file);
    }
    fclose(file);
}

BMPIMAGE* CopyBMP(const BMPIMAGE* src){
    BMPIMAGE *copy = malloc(sizeof(BMPIMAGE));
    copy->fileHeader = src->fileHeader;
    copy->infoHeader = src->infoHeader;

    if(copy->infoHeader.biBitCount==24) copy->pixelMatrix = (unsigned char ***)malloc(3 * sizeof(unsigned char **));
    else if(copy->infoHeader.biBitCount==32) copy->pixelMatrix = (unsigned char ***)malloc(4 * sizeof(unsigned char **));
    
    for (int i = 0; i < (copy->infoHeader.biBitCount/8); i++) {
        copy->pixelMatrix[i] = (unsigned char **)malloc(copy->infoHeader.biHeight * sizeof(unsigned char *));
        for (int j = 0; j < copy->infoHeader.biHeight; j++) {
            copy->pixelMatrix[i][j] = (unsigned char *)malloc(copy->infoHeader.biWidth * sizeof(unsigned char));
            for(int k=0;k<copy->infoHeader.biWidth;k++){
                copy->pixelMatrix[i][j][k] = src->pixelMatrix[i][j][k];  
            }
        }
    }
    return copy;
}

void freeBMP(BMPIMAGE* img) {
    int numChannels = img->infoHeader.biBitCount / 8;
    for (int i = 0; i < numChannels; i++) {
        for (int j = 0; j < img->infoHeader.biHeight; j++) {
            free(img->pixelMatrix[i][j]);
        }
        free(img->pixelMatrix[i]);
    }
    free(img->pixelMatrix);
    free(img);
}

#endif