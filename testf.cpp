#include "opencv2/opencv.hpp" 
#include<string>
#include<algorithm>

#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

Mat imgFrame,imgFrame2,test;

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

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                    double, const Scalar& color)
{
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
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
    
    //Mat bg = imread("bg.jpg");
    //cvtColor( bg, bg, COLOR_BGR2GRAY );
    //imshow("imgFrame3", bg);

    if(!cap.isOpened()){
        cout << "Error in opening video file" << endl;
        return -1;
    }

    Mat flow, cflow, frame;
    UMat gray, prevgray, uflow;
    namedWindow("flow", 1);

    for(;;)
    {
        cap >> frame;
        frame = crop(frame);
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        if( !prevgray.empty() )
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
            uflow.copyTo(flow);
            drawOptFlowMap(flow, cflow, 16, 1.5, Scalar(0, 255, 0));
            namedWindow("flow",0);
            imshow("flow", cflow);
        }
        if(waitKey(30)>=0)
            break;
        std::swap(prevgray, gray);
    }

        waitKey(100);
    }
