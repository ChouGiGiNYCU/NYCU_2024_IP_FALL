#include<stdio.h>
#include<stdlib.h>
#include"Function.h"

void CropBMP(const char* in_filename,const char* out_filename,const int x,const int y,const int w,const int h);

int main(int argc,char* argv[]){
    char *Input_BmpFile1 = "input1.bmp";
    char *Input_BmpFile2 = "input2.bmp";
    char *Crop_Img1 ="output1_crop.bmp";
    char *Crop_Img2 ="output2_crop.bmp";
    
    CropBMP(Input_BmpFile1,Crop_Img1,120,150,400,399);
    CropBMP(Input_BmpFile2,Crop_Img2,120,150,100,100);
    
    return 0;
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