#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include"Function.h"
#define Alpha 12
#define KERNAL_SIZE 5
#define C1 6.5025 // (K1*L)^2 ,K1 = 0.01 , L (8bit) = 255
#define C2 58.5225 // (K2*L)^2 ,K2 = 0.03 , L (8bit) = 255


void Median_Filter(const char *input_filename,const char *output_filename,const char *Origin_filename,bool count_SSIM);
void Alpha_Trimmed_Mean_Filter(const char *input_filename,const char *output_filename,int alpha,char *Origin_filename,bool count_SSIM);
int cmpfunc(const void *a,const void *b);
double ssim_rgb(unsigned char ***img1,unsigned char ***img2, int width, int height,int channel);
double mean(unsigned char **img, int width, int height);
double variance(unsigned char **img, int width, int height,double mean_val);
double covariance(unsigned char **img1,unsigned char **img2,int width,const int height,const double mean1,const double mean2);
double ssim_channel(unsigned char **img1,unsigned char **img2, int width, int height);

int main(int argc,char* argv[]){

    char *Input_BmpFile1 = "input3.bmp";
    char *Origin_BmpFile1 = "input3_org.bmp";
    char *Denoise_1 ="output3_1.bmp";
    char *Denoise_2 ="output3_2.bmp";
    Median_Filter(Input_BmpFile1,Denoise_2,Origin_BmpFile1,true);
    Alpha_Trimmed_Mean_Filter(Input_BmpFile1,Denoise_1,Alpha,Origin_BmpFile1,true);
    char *Input_BmpFile2 = "input4.bmp";
    char *Origin_BmpFile2 = "input4_org.bmp";
    char *Denoise_3 ="output4_1.bmp";
    char *Denoise_4 ="output4_2.bmp";
    Median_Filter(Input_BmpFile2,Denoise_4,Origin_BmpFile2,true);
    Alpha_Trimmed_Mean_Filter(Input_BmpFile2,Denoise_3,Alpha,Origin_BmpFile2,true);
    return 0;
}

int cmpfunc(const void *a,const void *b){
    return *(unsigned char*)a - *(unsigned char*)b; // small -> big
}

// 取得環狀邊界條件的索引
int wrap_index(int index, int max_size) {
    if (index < 0) return (index + max_size) % max_size;
    if (index >= max_size) return index % max_size;
    return index;
}

