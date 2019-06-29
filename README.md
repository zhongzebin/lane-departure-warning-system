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
    
3. edge detection

    I use Canny detector to detect the edge in the image. The function in OpenCV is Canny.
    
    for more details: https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html?highlight=canny
    
    before and after edge detection:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/edge%20detection.png)
    
4. extract ROI (Range of Intrest)
    
    the current lane should only appear in a certain trapezoidal region in the picture, so the information in other regions can be ignored
    
    before and after extracting ROI:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/ROI.png)

5. antiperspective transformation

    First, I should select four points on the straight line and obtain the corridinates of these four points in the image. Then, I should find out the corridinates of these four points in the birdview projection. Using these four groups of points, I can calculate out the transformation matrix (by using the function getPerspectiveTransform).
    
    the four points I selected in the image:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/four%20points.png)
    
    Having obtained the transformation matrix, I can use the function warpPerspective to do the antiperspective transformation.
    
    before and after antiperspective transformation:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/antiperspective%20transformation.png)
6. Hough transform
    
    Using the function HoughtLinesP to detect the segment ends in the image.
    
    for more details: https://docs.opencv.org/2.4/modules/imgproc/doc/feature_detection.html?highlight=houghlinesp#void%20HoughLinesP(InputArray%20image,%20OutputArray%20lines,%20double%20rho,%20double%20theta,%20int%20threshold,%20double%20minLineLength,%20double%20maxLineGap)
    
    after Hough transform:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/hough%20transform.png)
    
7. segment screening
    
    Screen the segments according to their slope because the lane is apporximately vertical in the image. I choose to ignore the segments whose slope is less than 1.
    
    before and after screening (in case of shadow section):
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/screen.png)

8. get the center points
    
    In order to have a better detection of the lanes, I translate the left and right points to the center to create the center points. In this way, if one side of points detected is not enough for polyfit, the other side will also help.
    
    before and after getting the center points:
    
    ![image](https://github.com/zhongzebin/lane-departure-warning-system/blob/master/pictures%20for%20readme/center%20points.png)
    
9. polyfit

    I polyfit the center points and then got a quadratic function (lanes are designed according to the quadratic function).
    
    for more details: https://blog.csdn.net/eric_e/article/details/79519436
    
10. smoothing

    The lanes can't change a lot between frames, so I set a group of thresholds. If the change of polyfit parameters between frames is bigger than the thresholds, it can only change to the threshold.
    
    before and after smoothing:
    
    
