# OpenVX Compliant Sample Applications
[TOC]

<p align="center"><img width="50%" src="https://upload.wikimedia.org/wikipedia/commons/d/dd/OpenVX_logo.svg" /></p>

For teams that need compliance with the <a href="https://www.khronos.org/openvx/" target="_blank">Khronos OpenVX™</a> standard, CoreFlow provides a modern execution runtime with seamless integration into OpenVX. This means you can develop with familiar OpenVX APIs while taking advantage of CoreFlow’s portability and optimized execution.

This section provides sample applications that showcase potential usage or may used as reference to develop your own products.

## Prerequisites

* Conformant OpenVX Implementation (provided in release tar ball)

* [OpenCV >= 3.4.0](https://github.com/opencv/opencv/releases/tag/3.4.0) (provided in release tar ball)

* Camera

## Bubble Pop

This sample demonstrates creating and running an OpenVX graph to run Bubble Pop on a live camera. This sample application uses <a href="https://en.wikipedia.org/wiki/OpenCV" target="_blank">OpenCV</a> to decode input image, draw bubbles/donuts and display the output.

 <p align="center"><img width="60%" src="https://raw.githubusercontent.com/ROCm/MIVisionX/master/docs/data/vx-pop-app.gif" /></p>

### Run the bubble pop sample
* **Bubbles**

    ```
    ./bubble-pop --bubble
    ```

* **Donuts**

    ````
    ./bubble-pop --donut
    ````

## Canny Edge Detector

This sample demonstrates creating and running an OpenVX graph to run canny edge detection on an image or a live camera. This sample application uses <a href="https://en.wikipedia.org/wiki/OpenCV" target="_blank">OpenCV</a> to decode input image and display the output.

 <p align="center"><img width="60%" src="https://raw.githubusercontent.com/KhronosGroup/openvx-samples/main/images/canny_image.PNG" /></p>

### Run the canny edge detector sample

* **Live**

    ```
    ./cannyEdgeDetector --live
    ```

* **Image**

    ````
    ./cannyEdgeDetector --image ../data/face.png
    ````

## Optical Flow

This sample demonstrates creating and running an OpenVX graph to run Optical Flow on a video/live. This sample application uses <a href="https://en.wikipedia.org/wiki/OpenCV" target="_blank">OpenCV</a> to decode input video and display the output.

 <p align="center"> <img width="60%" src="https://raw.githubusercontent.com/ROCm/MIVisionX/master/docs/data/optical_flow_video.gif"> </p>

### Run the optical flow sample

* **Live**

    ```
    ./bin/examples/optical_flow --live 0
    ```
* **Video**

    ````
    ./bin/examples/optical_flow --video ../data/driving_virtual_20.mp4
    ````
## Skin Tone Detector

This sample demonstrates creating and running an OpenVX graph to run skintone detection on an image or a live camera. This sample application uses <a href="https://en.wikipedia.org/wiki/OpenCV" target="_blank">OpenCV</a> to decode input image and display the output.

 <p align="center"><img width="60%" src="https://raw.githubusercontent.com/KhronosGroup/openvx-samples/main/images/skintone-detect-app.png" /></p>

### Run the skin tone sample

* **Live**

    ```
    ./bin/examples/skinToneDetector --live
    ```

* **Image**

    ````
    ./bin/examples/skinToneDetector --image ../data/face.png
    ````


## ORB (Oriented FAST and Rotated BRIEF)
This sample demonstrates creating and running an OpenVX graph to run ORB (Oriented FAST and Rotated BRIEF) on a live camera. This sample application uses <a href="https://en.wikipedia.org/wiki/OpenCV" target="_blank">OpenCV</a> to detect and display keypoints.

 <p align="center"><img width="60%" src="https://raw.githubusercontent.com/opencv/opencv/master/doc/py_tutorials/py_feature2d/py_orb/images/orb_kp.jpg" /></p>

### Run the ORB sample

* **Live**

    ```
    ./bin/examples/orb
    ```

<div class="section_buttons">

| Previous          |                              Next |
|:------------------|----------------------------------:|
| [OpenVX Integration](openvx.md) | [License](../LICENSE.md) |

</div>