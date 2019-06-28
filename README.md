# lane-departure-warning-system
Abstract: using OpenCV and CUDA to build a real-time lane departure warning system

Environment: Windows 10, Visual Studio 2017, OpenCV 3.4.1, CUDA 10.0

Special Notice: this project has been used for my graduation design and has won the excellent graduation thesis

How to configure the environment?

1. install and configure OpenCV

    https://blog.csdn.net/weixin_42731241/article/details/81626700

2. install and configure CUDA (you need to have a NVIDIA GPU first)

    https://blog.csdn.net/qq_30623591/article/details/82084113
    
How to run this project?

1. create a Console Application in VS

2. copy the code in "lane departure warning system.cpp" to your main cpp file or add it as your main cpp file

3. copy the pictures in "calibration pictures" and the "project_video.mp4" to your project directory

4. run the code

5. the output video will be like "lane departure warning.mp4"

Steps

1. camera calibration

    use the camera (identical to the one installed in the car) to capture 15 chessboard pictures in different angles
    
    use the function findChessboardCorners, cornerSubPix and calibrateCamera to calculate camera calibration matrix
    
    use the function remap to calibrate the picture
    
    for more details: https://docs.opencv.org/2.4/doc/tutorials/calib3d/camera_calibration/camera_calibration.html
    
    https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/camera%20calibration.png
    
