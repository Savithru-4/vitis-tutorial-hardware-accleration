#include <iostream>
#include <cstdlib>
#include <hls_stream.h>
#include <fstream>
#include <sstream>

#define MAX_IMAGE_WIDTH  1920
#define MAX_IMAGE_HEIGHT 1080
#define FILTER_V_SIZE 15
#define FILTER_H_SIZE 15

extern "C" {
    void Filter2DKernel(
        const int           coeffs[256],
        float                factor,
        short                bias,
        unsigned short       width,
        unsigned short       height,
        unsigned short       stride,
        const unsigned char  src[MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT],
        unsigned char        dst[MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT]);
}

// Function to initialize coefficients
void initCoefficients(int coeffs[256]) {
    // Example: A simple averaging filter
    for (int i = 0; i < FILTER_V_SIZE * FILTER_H_SIZE; ++i) {
        coeffs[i] = 1; // Set to 1 for averaging
    }
}

// Function to initialize source image
void initSourceImage(unsigned char src[MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT], unsigned short width, unsigned short height) {
    for (unsigned short y = 0; y < height; ++y) {
        for (unsigned short x = 0; x < width; ++x) {
            src[y * width + x] = (x + y) % 256; // Simple pattern
        }
    }
}

// Function to display output image
void displayOutputImage(unsigned char dst[MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT], unsigned short width, unsigned short height) {
    for (unsigned short y = 0; y < height; ++y) {
        for (unsigned short x = 0; x < width; ++x) {
            std::cout << (int)dst[y * width + x] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Define parameters
    unsigned short width = 1920;
    unsigned short height = 1080;
    unsigned short stride = 1920;
    float factor = .005f; // Example factor
    short bias = 0;      // Example bias

    // Allocate memory for coefficients, source, and destination images
    int coeffs[256];
    unsigned char src[MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT];
    unsigned char dst[MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT] = {0};

    // Initialize coefficients and source image
    initCoefficients(coeffs);

//    initSourceImage(src, width, height);

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


    // Call the HLS kernel
    Filter2DKernel(coeffs, factor, bias, width, height, stride, src, dst);

    // Optionally, display the output image (for small sizes)
    // Comment this out if the output is too large
//     displayOutputImage(dst, width, height);

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


    return 0;
}
