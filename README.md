# camera-v0.1.0
---
## Overview
This folder contains the necessary files and scripts to generate a depth map using Intel's MiDas machine learning model and perform object detection using YOLOv5s.

## Features
- **Depth Map Generation**: Utilize Intel's MiDas model to generate depth maps from images.
- **Object Detection**: Employ YOLOv5s for efficient and accurate object detection.

## Requirements
- Python 3.8+
- PyTorch
- OpenCV
- Other dependencies listed in `requirements.txt`

## Installation
1. Clone the repository:
    ```sh
    git clone -b camera-v0.1.0 https://github.com/STARS-NRM-Navigation/NRM-Navigation/
    cd your_repository
    ```

2. Install the required dependencies:
    ```sh
    pip install -r requirements.txt
    ```

## Usage
![processed_3](https://github.com/STARS-NRM-Navigation/NRM-Navigation/assets/59177816/141be07d-b50f-48e6-a627-5b4d419b5084)
This image shows a comparison between the monocular camera, depth camera and the depth map generation along with object detection.

1. **Generating Depth Map**:
    - Run `camera_depth_detection_v2.py` and enjoy!

## File Structure
- `camera_depth_detection_v1.py`: This script is for only depth map generataion.
- `camera_depth_detection_v2.py`: This script is combining `Yolov5s` and `MiDas` for object detection and depth map generation.
- `camera_depth_detection_v3.py`: This script is adjust to read images from a folder for processing.

## References
- [MiDas Model](https://github.com/intel-isl/MiDaS)
- [YOLOv5](https://github.com/ultralytics/yolov5)

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

---
