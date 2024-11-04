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
void FlipBMP(const char* in_filename,const char* out_filename);
void swap(unsigned char* a,unsigned char* b);
void QuantizationBMP(const char* in_filename,const char* out_filename,int bit); 
int Quantization_func(unsigned char pixel,int quantize_bit_bit);
void CropBMP(const char* in_filename,const char* out_filename,const int x,const int y,const int w,const int h);

int main(int argc,char* argv[]){
    char *Input_BmpFile1 = "input1.bmp";
    char *Input_BmpFile2 = "input2.bmp";
    char *Flip_Img1 = "output1_flip.bmp";
    char *Flip_Img2 = "output2_flip.bmp";
    char *Quantization_Img1_1 ="output1_1.bmp";
    char *Quantization_Img1_2 ="output1_2.bmp";
    char *Quantization_Img1_3 ="output1_3.bmp";
    char *Quantization_Img2_1 ="output2_1.bmp";
    char *Quantization_Img2_2 ="output2_2.bmp";
    char *Quantization_Img2_3 ="output2_3.bmp";
    char *Crop_Img1 ="output1_crop.bmp";
    char *Crop_Img2 ="output2_crop.bmp";
    FlipBMP(Input_BmpFile1,Flip_Img1);
    FlipBMP(Input_BmpFile2,Flip_Img2);
    QuantizationBMP(Input_BmpFile1,Quantization_Img1_1,6);
    QuantizationBMP(Input_BmpFile1,Quantization_Img1_2,4);
    QuantizationBMP(Input_BmpFile1,Quantization_Img1_3,2);
    QuantizationBMP(Input_BmpFile2,Quantization_Img2_1,6);
    QuantizationBMP(Input_BmpFile2,Quantization_Img2_2,4);
    QuantizationBMP(Input_BmpFile2,Quantization_Img2_3,2);
    CropBMP(Input_BmpFile1,Crop_Img1,120,150,400,399);
    CropBMP(Input_BmpFile2,Crop_Img2,120,150,100,100);
    return 0;
}


BMPIMAGE* readBMP(const char *filename){
    FILE *file = fopen(filename, "rb"); // rea with binary
    if (file == NULL) {
        printf("無法打開文件 %s\n", filename);
        return NULL;
    }
    BMPIMAGE *BMPImg = malloc(sizeof(BMPIMAGE));

    fread(&BMPImg->fileHeader, sizeof(BMPImg->fileHeader), 1, file); // 讀取文件頭
    // printf("文件標識符 : %x \n",BMPImg->fileHeader.bfType);
    if (BMPImg->fileHeader.bfType != 0x4D42) { // 檢查 BMP 文件標誌 ("BM")
        printf("這不是一個合法的 BMP 文件。\n");
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

void FlipBMP(const char* in_filename,const char* out_filename){
    BMPIMAGE* Img = readBMP(in_filename);
    if(Img==NULL){
        printf("%s read Error !! \n",in_filename);
        return;
    }
    BMPIMAGE* Copy_Img = CopyBMP(Img);
    // Flip  left <-> right
    for(int r=0;r<(Copy_Img->infoHeader.biHeight);r++){
        for(int c=0;c<(Copy_Img->infoHeader.biWidth/2);c++){
            for(int ch=0;ch<(Copy_Img->infoHeader.biBitCount/8);ch++){
                swap(&Copy_Img->pixelMatrix[ch][r][c],&Copy_Img->pixelMatrix[ch][r][Copy_Img->infoHeader.biWidth-c-1]);
            }
        }
    } 
    writeBMP(out_filename,Copy_Img);
    freeBMP(Img);
    freeBMP(Copy_Img);
    printf("Flip Sucess !\n");
}

void swap(unsigned char* a,unsigned char* b){
    unsigned char c=*a;
    *a=*b;
    *b=c;
}

void QuantizationBMP(const char* in_filename,const char* out_filename,int bit){
    BMPIMAGE* Img = readBMP(in_filename);
    if(Img==NULL){
        printf("%s read Error !! \n",in_filename);
        return;
    }
    BMPIMAGE* Copy_Img = CopyBMP(Img);
    for(int r=0;r<(Copy_Img->infoHeader.biHeight);r++){
        for(int c=0;c<(Copy_Img->infoHeader.biWidth);c++){
            for(int ch=0;ch<(Copy_Img->infoHeader.biBitCount/8);ch++){
                Copy_Img->pixelMatrix[ch][r][c] = Quantization_func(Copy_Img->pixelMatrix[ch][r][c],bit);
            }
        }
    } 
    writeBMP(out_filename,Copy_Img);
    freeBMP(Img);
    freeBMP(Copy_Img);
    printf("Quantization Success !\n");
    return;
} 

int Quantization_func(unsigned char pixel,int quantize_bit){
    // 8 bit -> x bit  =  (p>>(8-x))<<(8-x)
    return ((pixel>>(8-quantize_bit))<<(8-quantize_bit)) ;
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

void CropBMP(const char* in_filename,const char* out_filename,const int x,const int y,const int w,const int h){
    BMPIMAGE* Img = readBMP(in_filename);
    if(Img==NULL){
        printf("%s read Error !! \n",in_filename);
        return;
    }
    // check img size is enough to crop
    if (x < 0 || x + w > Img->infoHeader.biWidth || y < 0 || y + h > Img->infoHeader.biHeight) {
        printf("Crop area exceeds the image boundaries!\n");
        freeBMP(Img);
        return;
    }
    BMPIMAGE* Copy_Img = CopyBMP(Img);
    const int new_width  = w;
    const int new_height = h; 
    const int new_channel = Copy_Img->infoHeader.biBitCount/8;
    const int new_Colsize = ((Copy_Img->infoHeader.biBitCount*new_width+31)/32)*4;
    const int new_ImageSize = new_Colsize * new_height;  // 新圖像數據大小
    // free Copy img
    for (int i = 0; i < new_channel; i++) {
        for (int j = 0; j < Copy_Img->infoHeader.biHeight; j++) {
            free(Copy_Img->pixelMatrix[i][j]);
        }
        free(Copy_Img->pixelMatrix[i]);
    }
    free(Copy_Img->pixelMatrix);

    Copy_Img->pixelMatrix = (unsigned char ***)malloc(new_channel * sizeof(unsigned char **));
    for (int i = 0; i < new_channel; i++) {
        Copy_Img->pixelMatrix[i] = (unsigned char **)malloc(new_height * sizeof(unsigned char *));
        for (int j = 0; j < new_height; j++) {
            Copy_Img->pixelMatrix[i][j] = (unsigned char *)malloc(new_width * sizeof(unsigned char));
        }
    }
    
    // read crop img size  from origin img
    for (int row = y; row < y+new_height; row++) {
        for (int col = x; col < x+new_width; col++) {
            for(int ch=0;ch<new_channel;ch++){
                Copy_Img->pixelMatrix[ch][row-y][col-x] = Img->pixelMatrix[ch][row][col];
            }
        }
    }

    // change some parameter by bmp file setting
    Copy_Img->infoHeader.biHeight = new_height;
    Copy_Img->infoHeader.biWidth = new_width;
    Copy_Img->infoHeader.biSizeImage = new_ImageSize;
    Copy_Img->fileHeader.bfSize = Copy_Img->fileHeader.bfOffBits + new_ImageSize;  // 更新文件大小

    writeBMP(out_filename,Copy_Img);
    freeBMP(Img);
    freeBMP(Copy_Img);
    printf("Crop Success !\n");
    return;
}