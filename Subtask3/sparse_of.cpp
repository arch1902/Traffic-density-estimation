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

using namespace cv;
using namespace std;
Mat imgFrame,imgFrame2;

vector<Point2f> corners1, corners2;
Mat plot_result, plot_result2;
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
int main(int argc, char **argv)
{
    vector<int> y_axis_d;
    // Read the video 
    VideoCapture capture(argv[1]);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }

    // Create random colors
    vector<Scalar> colors;
    RNG rng;
    for(int i = 0; i < 100; i++)
    {
        int r = rng.uniform(0, 256);
        int g = rng.uniform(0, 256);
        int b = rng.uniform(0, 256);
        colors.push_back(Scalar(r,g,b));
    }

    Mat old_frame, old_gray;
    vector<Point2f> p0, p1;

    // Read first frame and find corners in it
    capture >> old_frame;
    old_gray=crop(old_frame);
    goodFeaturesToTrack(old_gray, p0, 1000, 0.3, 7, Mat(), 7, false, 0.04);

    // Create a mask image for drawing purposes
    Mat mask = Mat::zeros(old_gray.size(), old_gray.type());
    int framenum=1;
    // Output File
    ofstream myfile;    
    myfile.open ("sparse.csv");
    myfile<<"frame,time,dynamic density\n";
    double time =0;
    while(true){
        framenum++;
        // Read new frame
        Mat frame, frame_gray;
        capture >> frame;
        if(framenum%3!=1) continue;
        if (frame.empty())
            break;
        frame_gray = crop(frame);
        
        goodFeaturesToTrack(frame_gray, p0, 1000, 0.2, 7, Mat(), 7, false, 0.04);
        // Calculate optical flow
        vector<uchar> status;
        vector<float> err;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);

        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15,15), 2, criteria);
        
        vector<Point2f> good_new;

        
        // Visualization part
        float count = 0;
        for(uint i = 0; i < p0.size(); i++)
        {
            // Select good points
            if(status[i] == 1) {
                double res = cv::norm(p0[i]-p1[i]);
                if(res>2.3){count++;
                
                circle(frame_gray, p1[i], 5, colors[i], -1);}
            }
        }

        // Display the demo
        Mat img;
        add(frame_gray, mask, img);
        imshow("flow", img);
        // int keyboard = waitKey(25);
        // if (keyboard == 'q' || keyboard == 27)
        //     break;

        y_axis_d.push_back(count);

        // Applying rolling statistics to handle the noise
        int i = y_axis_d.size() -1;
        float s = 0;
        if(y_axis_d.size()>3){
        for(int j = 0;j<3;j++){
            s += y_axis_d[i-j];
        }}
        
        count=s/(float)3;

        // Update the previous frame and previous points
        old_gray = frame_gray;
        myfile<<framenum<<","<<time<<","<<(float)count/250<<endl;
        time+=0.2;

    }

}
