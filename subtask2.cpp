#include "opencv2/opencv.hpp" 
#include<string>
#include<algorithm>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/plot.hpp>

#include <fstream>


using namespace cv;
using namespace std;

Mat imgFrame,imgFrame2,test,prvs,thresh,dilated,ne;

vector<Point2f> corners1, corners2;
Mat gray_image;
Mat img1_warp;
Mat display;
string Image_name;
void perspective();
int n ;

bool compx( Point2f a, Point2f b ){
        return a.x < b.x;
}
bool compy( Point2f a, Point2f b ){
        return a.y < b.y;
}


void perspective(){

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

    // Warp source image to destination based on homography
    warpPerspective(gray_image, img1_warp, H, {1280,875});

}

//

Mat crop(Mat im_src){

	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
    perspective();

    Mat croppedImage = img1_warp(Rect(472,52,329,779));
    return croppedImage;
}

int main( int argc, char** argv)
{
    VideoCapture cap("trafficvideo.mp4");
    
    if(!cap.isOpened()){
        cout << "Error in opening video file" << endl;
        return -1;
    }

    double time=0;
    vector<int> x_axis;
    vector<double> y_axis;
    int frame_num=0;

    cap.read(imgFrame2);
    //prvs = crop(imgFrame2);
    cvtColor( imgFrame2,prvs, COLOR_BGR2GRAY );


    ofstream myfile;
    myfile.open ("test.csv");
    
    while(true){
        cap.read(imgFrame);

        frame_num+=1;
        if(frame_num%5!=1) continue;
        if(imgFrame.empty()) break;

        //Mat next = crop(imgFrame);
        cvtColor( imgFrame, ne , COLOR_BGR2GRAY);

        namedWindow("imgFrame",0);
        imshow("imgFrame", ne);
        
        Mat flow(prvs.size(), CV_32FC2);

        calcOpticalFlowFarneback(prvs, ne, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        prvs = ne;
        //prvs = ne;
        //calcOpticalFlowPyrLK(prvs,prvs,next,flow, status, err, Size(15,15), 2, criteria);
        //prvs = next;
        //cvtColor(flow,flow,COLOR_BGR2GRAY);
        //namedWindow("f",0);
        //imshow("f", flow);
        // visualization
        Mat flow_parts[2];
        split(flow, flow_parts);

        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        namedWindow("frame2",0);
        imshow("frame2", bgr);

        //threshold( bgr, thresh, 30, 255, 0);
        //dilate(thresh,dilated, 0, Point(-1,-1),2);
        bgr = crop(bgr);


        //imshow("dilate", dilated);
        //cvtColor(bgr,dilated,COLOR_BGR2GRAY);

        double density = countNonZero(bgr)/256291.0;
        myfile<<frame_num<<","<<density<<","<<time<<endl;
        x_axis.push_back(time);
        y_axis.push_back(density);

        waitKey(100);

        time+=0.2;
    }

    myfile.close();


    Mat x(x_axis,true);
    Mat y(y_axis,true);
    x.convertTo(x, CV_64F);
    x.convertTo(x, CV_64F);

    Ptr<plot::Plot2d> plot = plot::Plot2d::create(x,y);
    Mat plot_result ;

    plot->setShowText(true);

    plot->setInvertOrientation(true);   
    plot->setShowGrid(false);
    plot->render(plot_result); 

    imshow("Graph", plot_result);
    imwrite("plot.jpg",plot_result);

}