Compile Command:

Flip:
	gcc Flip.c -o Flip
	./Flip

Resolution:
	gcc Resolution.c -o Resolution
	./Resolution

Crop:
	gcc Crop.c -o Crop
	./Crop
 

or use IDE compiler is ok.

Please ensure all input .bmp file and Function.h .c file in same folder !


----------------------------------------------------------------------------
Function.h : define the bmp struct and some function
	- readBMP : read bmp file name , return bmp format.
	- writeBMP : write a new bmp file.
	- CopyBMP : Copy origin bmp data.
	- freeBMP : free memory.

Flip.c: Do Flip the img 
	- FlipBMP : Flip the img.

Resolution.c : Do quantize for img
	- QuantizationBMP : Do quantize for img .
	- Quantization_func : Do bit shift function

Crop.c : Crop the img
	- CropBMP : Crop the img.



