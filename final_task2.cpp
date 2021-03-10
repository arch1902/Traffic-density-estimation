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

using namespace cv;
using namespace std;
Mat imgFrame,imgFrame2;

vector<Point2f> corners1, corners2;
Mat display;
Mat plot_result ;
string Image_name;
void perspective();
int n ;

bool compx( Point2f a, Point2f b ){
        return a.x < b.x;
    }
bool compy( Point2f a, Point2f b ){
        return a.y < b.y;
    }


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

//

Mat crop(Mat im_src){
    Mat gray_image;
	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
    Mat img1_warp = perspective(gray_image);
    //gray_image = im_src;
    Mat croppedImage = img1_warp(Rect(472,52,329,779));
    GaussianBlur(croppedImage,croppedImage,Size(21,21), 0);
    return croppedImage;
}

void average(vector<double> p ){
    int i = p.size() -1;
    float s = 0;
    for(int j = 0;j<20;j++){
        s += p[i-j];
    }
    p[i] = s/(float)20;
}


int main()
{
    Mat bg = imread("bg.jpg");
    cvtColor( bg, bg, COLOR_BGR2GRAY );
    GaussianBlur(bg,bg,Size(21,21), 0);



    VideoCapture capture("trafficvideo.mp4");
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    Mat frame1, prvs;
    capture >> frame1;
    prvs = crop(frame1);
    int frame = 0 ;
    double time=0;
    vector<int> x_axis;
    vector<double> y_axis;

    ofstream myfile;
    myfile.open ("please.csv");
    
    //cvtColor(frame1, prvs, COLOR_BGR2GRAY);
    while(true){
        Mat frame2, next;
        capture >> frame2;
        frame +=1;
        if (frame%3 !=1){continue;}
        if (frame2.empty())
            break;
        //cvtColor(frame2, next, COLOR_BGR2GRAY);
        next = crop(frame2);
        namedWindow("d",0);
        imshow("d",next);

        // Dynamic Density--------------------------------------------------------

        Mat flow(prvs.size(), CV_32FC2);
        calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
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
        cvtColor(bgr,bgr,COLOR_BGR2GRAY);
        Mat thresh,dilated;
        //adaptiveThreshold(bgr,thresh,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,11,10);
        threshold( bgr, thresh,20,255,0);
        dilate(thresh,dilated,0,Point(-1,-1),2);
        namedWindow("frame2",0);
        imshow("frame2", dilated);

        double Ddensity = 2*countNonZero(dilated)/256291.0;

        // Queue Density-------------------------------------------------------------

        GaussianBlur(next,imgFrame,Size(21,21), 0);

        absdiff(bg,imgFrame,imgFrame2);

        Mat thresh1,dilated1;

        threshold( imgFrame2, thresh1, 30, 255, 0);
        dilate(thresh1,dilated1, 0, Point(-1,-1),2);
        imshow("dilate", dilated1);    

        double Qdensity = countNonZero(dilated1)/256291.0;   

        //-----------------------------------------------------------------------------



        myfile<<frame<<","<<Qdensity<<","<<Ddensity<<","<<time<<endl;
        cout<<frame<<" , "<<Qdensity<<" , "<<Ddensity<<endl;

        x_axis.push_back(time);
        y_axis.push_back(Qdensity);

        //if (y_axis.size()>20){average(y_axis);}

        Mat x(x_axis,true);
        Mat y(y_axis,true);
        x.convertTo(x, CV_64F);

        Ptr<plot::Plot2d> plot = plot::Plot2d::create(x,y);
        plot->render(plot_result); 

        imshow("Graph", plot_result);

        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
        prvs = next;
        time += 0.2;
    }
    myfile.close();


    imwrite("plot.jpg",plot_result);
}
