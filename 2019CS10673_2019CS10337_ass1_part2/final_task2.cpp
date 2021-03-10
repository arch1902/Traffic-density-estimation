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
//// ---------------SubTask 1 -------------------////


int main( int argc, char** argv)
{
    // Reading Empty BackGround Image //
    Mat bg = imread("bg.jpg");
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
    vector<double> y_axis_q,y_axis_d;

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

        // Dynamic Density--------------------------------------------------------

        // Optical flow
        Mat flow(prvs.size(), CV_32FC2);
        calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        
        // Convert the inmage from cartesian to polar coordinates so that the magnitude of the motion 
        // of individual pixel can be determined 
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));

        // Convert the polar image into hsv image 
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
        // To reduce and handle noise in the image 
        threshold( bgr, thresh,10,255,0);
        dilate(thresh,dilated,0,Point(-1,-1),3);

        //cout << dilated.total();

        double Ddensity = countNonZero(dilated)/(double)dilated.total();

        // Queue Density-------------------------------------------------------------

        //Applying Gaussion blur to smoothen the image 
        GaussianBlur(next,imgFrame,Size(21,21), 0);
        // Substracting the two image
        absdiff(bg,imgFrame,imgFrame2);

        Mat thresh1,dilated1;
        // To reduce and handle noise in the image
        threshold( imgFrame2, thresh1, 30, 255, 0);
        dilate(thresh1,dilated1, 0, Point(-1,-1),2);
    ;    

        double Qdensity = countNonZero(dilated1)/(double)dilated1.total();   

        //-----------------------------------------------------------------------------


        x_axis.push_back(time);
        y_axis_q.push_back(Qdensity);
        y_axis_d.push_back(Ddensity);

        // Applying rolling statistics to handle the noise
        int i = y_axis_d.size() -1;
        float s = 0;
        if(y_axis_d.size()>3){
        for(int j = 0;j<3;j++){
            s += y_axis_d[i-j];
        }}
        
        Ddensity=s/(float)3;

        myfile<<frame<<","<<time<<","<<Qdensity<<","<<Ddensity<<endl;
        cout<<frame<<" , "<<Qdensity<<" , "<<Ddensity<<endl;


        // Ploting the live curve 
        /*Mat x(x_axis,true);
        Mat y_q(y_axis_q,true);
        Mat y_d(y_axis_d,true);
        x.convertTo(x, CV_64F);

        Ptr<plot::Plot2d> plot = plot::Plot2d::create(x,y_q);
        plot->setShowText(true);
        plot->setInvertOrientation(true);   
        plot->setShowGrid(false);
        plot->render(plot_result); 
        plot->setMaxX(400);
        plot->setMaxY(0.8);
        plot->setPlotLineColor(Scalar(0,0,255));

        Ptr<plot::Plot2d> plot2 = plot::Plot2d::create(x,y_d);
        plot2->setShowText(true);
        plot2->setInvertOrientation(true);   
        plot2->setShowGrid(false);
        plot2->render(plot_result2);
        plot2->setMaxX(400);
        plot2->setMaxY(0.8);
        plot2->setPlotLineColor(Scalar(0,255,0));

        
        imshow("Graph_Queue", plot_result);
        imshow("Graph_Dynamic", plot_result2);*/
        waitKey(30);
        prvs = next;
        time += 0.2;
    }
    myfile.close();
}
