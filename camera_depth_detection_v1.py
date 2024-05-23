import torch
import torchvision.transforms as transforms
from PIL import Image
import cv2
import numpy as np

# Load MiDaS model
model_type = "MiDaS_small"  # Use a smaller model for faster processing
midas = torch.hub.load("intel-isl/MiDaS", model_type)
device = torch.device("cpu")
midas.to(device)
midas.eval()

# Define a custom transform to normalize the image
class CustomTransform:
    def __init__(self):
        self.to_tensor = transforms.ToTensor()
        self.normalize = transforms.Normalize(mean=[0.5], std=[0.5])

    def __call__(self, img):
        # Convert to tensor and normalize
        img_tensor = self.to_tensor(img)
        img_tensor = self.normalize(img_tensor)
        return img_tensor

custom_transform = CustomTransform()

# Open the camera
cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Resize the frame to match MiDaS input size
    frame_resized = cv2.resize(frame, (256, 192)) #  (800, 450)
    # frame_resized = cv2.resize(frame, (640, 480))  # Resize to 640x480 (or other size supported by MiDaS)

    # Convert resized frame to PIL image
    img = Image.fromarray(cv2.cvtColor(frame_resized, cv2.COLOR_BGR2RGB))

    # Apply custom transform
    img_tensor = custom_transform(img).unsqueeze(0)

    # Perform depth estimation
    with torch.no_grad():
        prediction = midas(img_tensor)
        prediction = torch.nn.functional.interpolate(
            prediction.unsqueeze(1),
            size=(frame_resized.shape[1], frame_resized.shape[0]),  # Resize back to original size
            mode="bicubic",
            align_corners=False,
        ).squeeze()

    depth_map = prediction.numpy()
    depth_map = (depth_map - depth_map.min()) / (depth_map.max() - depth_map.min())
    depth_map = (depth_map * 255).astype(np.uint8)

    # Display depth map
    depth_map_colored = cv2.applyColorMap(depth_map, cv2.COLORMAP_MAGMA)
    cv2.imshow('Depth Map', depth_map_colored)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
