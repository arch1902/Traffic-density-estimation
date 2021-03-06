#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/bgsegm.hpp>
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




const char* params
    = "{ help h         |           | Print usage }"
      "{ input          | vtest.avi | Path to a video or a sequence of image }"
      "{ algo           | MOG2      | Background subtraction method (KNN, MOG2) }";
int main(int argc, char* argv[])
{
    CommandLineParser parser(argc, argv, params);
    parser.about( "This program shows how to use background subtraction methods provided by "
                  " OpenCV. You can process both videos and images.\n" );
    if (parser.has("help"))
    {
        //print help information
        parser.printMessage();
    }
    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = bgsegm::createBackgroundSubtractorMOG();
    //pBackSub = createBackgroundSubtractorKNN(300,200.0,false);
    VideoCapture capture( "trafficvideo.mp4" );

    double time=0;
    vector<int> x_axis;
    vector<double> y_axis;
    int frame_num=0;

    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open: " << parser.get<String>("input") << endl;
        return 0;
    }

    ofstream myfile;
    myfile.open ("test.csv");

    Mat frame, fgMask;
    while (true) {
        capture >> frame;
        if (frame.empty())
            break;
        frame_num+=1;
        if(frame_num%3!=1) continue;
        frame = crop(frame);
        GaussianBlur(frame,frame,Size(21,21), 0);
        //update the background model
        pBackSub->apply(frame, fgMask);
        //get the frame number and write it on the current frame
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),cv::Scalar(255,255,255), -1);
        stringstream ss;
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //show the current frame and the fg masks
        namedWindow("Frame",0);
        namedWindow("FG Mask",0);
        imshow("Frame", frame);
        imshow("FG Mask", fgMask);
        //get the input from the keyboard

        threshold( fgMask, thresh, 30, 255, 0);
        namedWindow("threshold",0);
        imshow("threshold", thresh);

        dilate(thresh,dilated, 0, Point(-1,-1),2);
        namedWindow("dilate",0);
        imshow("dilate", dilated);       



        //imshow("dilate", dilated);    myfile << "Writing this to a file.\n";

        double density = countNonZero(dilated)/256291.0;
        myfile<<frame_num<<","<<density<<","<<time<<endl;
        x_axis.push_back(time);
        y_axis.push_back(density);

        int keyboard = waitKey(30);
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


    return 0;
}