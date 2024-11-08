from PIL import Image
import numpy as np

# Load the existing image
image_path = 'input_image.jpg'  # Update this to your image file path
image = Image.open(image_path)

# Convert to grayscale if necessary
image = image.convert('L')

# Resize to 1920x1080 if needed
image = image.resize((1920, 1080))

# Convert to numpy array
image_array = np.array(image, dtype=np.uint8)

# Save to a binary file
with open('input_image.txt', 'w') as f:
    for row in image_array:
        # Write each row as space-separated values
        np.savetxt(f, [row], fmt='%d', delimiter=' ')


print("Image pixel and saved as 'input_image.txt'")

