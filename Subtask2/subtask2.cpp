#include "opencv2/opencv.hpp" 
#include <opencv2/plot.hpp>
#include<string>
#include<algorithm>
#include<iostream>

using namespace cv;
using namespace std;

Mat imgFrame,imgFrame2;

// Cropping 

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




int main()
{
    
    Mat bg = imread("bg.jpg");
    cvtColor( bg, bg, COLOR_BGR2GRAY );
    imshow("imgFrame3", bg);

    GaussianBlur(bg,bg,Size(21,21), 0);

    VideoCapture cap("trafficvideo.mp4");

    if(!cap.isOpened()){
        cout << "Error in opening video file" << endl;
        return -1;
    }

    double time=0;
    vector<int> x_axis;
    vector<double> y_axis;
    int frame_num=0;

    while(cap.isOpened()){
        cap.read(imgFrame);
        frame_num+=1;
        if(frame_num%3!=1) continue;
        if(imgFrame.empty()) break;
        imshow("imgFrame", imgFrame);
        imgFrame = crop(imgFrame);
        imshow("gray", imgFrame);
        GaussianBlur(imgFrame,imgFrame,Size(21,21), 0);
        
        // Ptr<BackgroundSubtractor> pBackSub=createBackgroundSubtractorKNN();
        // pBackSub->apply(imgFrame,fgMask);

        // cout<<bg.size()<<" "<<imgFrame.size()<<endl;
        // cout<<bg.channels()<<" "<<imgFrame.channels()<<endl;

        absdiff(bg,imgFrame,imgFrame2);

        Mat thresh,dilated;

        imshow("diff", imgFrame2);
        threshold( imgFrame2, thresh, 30, 255, 0);
        dilate(thresh,dilated, 0, Point(-1,-1),2);



        imshow("dilate", dilated);

        double density = countNonZero(dilated)/256291.0;
        cout<<frame_num<<","<<density<<","<<time<<endl;
        x_axis.push_back(time);
        y_axis.push_back(density);

        waitKey(1);

        time+=0.2;


       


        
        
    }
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



    cap.release();
    destroyAllWindows();


}



