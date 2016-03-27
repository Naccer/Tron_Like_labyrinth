// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include<stdio.h>
#include<stdlib.h>

typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

typedef struct {
     int columns, lines;
     int *matrix;
} PPMImage;

PPMImage* readPPM(char *nameFile) {

    PPMImage *image = (PPMImage*)malloc(sizeof(PPMImage));
    std::fstream myFile(nameFile, std::ios_base::in);

    
    char *type = new char[2];
    myFile >> type;
    std::cout << "Function read PPM:" << std::endl;
    std::cout << "type:" << type << std::endl;

    int c = 0;
    int l = 0;
    myFile >>  c;
    myFile >> l;

    std::cout << "lines:" << l << std::endl;
    std::cout << "colums:" << c << std::endl;
    
    int max;
    myFile >> max;
    std::cout << "max:" << max << std::endl;

    //c = 50;
    //l = 50;
    //image -> columns = c;
    //image -> lines = l;
    
    image -> matrix = new int[l*c];

    int pos = 0;
    for (int i = 0; i < l; i++)
    {
	for (int j = 0; j < c; j++)
	{
        	unsigned char r;
        	unsigned char g;
       		unsigned char b;
    		myFile >> r;
    		myFile >> g;
    		myFile >>  b;  
        	//std::cout << "(" << (int)r << ", " << (int)g << ", " << (int)b << ")" << std::endl;
		//std::cout << "(" << i%16 << ", " << j%16 << ")" << std::endl;
		if (i%16 == 0 and j%16== 0)
		{
			//cout << "aqui" << std::endl;
			if (r == 0 and g == 0 and b == 0)
				image -> matrix[pos] = 0;
			else if (r == 255 and g == 255 and b == 255)
				image -> matrix[pos] = 1;
       			else if (r == 255 and g == 0 and b == 0)
				image -> matrix[pos] = 2; 
        		else if (r == 0 and g == 255 and b == 0)
				image -> matrix[pos] = 3;  
			pos++;
		}
        	//std::cout << image -> matrix[i]; 
	}    
    }
    image -> columns = c/16;
    image -> lines = l/16;
    
    myFile.close();
    return image;
}

/*int main()
{
    PPMImage* image = readPPM("image.ppm");

	return 0;
}*/


