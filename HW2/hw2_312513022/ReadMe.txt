Compile Command:

Low_luminosity_Enhancement:
	gcc Low_luminosity_Enhancement.c -o Low_luminosity_Enhancement
	./Low_luminosity_Enhancement

Sharpness_Enhancemen:
	gcc Sharpness_Enhancemen.c -o Sharpness_Enhancemen
	./Sharpness_Enhancemen

Denoise:
	gcc Denoise.c -o Denoise
	./Denoise


or use IDE compiler is ok.

Please ensure all input *.bmp file and Function.h *.c file in same folder !


----------------------------------------------------------------------------
Function.h : define the bmp struct and some function
	- readBMP : read bmp file name , return bmp format.
	- writeBMP : write a new bmp file.
	- CopyBMP : Copy origin bmp data.
	- freeBMP : free memory.

Low_luminosity_Enhancement.c: Do luminosity Enhancement for img
	- Luma_Enhancement : Flip the img.

Sharpness_Enhancemen.c : Do Sharpness Enhancemen for img
	- UnSharp_Masking : Do Sharpness Enhancement for img
	- High_Boosting   : Do Sharpness Enhancement for img

Denoise.c : Denoise the img
	- Median_Filter : Denoise the img
    - Alpha_Trimmed_Mean_Filter : Denoise the img



