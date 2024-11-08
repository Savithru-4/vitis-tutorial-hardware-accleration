#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include "/home/vitis_xrt/Desktop/FPGA_HARDWARE_ACCELERATION/Vitis-Tutorials-2022.2/Hardware_Acceleration/Design_Tutorials/01-convolution-tutorial/src/common.h"

// Define the filter coefficients (example: a simple averaging filter)
//const char coeffs[FILTER_V_SIZE][FILTER_H_SIZE] = {
//    { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//    { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//    { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	 { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	 { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
//};

//const char coeffs[FILTER_V_SIZE][FILTER_H_SIZE] = {
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 1,-7, 1, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
//};

const char coeffs[FILTER_V_SIZE][FILTER_H_SIZE] = {
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 4, 5, 4, 2, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 4, 9,12, 9, 4, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 5,12,15,12, 5, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 4, 9,12, 9, 4, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 4, 5, 4, 2, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


int main() {
    const unsigned short width = 1920;
    const unsigned short height = 1080;
    const unsigned short stride = width;

    // Allocate memory for the source and destination images
    unsigned char src[width * height];
    unsigned char dst[width * height] = {0};

    // Load the image from a text file
    std::ifstream infile("/home/vitis_xrt/Documents/input_image.txt");
    if (!infile) {
        std::cerr << "Error: Could not open input_image.txt" << std::endl;
        return 1;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int pixel_value;
            infile >> pixel_value;
            // Ensure the pixel value is within the valid range
            src[y * stride + x] = static_cast<unsigned char>(std::max(0, std::min(pixel_value, 255)));
        }
    }
    infile.close();

    // Define filter parameters
    float factor = 1.0f / 160.0f; // Adjust as needed for your filter
    short bias = 0;

    // Call the Filter2D function
    Filter2D(coeffs, factor, bias, width, height, stride, src, dst);

    // Write the output image to a text file
    std::ofstream outfile("/home/vitis_xrt/Documents/output_image.txt");
    if (!outfile) {
        std::cerr << "Error: Could not open output_image.txt for writing" << std::endl;
        return 1;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            outfile << static_cast<int>(dst[y * stride + x]) << " ";
        }
        outfile << std::endl; // New line after each row
    }
    outfile.close();

    // Optionally, output a small section of the result for verification
    std::cout << "Output image written to output_image.txt" << std::endl;

    return 0;
}
