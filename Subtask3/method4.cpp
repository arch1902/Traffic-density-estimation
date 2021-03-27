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


void* qdensity(void* src1){
        struct image_info *img= (struct image_info*) src1;
        Mat frame2 = img->frame;
        Mat thresh1,dilated1,imgFrame2,imgFrame;
        //Applying Gaussion blur to smoothen the image 
        GaussianBlur(frame2,imgFrame,Size(21,21), 0);
        // Substracting the two image
        absdiff(bg,imgFrame,imgFrame2);
        // To reduce and handle noise in the image
        threshold( imgFrame2, thresh1, 30, 255, 0);
        dilate(thresh1,dilated1, 0, Point(-1,-1),2); 
        double q = countNonZero(dilated1)/(double)dilated1.total();
        img->density = q;
        pthread_exit(NULL);
}

int main( int argc, char** argv)
{
    bg = imread("bg.jpg");
    cvtColor( bg, bg, COLOR_BGR2GRAY );
    GaussianBlur(bg,bg,Size(21,21), 0);

    int num_threads = stoi(argv[2]);

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
    bool end = true;
    while (true)
    {
        Mat frames[num_threads];
        int count = 0;
        for(int i = 0 ;i<num_threads;i++){
            capture>>frames[i];
            if(!frames[i].empty()){frames[i] = crop(frames[i]);count ++;}
        }
        if(count!=num_threads){end = false;}
        struct image_info args[count];
        pthread_t tids[count];
	    for (int i = 0; i < count; i++) {
            args[i].frame = frames[i];
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tids[i], &attr, qdensity, &args[i]);
	}

	// Wait until thread is done its work
	for (int i = 0; i < count; i++) {
		pthread_join(tids[i], NULL);
        y_axis_q.push_back(args[i].density);
        x_axis.push_back(time);
        time+= 0.0667;
	}
    if(!end){break;}
    }
    
    ofstream myfile;
    myfile.open ("out.csv");
    myfile<<"density,time"<<endl;
    for(int j = 0;j<x_axis.size();j++){
        myfile<<y_axis_q[j]<<","<<x_axis[j]<<endl;
    }
    myfile.close();
}
