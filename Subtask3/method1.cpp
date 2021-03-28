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
#include <sstream>

using namespace std::chrono;
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
    auto start = high_resolution_clock::now();
    // Reading Empty BackGround Image //
    Mat bg = imread("bg.jpg");
    int x = strtol(argv[2], nullptr, 0);
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
    myfile.open ("out_"+to_string(x)+"f.txt");
    
    while(true){
        Mat frame2, next;
        capture >> frame2;

        // Calculating at 15/x Frames per second
        frame +=1;
        if (frame%x!=1){continue;}


        // frame2=(frame2+prev1+prev2)/3;


        if (frame2.empty())
            break;
        next = crop(frame2);
        namedWindow("Gray Video",0);
        imshow("Gray Video",next);

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

        // Applying rolling statistics to handle the noise


        myfile<<frame<<","<<time<<","<<Qdensity<<endl;
        //cout<<frame<<" , "<<Qdensity<<endl;


        prvs = next;
        time += (double)x/15;
    }
    myfile.close();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    myfile<< "Time taken processing every "<<x<<" frame : "<<duration.count()/pow(10,6) << endl;
    cout<<"Time taken processing every "<<x<<"th frame : "<<duration.count()/pow(10,6) << endl;

    // Error Calc


    double error = 0, qd;
    int i=0;
    vector<string> row;
    string temp,line,word;

    fstream a;
    a.open("baseline.csv",ios::in);
    if(!a.is_open()){cout<<"File not found"<<endl;exit(-1);}

    int j=0;
    
    while(getline(a, line)){
        j++;
        if(j%x!=1) continue;
        row.clear();
        stringstream s(line);
        while (s.good()) {
            getline(s, word, ',');
            row.push_back(word);
            
        }
        
        qd = stod(row[2]);
        //cout<<qd<<" "<<y_axis_q[i]<<endl;
        
        error+= pow(qd-y_axis_q[i++],2);

    }
    // cout<<i<<endl;
    // cout<<error<<endl;
    error = error / (i);
    cout<<"Mean Squared Error : "<<error<<endl;
    a.close();
}
