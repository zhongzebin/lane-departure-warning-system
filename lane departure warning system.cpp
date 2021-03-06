#include "pch.h"
#include <iostream>
#include <opencv.hpp>
#include<stdio.h>
#include<windows.h>
#include <mmsystem.h>
#include<dsound.h>
#pragma comment(lib, "WINMM.LIB")
using namespace std;
using namespace cv;
extern "C" Mat rgb2gray(Mat);
Mat img,med,out,inter,roi;
cuda::GpuMat gpu_img,gpu_med,gpu_mask,gpu_org,gpu_gray,gpu_inter;
int i;
Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
Mat distCoeffs = Mat::zeros(8, 1, CV_64F);
//polyfit function
Mat polyfit(std::vector<cv::Point2f> &chain, int n)
{
	Mat y(chain.size(), 1, CV_32F, Scalar::all(0));
	cv::Mat phy(chain.size(), n, CV_32F, Scalar::all(0));
	for (int i = 0; i < phy.rows; i++)
	{
		float* pr = phy.ptr<float>(i);
		for (int j = 0; j < phy.cols; j++)
		{
			pr[j] = pow(chain[i].y, j);
		}
		y.at<float>(i) = chain[i].x;
	}
	Mat phy_t = phy.t();
	Mat phyMULphy_t = phy.t()*phy;
	Mat phyMphyInv = phyMULphy_t.inv();
	Mat a = phyMphyInv * phy_t;
	a = a * y;
	return a;
}
//preparing camera calibration parameters
void camera()
{
	Mat cal[20];//黑白棋盘图像
	vector<Point2f> corners;//每幅图角点
	vector<vector<Point3f>> objectlist(1);//世界坐标系下的棋盘角点
	vector<vector<Point2f>> cornerslist;//所有图中角点
	Mat  out, org;
	int i = 0, j = 0;
	bool found;//是否找到角点
	//根据格子大小生成世界坐标系下角点的坐标，z=0
	for (i = 0; i < 6; ++i)
		for (j = 0; j < 9; ++j)
		{
			objectlist[0].push_back(Point3f(j * 30, i * 30, 0));
		}
	//读取棋盘图像
	cal[0] = imread("calibration1.jpg");
	cal[1] = imread("calibration2.jpg");
	cal[2] = imread("calibration3.jpg");
	cal[3] = imread("calibration4.jpg");
	cal[4] = imread("calibration5.jpg");
	cal[5] = imread("calibration6.jpg");
	cal[6] = imread("calibration7.jpg");
	cal[7] = imread("calibration8.jpg");
	cal[8] = imread("calibration9.jpg");
	cal[9] = imread("calibration10.jpg");
	cal[10] = imread("calibration11.jpg");
	cal[11] = imread("calibration12.jpg");
	cal[12] = imread("calibration13.jpg");
	cal[13] = imread("calibration14.jpg");
	cal[14] = imread("calibration15.jpg");
	cal[15] = imread("calibration16.jpg");
	cal[16] = imread("calibration17.jpg");
	cal[17] = imread("calibration18.jpg");
	cal[18] = imread("calibration19.jpg");
	cal[19] = imread("calibration20.jpg");
	//对每个棋盘图像进行处理
	for (i = 0; i < 20; i++)
	{
		cvtColor(cal[i], cal[i], COLOR_BGR2GRAY);
		//角点检测
		found = findChessboardCorners(cal[i], Size(9, 6), corners, CALIB_CB_ADAPTIVE_THRESH);//输入，（每行角点数，每列角点数），输出角点坐标，检测方式
		if (found)
		{
			//亚像素级别角点提取
			cornerSubPix(cal[i], corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));//输入，角点坐标，检测窗口的一半，死点窗口一半，检测方式
			//将从每个图像中找到的角点存入角点列表
			cornerslist.push_back(corners);
			//角点绘制
			//drawChessboardCorners(cal[i], Size(9, 6), Mat(corners), found);//输入，（每行角点数，每列角点数），角点坐标，判据
		}
	}
	//准备相机标定参数
	objectlist.resize(cornerslist.size(), objectlist[0]);
	vector<Mat> rves, tvecs;
	//进行相机标定
	calibrateCamera(objectlist, cornerslist, cal[0].size(), cameraMatrix, distCoeffs, rves, tvecs);//世界坐标系中的点，像素坐标系中的点，格子大小，相机内参，矫正参数，旋转向量，位移向量
}
int main()
{
	//cuda::printCudaDeviceInfo(0);
	clock_t start, end;
	//read the video
	VideoCapture cap;
	cap.open("project_video.mp4");
	//preparing camera calibration parameters
	camera();
	//build mask for ROI
	cap >> img;
	undistort(img, med, cameraMatrix, distCoeffs);
	//initialize output document
	//VideoWriter videoout("line departure warning.avi", CV_FOURCC('P', 'I', 'M', '1'), 25, med.size(), 1);
	VideoWriter videoout("robustness.avi", CV_FOURCC('P', 'I', 'M', '1'), 25, med.size(), 1);
	cvtColor(med, img, COLOR_BGR2GRAY);
	out = Mat(img.size(), img.type(),Scalar::all(0));
	int i, j;
	for (i = 0; i < img.rows; i++)
		for (j = 0; j < img.cols; j++)
		{
			if (i < img.rows / 1.7 || i>img.rows*0.95)
				out.ptr<uchar>(i)[j] = 255;
			else if (j<(-img.cols*0.83 / img.rows)*i + img.cols*0.97 || j>img.cols / img.rows*i*2.2 - img.cols*0.22)
				out.ptr<uchar>(i)[j] = 255;
		}
	gpu_mask.upload(out);
	//building transform matrix
	vector<Point2f> src(4), dst(4);
	src[0].x = 194;
	src[0].y = 719;
	src[1].x = 1117;
	src[1].y = 719;
	src[2].x = 705;
	src[2].y = 461;
	src[3].x = 575;
	src[3].y = 461;
	dst[0].x = 290;
	dst[0].y = 719;
	dst[1].x = 990;
	dst[1].y = 719;
	dst[2].x = 990;
	dst[2].y = 0;
	dst[3].x = 290;
	dst[3].y = 0;
	Mat m,rm;
	m = getPerspectiveTransform(src, dst);
	rm= getPerspectiveTransform(dst, src);
	//declare Hough lines variants
	cuda::GpuMat gpu_lines;
	cuda::GpuMat lines;
	Mat cpu_lines;
	//building map for warp transform
	Mat I = Mat_<double>::eye(3, 3);
	Mat map1,map2;
	cuda::GpuMat gpu_map1, gpu_map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, I, cameraMatrix,Size(img.cols,img.rows), CV_32FC1,map1,map2);
	gpu_map1.upload(map1);
	gpu_map2.upload(map2);
	//used to store previous polyfit variants
	float pre_a, pre_b, pre_c;
	int flag = 0;
	double offcenter;
	mciSendString(TEXT("open warning.wma alias mysong"), NULL, 0, NULL);
	//image processing
	while (1)
	{
		//read frames from the video
		cap >> img;
		if (img.empty()) break;
		//camera calibration
		gpu_img.upload(img);
		cuda::remap(gpu_img, gpu_med, gpu_map1, gpu_map2, INTER_LINEAR);
		//upload the image into GPU
		gpu_med.copyTo(gpu_img);
		gpu_med.copyTo(gpu_org);
		//convert into gray image
		cuda::cvtColor(gpu_org, gpu_med, COLOR_BGR2HLS);
		cuda::cvtColor(gpu_org, gpu_inter, COLOR_BGR2GRAY);
		vector<cuda::GpuMat> channels;
		cuda::split(gpu_med, channels);
		gpu_med = channels.at(2);//extract S channel
		cuda::addWeighted(gpu_med,0.7, gpu_inter,0.3,0.0, gpu_gray);//gpu_gray=gpu_med*0.7+gpu_inter*0.3
		//Canny detector
		Ptr<cuda::CannyEdgeDetector>Canny_edge = cuda::createCannyEdgeDetector(100, 210);
		Canny_edge->detect(gpu_gray, gpu_img);
		//using mask to extract ROI
		cuda::subtract(gpu_img, gpu_mask, gpu_med);
		//gpu_med.download(roi);
		//wrap transform
		cuda::warpPerspective(gpu_med, gpu_img, m, gpu_med.size());
		//gpu_img.download(inter);
		//imshow("反透视变换", inter);
		//Hough Transform
		Ptr<cuda::HoughSegmentDetector> hough = cuda::createHoughSegmentDetector(2.0, CV_PI / 180, 60, 20);
		hough->detect(gpu_img, gpu_lines);
		//showing and storing the detected points
		cuda::GpuMat change_lines;
		int a = gpu_lines.cols % 4;
		change_lines = gpu_lines.colRange(0, gpu_lines.cols-a);//delete some cols so that the cols can be divided by 4
		lines = change_lines.reshape(0, int(change_lines.cols/4)).clone();//reshape the matrix so that each row indicates a pair of points
		lines.download(cpu_lines);
		Point p1, p2,p3,p4;
		out = Scalar::all(0);
		vector<Point2f> middlepoint;
		//using for loop to store the middlepoints
		for (i = 0; i < cpu_lines.rows; i++)
		{
			p1.x = cpu_lines.ptr<int>(i)[0];
			p1.y = cpu_lines.ptr<int>(i)[1];
			p2.x = cpu_lines.ptr<int>(i)[2];
			p2.y = cpu_lines.ptr<int>(i)[3];
			//using slope to screen the lane points
			double slope= abs(double(p2.x - p1.x) / (p2.y - p1.y));
			if (slope < 1)
			{
				//left points add 330 in x
				if (p1.x < med.cols / 2 && p2.x < med.cols / 2)
				{
					p3.x = p1.x + 330;
					p3.y = p1.y;
					p4.x = p2.x + 330;
					p4.y = p2.y;
					middlepoint.push_back(p3);
					middlepoint.push_back(p4);
				}
				//right points minus 330 in x
				if (p1.x > med.cols / 2 && p2.x > med.cols / 2)
				{
					p3.x = p1.x - 330;
					p3.y = p1.y;
					p4.x = p2.x - 330;
					p4.y = p2.y;
					middlepoint.push_back(p3);
					middlepoint.push_back(p4);
				}
			}
			//draw middle points
			//circle(out, p3, 3, Scalar(255), -1);
			//circle(out, p4, 3, Scalar(255), -1);
		}
		//polyfit the middle points
		Mat mm = polyfit(middlepoint, 3);
		i = 0;
		//initializing canvas
		Mat outt=Mat(med.size(),med.type(),Scalar::all(0));
		cuda::GpuMat gpu_outt;
		//smooth modification
		if (flag == 1)
		{
			if ((mm.ptr<float>(0)[0] - pre_a) > 5)   mm.ptr<float>(0)[0] = pre_a + 5;
			if ((mm.ptr<float>(0)[0] - pre_a) < -5)   mm.ptr<float>(0)[0] = pre_a - 5;
			if ((mm.ptr<float>(1)[0] - pre_b) > 0.02)   mm.ptr<float>(1)[0] = pre_b + 0.02;
			if ((mm.ptr<float>(1)[0] - pre_b) < -0.02)   mm.ptr<float>(1)[0] = pre_b - 0.02;
			if ((mm.ptr<float>(2)[0] - pre_c) > 0.000015)   mm.ptr<float>(2)[0] = pre_c + 0.000015;
			if ((mm.ptr<float>(2)[0] - pre_c) < -0.000015)   mm.ptr<float>(2)[0] = pre_c - 0.000015;
		}
		pre_a = mm.ptr<float>(0)[0];
		pre_b = mm.ptr<float>(1)[0];
		pre_c = mm.ptr<float>(2)[0];
		//using points to show the polyfit lines
		while (i < med.rows)
		{
			j = mm.ptr<float>(0)[0] + mm.ptr<float>(1)[0] * i + mm.ptr<float>(2)[0] * i*i;
			circle(outt, Point(j + 330, i), 10, Scalar(255,255,255), -1);
			circle(outt, Point(j - 330, i), 10, Scalar(255,255,255), -1);
			i = i + 10;
		}
		//calculate offcenter
		offcenter = abs(j - med.cols/2+30);//报警阈值0.6，车道宽3.75，车宽2
		offcenter = offcenter / 660 * 3.75;
		char oo[20];
		sprintf_s(oo, "%.2f", offcenter);//保留两位小数
		string off = oo;
		//upload the dectected lines to GPU
		gpu_outt.upload(outt);
		//warp transform
		cuda::warpPerspective(gpu_outt, gpu_med, rm, gpu_outt.size());
		cuda::subtract(gpu_org,gpu_med, gpu_img);//use black to indicate detected lines
		//download the image into CPU
		gpu_img.download(med);
		//imshow("车道检测",med);
		//calculating and showing fps
		double time;
		end = clock();
		if(flag==1) time = double(end - start);
		start = clock();
		string ff;
		if (flag == 1)
		{
			time = time / CLOCKS_PER_SEC;
			time = 1.0 / time;
			int fre = int(time);
			char kk[20];
			sprintf_s(kk, "%d", fre);
			ff = kk;
			putText(med,"fps:"+ff,Point(med.cols-100,25), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);
		}
		//add text on the imaage
		if (offcenter < 0.6)
		{
			putText(med, "off the center:" + off + "m", Point(0, 25), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);
			//mciSendString(TEXT("close mysong"), NULL, 0, NULL);
		}
		else
		{
			putText(med, "off the center:" + off + "m", Point(0, 25), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4, 8);
			putText(med, "Warning!!!", Point(0, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4, 8);
			mciSendString(TEXT("play mysong"), NULL, 0, NULL);
		}
		videoout << med;
		//put the image into the output video
		//videoout << med;
		//show the image in the screen
		//imshow("ROI", roi);
		//imshow("wrapped image", inter);
		//imshow("detected points", out);
		imshow("lane detection", med);
		waitKey(1);
		flag = 1;
	}
	return 0;
}