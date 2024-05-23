import torch
import torchvision.transforms as transforms
from PIL import Image
import cv2
import numpy as np
import os

# Load YOLOv5s model
model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)
model.cpu()

# Load MiDaS model
model_type = "MiDaS_small"
midas = torch.hub.load("intel-isl/MiDaS", model_type)
device = torch.device("cpu")
midas.eval()

# Define a custom transform to normalize the image
class CustomTransform:
    def __init__(self):
        self.to_tensor = transforms.ToTensor()
        self.normalize = transforms.Normalize(mean=[0.5], std=[0.5])

    def __call__(self, img):
        img_tensor = self.to_tensor(img)
        img_tensor = self.normalize(img_tensor)
        return img_tensor

custom_transform = CustomTransform()

# Define the directories containing the images and where to save processed images
image_directory = 'images'
processed_image_directory = 'processed_images'

# Create the processed images directory if it doesn't exist
os.makedirs(processed_image_directory, exist_ok=True)

# Iterate over each color image in the directory
for image_filename in os.listdir(image_directory):
    if image_filename.endswith('_Color.png'):
        color_image_path = os.path.join(image_directory, image_filename)
        depth_image_path = color_image_path.replace('_Color.png', '_Depth.png')

        # Load the color image
        color_image = cv2.imread(color_image_path)

        # Load the depth image
        depth_image = cv2.imread(depth_image_path, cv2.IMREAD_GRAYSCALE)

        if depth_image is None:
            print(f"Depth image not found for {image_filename}")
            continue

        # Resize the color image to match MiDaS input size
        color_image_resized = cv2.resize(color_image, (512, 384))

        # YOLO object detection
        results = model(color_image)

        # Convert resized color image to PIL image for MiDaS
        img = Image.fromarray(cv2.cvtColor(color_image_resized, cv2.COLOR_BGR2RGB))

        # Apply custom transform for MiDaS
        img_tensor = custom_transform(img).unsqueeze(0).to(device)

        # Perform depth estimation with MiDaS
        with torch.no_grad():
            prediction = midas(img_tensor)
            prediction = torch.nn.functional.interpolate(
                prediction.unsqueeze(1),
                size=(color_image_resized.shape[1], color_image_resized.shape[0]),
                mode="bicubic",
                align_corners=False,
            ).squeeze()

        depth_map = prediction.cpu().numpy()
        depth_map = (depth_map - depth_map.min()) / (depth_map.max() - depth_map.min())
        depth_map = (depth_map * 255).astype(np.uint8)

        # Display depth map with YOLO bounding boxes
        depth_map_colored = cv2.applyColorMap(depth_map, cv2.COLORMAP_MAGMA)

        for *xyxy, conf, cls in results.xyxy[0]:
            x, y, x2, y2 = map(int, xyxy)
            cv2.rectangle(depth_map_colored, (x, y), (x2, y2), (0, 255, 0), 2)
            cv2.putText(depth_map_colored, f"{model.names[int(cls)]} {conf:.2f}", (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

        # Resize the original and depth images for display
        color_image_resized_for_display = cv2.resize(color_image, (depth_map_colored.shape[1], depth_map_colored.shape[0]))
        depth_image_resized_for_display = cv2.resize(depth_image, (depth_map_colored.shape[1], depth_map_colored.shape[0]))
        depth_image_colored = cv2.applyColorMap(depth_image_resized_for_display, cv2.COLORMAP_MAGMA)

        # Stack the images side by side
        combined_image = np.hstack((color_image_resized_for_display, depth_image_colored, depth_map_colored))

        # Save the combined image
        output_path = os.path.join(processed_image_directory, f"processed_{image_filename.replace('_Color.png', '')}.png")
        cv2.imwrite(output_path, combined_image)

        print(f"Saved processed image: {output_path}")

print("Processing complete.")
