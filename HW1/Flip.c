#include<stdio.h>
#include<stdlib.h>
#include"Function.h"

void FlipBMP(const char* in_filename,const char* out_filename);
void swap(unsigned char* a,unsigned char* b);

int main(int argc,char* argv[]){
    char *Input_BmpFile1 = "input1.bmp";
    char *Input_BmpFile2 = "input2.bmp";
    char *Flip_Img1 = "output1_flip.bmp";
    char *Flip_Img2 = "output2_flip.bmp";
    
    FlipBMP(Input_BmpFile1,Flip_Img1);
    FlipBMP(Input_BmpFile2,Flip_Img2);
    
    return 0;
}
void swap(unsigned char* a,unsigned char* b){
    unsigned char c=*a;
    *a=*b;
    *b=c;
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