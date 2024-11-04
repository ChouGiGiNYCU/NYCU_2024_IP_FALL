#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"Function.h"
#include <stdint.h>

void Luma_Enhancement(const char *input_filename,const char *output_filename);

int main(int argc,char* argv[]){
    char *Input_BmpFile = "input1.bmp";
    char *Luma_Enhancement_Img ="output1.bmp";
    Luma_Enhancement(Input_BmpFile,Luma_Enhancement_Img);
    return 0;
}


void Luma_Enhancement(const char *input_filename,const char *output_filename){
    BMPIMAGE* Img = readBMP(input_filename);
    int channel = Img->infoHeader.biBitCount/8; // RGB
    int img_rowsize = Img->infoHeader.biHeight;
    int img_colsize = Img->infoHeader.biWidth;
    int *hist = (int*)calloc(256,sizeof(int));
    int *cusum = (int*)calloc(256,sizeof(int));
    unsigned char *mapping_value = (unsigned char*)calloc(256,sizeof(unsigned char));

    // RGB transform to YCbCr
    for(int r=0;r<img_rowsize;r++){
        for(int c=0;c<img_colsize;c++){
            YCbCr tmp = RGB2YCbCr(Img->pixelMatrix[0][r][c],Img->pixelMatrix[1][r][c],Img->pixelMatrix[2][r][c]);
            Img->pixelMatrix[0][r][c] = tmp.Y;
            Img->pixelMatrix[1][r][c] = tmp.Cb;
            Img->pixelMatrix[2][r][c] = tmp.Cr;
            hist[tmp.Y]++;
        }
    }
    cusum[0] = hist[0];
    int CDF_min = cusum[0]>0?cusum[0]:-1;
    for(int i=1;i<256;i++){
        cusum[i] = cusum[i-1] + hist[i];
        if(CDF_min==-1 && cusum[i]!=0) CDF_min=cusum[i];
    }

    // Do Histogram equalization that find mapping
    for(int i=0;i<256;i++){
        mapping_value[i] = round((double)(cusum[i]-CDF_min)/(img_colsize*img_rowsize-CDF_min)*255);
    }
    // Transform before value to new value
    for(int r=0;r<img_rowsize;r++){
        for(int c=0;c<img_colsize;c++){
            Img->pixelMatrix[0][r][c] = mapping_value[Img->pixelMatrix[0][r][c]];
        }
    }

    // YCbCr transform to RGB
    for(int r=0;r<img_rowsize;r++){
        for(int c=0;c<img_colsize;c++){
            RGB tmp = YCbCr2RGB(Img->pixelMatrix[0][r][c],Img->pixelMatrix[1][r][c],Img->pixelMatrix[2][r][c]);
            Img->pixelMatrix[0][r][c] = tmp.R;
            Img->pixelMatrix[1][r][c] = tmp.G;
            Img->pixelMatrix[2][r][c] = tmp.B;
        }
    }
    // write the img
    writeBMP(output_filename,Img);

    free(hist);
    free(cusum);
    free(mapping_value);
    return;
}

