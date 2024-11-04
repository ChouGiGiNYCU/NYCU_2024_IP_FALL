#include<stdio.h>
#include<stdlib.h>
#include"Function.h"

void QuantizationBMP(const char* in_filename,const char* out_filename,int bit); 
int Quantization_func(unsigned char pixel,int quantize_bit_bit);

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
    QuantizationBMP(Input_BmpFile1,Quantization_Img1_1,6);
    QuantizationBMP(Input_BmpFile1,Quantization_Img1_2,4);
    QuantizationBMP(Input_BmpFile1,Quantization_Img1_3,2);
    QuantizationBMP(Input_BmpFile2,Quantization_Img2_1,6);
    QuantizationBMP(Input_BmpFile2,Quantization_Img2_2,4);
    QuantizationBMP(Input_BmpFile2,Quantization_Img2_3,2);
    
    return 0;
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

