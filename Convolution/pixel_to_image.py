import numpy as np
from PIL import Image

# Define the image dimensions
width, height = 1920, 1080

# Load the image data from the text file
image_array = np.zeros((height, width), dtype=np.uint8)

with open('output_image.txt', 'r') as f:
    for y in range(height):
        line = f.readline().strip()  # Read each line
        if line:  # Check if the line is not empty
            pixel_values = list(map(int, line.split()))  # Convert the space-separated string to a list of integers
            image_array[y, :len(pixel_values)] = pixel_values  # Fill the row with pixel values

# Convert the NumPy array back to an image
reconstructed_image = Image.fromarray(image_array, mode='L')

# Save the reconstructed image
reconstructed_image.save('reconstructed_image.png')

print("Image reconstructed and saved as 'reconstructed_image.png'")

