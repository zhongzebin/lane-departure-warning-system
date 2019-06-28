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
    
    before and after camera calibration:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/camera%20calibration.png)
    
2. graying
    
    use the function cvtColor to convert the colored image into gray image
    
    the principle of this function is: gray=0.3×R+0.59×G+0.11×B
    
    however, sometimes it doesn't work well such as the following picture shows:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/RGB%20graying.png)
    
    in order to solve this problem, I turn the RGB image into HLS image and extract channel S, then combine it with the gray image I have already got
    
    before and after graying:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/RGB%20and%20HLS%20graying.png)
