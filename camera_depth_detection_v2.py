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

# Open the camera
cap = cv2.VideoCapture(2)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Resize the frame to match MiDaS input size
    frame_resized = cv2.resize(frame, (512, 384))

    # YOLO object detection
    results = model(frame)

    # Convert resized frame to PIL image for MiDaS
    img = Image.fromarray(cv2.cvtColor(frame_resized, cv2.COLOR_BGR2RGB))

    # Apply custom transform for MiDaS
    img_tensor = custom_transform(img).unsqueeze(0).to(device)

    # Perform depth estimation with MiDaS
    with torch.no_grad():
        prediction = midas(img_tensor)
        prediction = torch.nn.functional.interpolate(
            prediction.unsqueeze(1),
            size=(frame_resized.shape[1], frame_resized.shape[0]),
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
    # for *xyxy, conf, cls in results.xyxy[0]:
    #     x, y, x2, y2 = map(int, xyxy)
    #     cv2.rectangle(depth_map_colored, (x, y), (x2, y2), (0, 255, 0), 2)
    #     cv2.putText(depth_map_colored, f"{int(cls)} {conf:.2f}", (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Display depth map with YOLO bounding boxes
    cv2.imshow('Depth Map with YOLO Bounding Boxes', depth_map_colored)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()