void Median_Filter(const char *input_filename,const char *output_filename,const char *Origin_filename,bool count_SSIM){
    BMPIMAGE* Img = readBMP(input_filename);
    int channel = Img->infoHeader.biBitCount/8; // RGB
    int img_rowsize = Img->infoHeader.biHeight;
    int img_colsize = Img->infoHeader.biWidth;
    unsigned char window[KERNAL_SIZE*KERNAL_SIZE];

    // new memory to save convolution result
    unsigned char *** conv_img = (unsigned char ***)malloc(channel*sizeof(unsigned char **));
    for(int ch=0;ch<channel;ch++){
        conv_img[ch] = (unsigned char **)malloc(img_rowsize*sizeof(unsigned char*));
        for(int r=0;r<img_rowsize;r++){
            conv_img[ch][r] = (unsigned char*)malloc(img_colsize*sizeof(unsigned char));
        }
    }
    // Do Median filter
    for(int ch=0;ch<channel;ch++){
        for(int r=0;r<img_rowsize;r++){
            for(int c=0;c<img_colsize;c++){
                int k=0;
                for(int k_r=0;k_r<KERNAL_SIZE;k_r++){
                    for(int k_c=0;k_c<KERNAL_SIZE;k_c++){
                        int pixel_r = r + k_r - KERNAL_SIZE/2;
                        int pixel_c = c + k_c - KERNAL_SIZE/2;
                        pixel_r = wrap_index(pixel_r,img_rowsize);
                        pixel_c = wrap_index(pixel_c,img_colsize);
                        window[k++] = Img->pixelMatrix[ch][pixel_r][pixel_c];
                    }
                }

                // sort
                qsort(window,KERNAL_SIZE*KERNAL_SIZE,sizeof(unsigned char),cmpfunc);
                conv_img[ch][r][c] = window[(KERNAL_SIZE*KERNAL_SIZE)/2];
            }
        }
    }
    // assign convolution result to img
    for(int ch=0;ch<channel;ch++){
        for(int r=0;r<img_rowsize;r++){
            for(int c=0;c<img_colsize;c++){
                Img->pixelMatrix[ch][r][c] = conv_img[ch][r][c];
            }
        }
    }
    // write the img
    writeBMP(output_filename,Img);
    // count SSIM
    if(count_SSIM && Origin_filename!=NULL){
        BMPIMAGE* Origin_Img = readBMP(Origin_filename);
        double ssim_value = ssim_rgb(Img->pixelMatrix,Origin_Img->pixelMatrix,img_colsize,img_rowsize,channel);
        printf("%s comapre with %s  SSIM : %f\n",output_filename,Origin_filename,ssim_value);
    }

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

void Alpha_Trimmed_Mean_Filter(const char *input_filename,const char *output_filename,int alpha,char *Origin_filename,bool count_SSIM){
    BMPIMAGE* Img = readBMP(input_filename);
    int channel = Img->infoHeader.biBitCount/8; // RGB
    int img_rowsize = Img->infoHeader.biHeight;
    int img_colsize = Img->infoHeader.biWidth;
    int trim_count = alpha/2;
    unsigned char window[KERNAL_SIZE*KERNAL_SIZE];

    // new memory to save convolution result
    unsigned char *** conv_img = (unsigned char ***)malloc(channel*sizeof(unsigned char **));
    for(int ch=0;ch<channel;ch++){
        conv_img[ch] = (unsigned char **)malloc(img_rowsize*sizeof(unsigned char*));
        for(int r=0;r<img_rowsize;r++){
            conv_img[ch][r] = (unsigned char*)malloc(img_colsize*sizeof(unsigned char));
        }
    }

    for(int ch=0;ch<channel;ch++){
        for(int r=0;r<img_rowsize;r++){
            for(int c=0;c<img_colsize;c++){
                int k=0;
                for(int k_r=0;k_r<KERNAL_SIZE;k_r++){
                    for(int k_c=0;k_c<KERNAL_SIZE;k_c++){
                        int pixel_r = r + k_r - KERNAL_SIZE/2;
                        int pixel_c = c + k_c - KERNAL_SIZE/2;
                        pixel_r = wrap_index(pixel_r,img_rowsize);
                        pixel_c = wrap_index(pixel_c,img_colsize);
                        window[k++] = Img->pixelMatrix[ch][pixel_r][pixel_c];
                    }
                }

                // sort
                qsort(window,KERNAL_SIZE*KERNAL_SIZE,sizeof(unsigned char),cmpfunc);\
                // 去掉 alpha 個最大和最小值，計算剩餘值的平均
                int sum=0;
                for(int i=trim_count;i<KERNAL_SIZE*KERNAL_SIZE-trim_count;i++){
                    sum += window[i];
                }
                int mean =  sum/(KERNAL_SIZE*KERNAL_SIZE-alpha);
                conv_img[ch][r][c] = (unsigned char)mean;
            }
        }
    }
    // assign convolution result to img
    for(int ch=0;ch<channel;ch++){
        for(int r=0;r<img_rowsize;r++){
            for(int c=0;c<img_colsize;c++){
                Img->pixelMatrix[ch][r][c] = conv_img[ch][r][c];
            }
        }
    }
    // write the img
    writeBMP(output_filename,Img);
    // count SSIM
    if(count_SSIM && Origin_filename!=NULL){
        BMPIMAGE* Origin_Img = readBMP(Origin_filename);
        double ssim_value = ssim_rgb(Img->pixelMatrix,Origin_Img->pixelMatrix,img_colsize,img_rowsize,channel);
        printf("%s comapre with %s  SSIM : %f\n",output_filename,Origin_filename,ssim_value);
    }
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
// 計算影像區域的平均值
double mean(unsigned char **img, int width, int height) {
    double sum = 0.0;
    for (int r = 0; r < height; r++) {
        for(int c=0;c<width;c++){
            sum += img[r][c];
        }
    }
    return sum / (width * height);
}

// 計算影像區域的方差
double variance(unsigned char **img, int width, int height,const double mean_val) {
    double sum = 0.0;
    for (int r = 0; r < height; r++) {
        for(int c=0;c<width;c++){
            sum += pow(img[r][c] - mean_val, 2);
        }
    }
    return sum / (width * height);
}

// 計算兩幅影像的協方差
double covariance(unsigned char **img1,unsigned char **img2,const int width,const int height,const double mean1,const double mean2) {
    double sum = 0.0;
    for (int r=0; r<height;r++) {
        for(int c=0;c<width;c++){
            sum += (img1[r][c] - mean1) * (img2[r][c] - mean2);
        }
    }
    return sum / (width * height);
}

// 計算 SSIM，適用於單通道影像
double ssim_channel(unsigned char **img1,unsigned char **img2, int width, int height) {
    double mean1 = mean(img1, width, height);
    double mean2 = mean(img2, width, height);

    double var1 = variance(img1, width, height, mean1);
    double var2 = variance(img2, width, height, mean2);

    double cov12 = covariance(img1, img2, width, height, mean1, mean2);

    double ssim_value = ((2 * mean1 * mean2 + C1) * (2 * cov12 + C2)) /
                        ((mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2));

    return ssim_value;
}

// 計算 RGB 影像的 SSIM，對 R、G、B 三個通道計算 SSIM 並取平均
double ssim_rgb(unsigned char ***img1,unsigned char ***img2, int width, int height,int channel) {
    // 分別計算 R、G、B 通道的 SSIM
    double ssim_r = ssim_channel(img1[0], img2[0], width, height);
    double ssim_g = ssim_channel(img1[1], img2[1], width, height);
    double ssim_b = ssim_channel(img1[2], img2[2], width, height);

    // 計算三個通道的 SSIM 平均值
    return (ssim_r + ssim_g + ssim_b) / 3.0;
}