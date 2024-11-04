#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"Function.h"
#include <stdint.h>

void UnSharp_Masking(const char * input_filename,const char *output_filename);
void High_Boosting(const char * input_filename,const char *output_filename,const float k);

int main(int argc,char* argv[]){
    char *Input_BmpFile = "input2.bmp";
    char *Sharpness_Enhancement_Img1 ="output2_1.bmp";
    char *Sharpness_Enhancement_Img2 ="output2_2.bmp";
    float high_boost_k  = 1.6;
    UnSharp_Masking(Input_BmpFile,Sharpness_Enhancement_Img1);
    High_Boosting(Input_BmpFile,Sharpness_Enhancement_Img2,high_boost_k);
    return 0;
}

void UnSharp_Masking(const char * input_filename,const char *output_filename){
    // filter setting
    int kernal[3][3]={
        {-1,-1,-1},
        {-1,9,-1},
        {-1,-1,-1}
    };
    int kernal_rowsize = 3 , kernal_colsize = 3;
    BMPIMAGE* Img = readBMP(input_filename);
    int channel = Img->infoHeader.biBitCount/8; // RGB
    int img_rowsize = Img->infoHeader.biHeight;
    int img_colsize = Img->infoHeader.biWidth;

    // new memory to save convolution result
    unsigned char *** conv_img = (unsigned char ***)malloc(channel*sizeof(unsigned char **));
    for(int ch=0;ch<channel;ch++){
        conv_img[ch] = (unsigned char **)malloc(img_rowsize*sizeof(unsigned char*));
        for(int r=0;r<img_rowsize;r++){
            conv_img[ch][r] = (unsigned char*)malloc(img_colsize*sizeof(unsigned char));
        }
    }
    // do conv
    for(int ch=0;ch<channel;ch++){
        for(int r=0;r<img_rowsize;r++){
            for(int c=0;c<img_colsize;c++){
                int sum=0;
                // apply the kernel
                for (int k_r = 0; k_r < kernal_rowsize; k_r++) {
                    for (int k_c = 0; k_c < kernal_colsize; k_c++) {
                        // calculate the pixel location relative to the kernel center
                        int pixel_r = r + k_r - kernal_rowsize / 2;
                        int pixel_c = c + k_c - kernal_colsize / 2;
                        // check for boundary conditions
                        if (pixel_r >= 0 && pixel_r < img_rowsize && pixel_c >= 0 && pixel_c < img_colsize) {
                            sum += kernal[k_r][k_c] * Img->pixelMatrix[ch][pixel_r][pixel_c];
                        }
                    }
                }

                sum = fmax(0,fmin(255,sum));
                conv_img[ch][r][c] = (unsigned char)sum;
            }
        }
    }

    int padding_size = kernal_colsize/2; // 因為zero padding 外圍沒有跟全部的filter做完整convolution，所以外圍採用原本的就好
    // assign convolution result to img
    for(int ch=0;ch<channel;ch++){
        for(int r=padding_size;r<img_rowsize-padding_size;r++){
            for(int c=padding_size;c<img_colsize-padding_size;c++){
                Img->pixelMatrix[ch][r][c] = conv_img[ch][r][c];
            }
        }
    }
    // write the img
    writeBMP(output_filename,Img);

    // Free allocated memory
    for (int ch = 0; ch < channel; ch++) {
        for (int r = 0; r < img_rowsize; r++) {
            free(conv_img[ch][r]);
        }
        free(conv_img[ch]);
    }
    free(conv_img);
    return;
}

void High_Boosting(const char * input_filename,const char *output_filename,const float k){
    // filter setting
    int kernal[5][5]={
        { 0,  0, -1,  0,  0},
        { 0, -1, -2, -1,  0},
        {-1, -2, 16, -2, -1},
        { 0, -1, -2, -1,  0},
        { 0,  0, -1,  0,  0}
    };
    int kernal_rowsize = 5 , kernal_colsize = 5;
    BMPIMAGE* Img = readBMP(input_filename);
    int channel = Img->infoHeader.biBitCount/8; // RGB
    int img_rowsize = Img->infoHeader.biHeight;
    int img_colsize = Img->infoHeader.biWidth;

    // new memory to save convolution result
    unsigned char *** conv_img = (unsigned char ***)malloc(channel*sizeof(unsigned char **));
    for(int ch=0;ch<channel;ch++){
        conv_img[ch] = (unsigned char **)malloc(img_rowsize*sizeof(unsigned char*));
        for(int r=0;r<img_rowsize;r++){
            conv_img[ch][r] = (unsigned char*)malloc(img_colsize*sizeof(unsigned char));
        }
    }
    // do conv
    for(int ch=0;ch<channel;ch++){
        for(int r=0;r<img_rowsize;r++){
            for(int c=0;c<img_colsize;c++){
                int sum=0;
                // apply the kernel
                for (int k_r = 0; k_r < kernal_rowsize; k_r++) {
                    for (int k_c = 0; k_c < kernal_colsize; k_c++) {
                        // calculate the pixel location relative to the kernel center
                        int pixel_r = r + k_r - kernal_rowsize / 2;
                        int pixel_c = c + k_c - kernal_colsize / 2;
                        // check for boundary conditions
                        if (pixel_r >= 0 && pixel_r < img_rowsize && pixel_c >= 0 && pixel_c < img_colsize) {
                            sum += kernal[k_r][k_c] * Img->pixelMatrix[ch][pixel_r][pixel_c];
                        }
                    }
                }

                sum = fmax(0,fmin(255,sum));
                conv_img[ch][r][c] = (unsigned char)sum;
            }
        }
    }
    // assign high boost with times k
    int padding_size = kernal_colsize/2; // 因為zero padding 外圍沒有跟全部的filter做完整convolution，所以外圍採用原本的就好(如果疊加上去外圍變白色)
    for(int ch=0;ch<channel;ch++){
        for(int r=padding_size;r<img_rowsize-padding_size;r++){
            for(int c=padding_size;c<img_colsize-padding_size;c++){
                int new_pixel = Img->pixelMatrix[ch][r][c] + k * conv_img[ch][r][c];
                Img->pixelMatrix[ch][r][c] = fmax(0,fmin(255,new_pixel));
            }
        }
    }

    // write the img
    writeBMP(output_filename,Img);

    // Free allocated memory
    for (int ch = 0; ch < channel; ch++) {
        for (int r = 0; r < img_rowsize; r++) {
            free(conv_img[ch][r]);
        }
        free(conv_img[ch]);
    }
    free(conv_img);
    return;
}