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
#include <thread>

using namespace cv;
using namespace std;
Mat bg;

vector<Point2f> corners1, corners2;
Mat plot_result, plot_result2;
int Number_of_threads =  2;
int Qdensity;
int row_size;
int col_size;
void perspective();

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


void qdensity(Mat src){
        Mat thresh1,dilated1,imgFrame2,imgFrame;
        //Applying Gaussion blur to smoothen the image 
        GaussianBlur(src,imgFrame,Size(21,21), 0);
        // Substracting the two image
        absdiff(bg,imgFrame,imgFrame2);
        // To reduce and handle noise in the image
        threshold( imgFrame2, thresh1, 30, 255, 0);
        dilate(thresh1,dilated1, 0, Point(-1,-1),2); 
        double qdensity = countNonZero(dilated1)/(double)dilated1.total();
        Qdensity+= qdensity;
}

int main( int argc, char** argv)
{
    // Reading Empty BackGround Image //
    bg = imread("bg.jpg");
    cvtColor( bg, bg, COLOR_BGR2GRAY );
    GaussianBlur(bg,bg,Size(21,21), 0);

    // Taking videofile Input//
    VideoCapture capture(argv[1]);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    // first image frame for the optical flow
    Mat frame1, prvs;
    capture >> frame1;
    prvs = crop(frame1);

    //Ploting variables 
    int frame = 0 ;
    double time=0;
    vector<int> x_axis;
    vector<double> y_axis_q;

    // Output File
    ofstream myfile;
    myfile.open ("out.txt");
    
    while(true){
        Mat frame2, next;
        capture >> frame2;

        // Calculating at 5 Frames per second
        frame +=1;
        if (frame%3 !=1){continue;}


        // frame2=(frame2+prev1+prev2)/3;


        if (frame2.empty())
            break;
        next = crop(frame2);
        namedWindow("Gray Video",0);
        imshow("Gray Video",next);

        Mat split_images[Number_of_threads];
        split(next,split_images);
        row_size = split_images[0].rows;
        col_size = split_images[0].cols;
        Qdensity = 0;

        // Queue Density-------------------------------------------------------------
        //-----------------------------------------------------------------------------


        x_axis.push_back(time);
        y_axis_q.push_back(Qdensity);

        myfile<<frame<<","<<time<<","<<Qdensity<<endl;



        // Ploting the live curve 
        Mat x(x_axis,true);
        Mat y_q(y_axis_q,true);
        x.convertTo(x, CV_64F);

        Ptr<plot::Plot2d> plot = plot::Plot2d::create(x,y_q);
        plot->setShowText(true);
        plot->setInvertOrientation(true);   
        plot->setShowGrid(false);
        plot->render(plot_result); 
        plot->setMaxX(400);
        plot->setMaxY(0.8);
        plot->setPlotLineColor(Scalar(0,0,255));


        
        imshow("Graph_Queue", plot_result);
        waitKey(30);
        prvs = next;
        time += 0.2;
    }
    myfile.close();
}
