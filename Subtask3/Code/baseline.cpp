#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include "opencv2/opencv.hpp" 
#include<string>
#include<algorithm>
#include <opencv2/plot.hpp>
#include <fstream>
#include <pthread.h>
#include <chrono>
#include <sstream>

using namespace std::chrono;
using namespace cv;
using namespace std;
Mat bg;

vector<Point2f> corners1, corners2;
void perspective();

struct image_info
{
    /* data */
    Mat frame;
    double density;
};


//// ---------------SubTask 1 -------------------////
Mat perspective(Mat gray_image ){

    corners1.push_back(Point2f(967,205));
    corners1.push_back(Point2f(241,1066));
    corners1.push_back(Point2f(1612,1059));
    corners1.push_back(Point2f(1283,208));

    // Four corners in destination image.
    corners2.push_back(Point2f(472,52));
    corners2.push_back(Point2f(472,830));
    corners2.push_back(Point2f(800,830));
    corners2.push_back(Point2f(800,52));

    // Calculate Homography
    Mat H = findHomography(corners1, corners2);
    Mat p;
    // Warp source image to destination based on homography
    warpPerspective(gray_image, p, H, {1280,875});
    return p;
}

Mat crop(Mat im_src){
    Mat gray_image;
	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
    Mat img1_warp = perspective(gray_image);
    //gray_image = im_src;
    Mat croppedImage = img1_warp(Rect(472,52,329,779));
    // GaussianBlur(croppedImage,croppedImage,Size(21,21), 0);
    return croppedImage;
}
//// ---------------SubTask 1 -------------------////

int main( int argc, char** argv)
{
    auto start = high_resolution_clock::now();
    bg = imread("bg.jpg");
    cvtColor( bg, bg, COLOR_BGR2GRAY );
    GaussianBlur(bg,bg,Size(21,21), 0);

    double time=0;
    vector<double> x_axis;
    vector<double> y_axis_q;

    // Taking videofile Input//
    VideoCapture capture(argv[1]);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    Mat frame2;
    while (true)
    {
        capture>>frame2;
        if(frame2.empty()){
            break;
        }
        frame2 = crop(frame2);
        Mat thresh1,dilated1,imgFrame2,imgFrame;
        //Applying Gaussion blur to smoothen the image 
        GaussianBlur(frame2,imgFrame,Size(21,21), 0);
        // Substracting the two image
        absdiff(bg,imgFrame,imgFrame2);
        // To reduce and handle noise in the image
        threshold( imgFrame2, thresh1, 30, 255, 0);
        dilate(thresh1,dilated1, 0, Point(-1,-1),2); 
        double q = countNonZero(dilated1)/(double)dilated1.total();
        y_axis_q.push_back(q);
        x_axis.push_back(time);
        time += 0.067;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout<< "Time taken : "<<duration.count()/pow(10,6) << endl;

    ofstream myfile;
    myfile.open ("baseline.csv");
    myfile<<"Frame,Time,Density"<<endl;
    for(int j = 0;j<x_axis.size();j++){
        myfile<<j+1<<","<<x_axis[j]<<","<<y_axis_q[j]<<endl;
    }
    myfile.close();
}
