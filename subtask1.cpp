#include "opencv2/opencv.hpp" 
#include<string>

using namespace cv;
using namespace std;

vector<Point2f> corners1, corners2;
Mat gray_image;
Mat display;
Mat im_src;
void perspective();
int n ;


int resolution (int x){
    return (x*2)/3;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    imshow("My Window", display);
     if  ( n<4 && (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) ) {
	 			corners1.push_back(Point2f(x, y));
                n+=1;
                String to_display = "("+to_string(x)+","+to_string(y)+")";
                putText(display,to_display,Point2f(x,y),FONT_HERSHEY_SIMPLEX,1, (0,255,0),3);
                imshow("My Window", display);
				cout<<x<<" "<<y<<endl;
        if (n==4){perspective();}
	 }
}

void perspective(){

    // Four corners in destination image.
    corners2.push_back(Point2f(resolution(472),resolution(52)));
    corners2.push_back(Point2f(resolution(472),resolution(830)));
    corners2.push_back(Point2f(resolution(800),resolution(830)));
    corners2.push_back(Point2f(resolution(800),resolution(52)));

    // Calculate Homography
    Mat H = findHomography(corners1, corners2);

    // Warp source image to destination based on homography
    Mat img1_warp;
    warpPerspective(gray_image, img1_warp, H, {resolution(1280),resolution(875)});
	cout<<"Size of the source image is "<<im_src.size()<<"\n";

    // Display images
    //imshow("Source Image", gray_image);
    imshow("Perspective Image", img1_warp);
    imwrite("Perspective Image.jpg",img1_warp);

	Mat cropedImage = img1_warp(Rect(resolution(472),resolution(52),resolution(329),resolution(779)));
	imshow("Cropped Image", cropedImage);
    imwrite("Cropped Image.jpg",cropedImage);
}

int main( int argc, char** argv)
{
    // Read source image.
    im_src = imread("Resized_empty.jpg");
	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
	display = gray_image.clone();
    // Four corners of the book in source image
	//Create a window
    namedWindow("My Window",0); 
     cout<<"Please select four points on the image : \n";
     //set the callback function for any mouse event
    setMouseCallback("My Window", CallBackFunc, NULL);

     //show the image
    // Wait until user press some key
	waitKey(0);
    cout << "End of Programme\n";
    
}