#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include "opencv2/opencv.hpp" 
#include <string>
#include <algorithm>
#include <opencv2/plot.hpp>
#include <fstream>
#include <pthread.h>
#include <chrono>
#include <sstream>

using namespace std::chrono;
using namespace cv;
using namespace std;
Mat bg;

vector<Point2f> corners1, corners2;
Mat plot_result, plot_result2;
int NUM_THREADS ;

vector<Mat> split_bg;
vector<Mat> split_images;


fstream a;

double Qdensity;
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


int num_row;
int num_col;


void* qdensity(void* tid){
        

        Mat thresh1,dilated1,imgFrame2,imgFrame;
        // Mat src = Mat(row_size, col_size, CV_16U, &src1);
        long i;
        i= (long)tid;
        
        //Applying Gaussion blur to smoothen the image 
        GaussianBlur(split_images[i],imgFrame,Size(21,21), 0);
        // Substracting the two image
        
        absdiff(split_bg[i],imgFrame,imgFrame2);
        // To reduce and handle noise in the image
        threshold( imgFrame2, thresh1, 30, 255, 0);
        dilate(thresh1,dilated1, 0, Point(-1,-1),2); 
        double q= countNonZero(dilated1)/(double)dilated1.total();
        //cout<<q<<endl;
        Qdensity+=q;
        //cout<<Qdensity<<endl;
        pthread_exit(NULL);
}

int main( int argc, char** argv)
{
    NUM_THREADS = stoi(argv[2]);
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

    split_bg.resize(NUM_THREADS);
    split_images.resize(NUM_THREADS);

    auto start = high_resolution_clock::now();
    // Reading Empty BackGround Image //
    bg = imread("bg.jpg");

    num_row = bg.rows;
    num_col = bg.cols/NUM_THREADS;

    cvtColor( bg, bg, COLOR_BGR2GRAY );
    GaussianBlur(bg,bg,Size(21,21), 0);

    for(int i=0;i<NUM_THREADS;i++){
    split_bg[i] = bg(Rect(i*num_col,0,num_col,num_row));
    }

    // Taking videofile Input//
    VideoCapture capture(argv[1]);
    // NUM_THREADS =  atoi(argv[2]);
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
    myfile.open ("out_"+to_string(NUM_THREADS)+".csv");



       // Initialize and set thread joinable
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


 
        for(int i=0;i<NUM_THREADS;i++){
        split_images[i] = next(Rect(i*num_col,0,num_col,num_row));
        
        }
        
        row_size = split_images[0].rows;
        col_size = split_images[0].cols;

        // row_size = next.rows;
        // col_size = next.cols;
        Qdensity = 0;

        // Queue Density-------------------------------------------------------------

        int rc,i;
        void *status;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        for(i = 0; i < NUM_THREADS; i++ ) {
            
            //cout << "main() : creating thread, " << i << endl;
            rc = pthread_create(&threads[i], &attr,qdensity, (void *)i );
            if (rc) {
                cout << "Error:unable to create thread," << rc << endl;
                exit(-1);
            }
        }
        //cout<<"hello"<<endl;
        // free attribute and wait for the other threads
        pthread_attr_destroy(&attr);
        for( i = 0; i < NUM_THREADS; i++ ) {
            rc = pthread_join(threads[i], &status);
            
            if (rc) {
                cout << "Error:unable to join," << rc << endl;
                exit(-1);
            }
            //cout << "Main: completed thread id :" << i ;
            //cout << "  exiting with status :" << status << endl;
        }

        //cout << "Main: program exiting." << endl;
       


        //-----------------------------------------------------------------------------


        x_axis.push_back(time);
        y_axis_q.push_back(Qdensity/NUM_THREADS);

        myfile<<frame<<","<<time<<","<<Qdensity/NUM_THREADS<<endl;
        // cout<<frame<<" "<<time<<" "<<Qdensity/NUM_THREADS<<endl;

        // Ploting the live curve 
        Mat x(x_axis,true);
        Mat y_q(y_axis_q,true);
        x.convertTo(x, CV_64F);

        prvs = next;
        time += 0.2;
        // if(frame==31) break;

    }
    capture.release();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    myfile<<endl;
    myfile<< "Time taken using "<<NUM_THREADS<<"Threads : "<<duration.count()/pow(10,6) << endl;
    cout<< "Time taken : "<<duration.count()/pow(10,6) << endl;
    
    myfile.close();
    

    // Error calculation

    double error = 0, qd;
    int i=0;
    vector<string> row;
    string temp,line,word;



    a.open("baseline.csv",ios::in);
    if(!a.is_open()){cout<<"File not found"<<endl;exit(-1);}

    int j=0;
    
    while(getline(a, line)){
        j++;
        if(j%3!=1) continue;
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
    error = error / (i);
    cout<<"Mean Squared Error : "<<error<<endl;
    a.close();

}
