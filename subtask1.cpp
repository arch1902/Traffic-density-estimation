#include "opencv2/opencv.hpp" 

using namespace cv;
using namespace std;

vector<Point2f> corners1, corners2;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     if  ( event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) {
	 			corners1.push_back(Point2f(x, y));
				cout<<x<<" "<<y<<endl;
	 }
}

int main( int argc, char** argv)
{
    // Read source image.
    Mat im_src = imread("empty.jpg");
	Mat gray_image;
	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
	
    // Four corners of the book in source image
	//Create a window
     namedWindow("My Window", 1);

     //set the callback function for any mouse event
     setMouseCallback("My Window", CallBackFunc, NULL);

     //show the image
     imshow("My Window", gray_image);

    // Wait until user press some key
     waitKey(0);

    // Four corners in destination image.
    corners2.push_back(Point2f(472,52));
    corners2.push_back(Point2f(472, 830));
    corners2.push_back(Point2f(800, 830));
    corners2.push_back(Point2f(800, 52));

    // Calculate Homography
    Mat H = findHomography(corners1, corners2);

    // Warp source image to destination based on homography
    Mat img1_warp;
    warpPerspective(gray_image, img1_warp, H, {1280,875});
	cout<<im_src.size();

    // Display images
    imshow("Source Image", gray_image);
    imshow("Warped Source Image", img1_warp);

    waitKey(0);

	Mat cropedImage = img1_warp(Rect(472,52,329,779));
	imshow("Cropped Image", cropedImage);

	waitKey(0);

}