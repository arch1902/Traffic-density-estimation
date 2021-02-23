#include "opencv2/opencv.hpp" 
#include<string>

using namespace cv;
using namespace std;

vector<Point2f> corners1, corners2;
Mat gray_image;
Mat display;
Mat im_src;
Mat img1_warp;
void perspective();
int n ;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    
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
    corners2.push_back(Point2f(472,52));
    corners2.push_back(Point2f(472,830));
    corners2.push_back(Point2f(800,830));
    corners2.push_back(Point2f(800,52));

    // Calculate Homography
    Mat H = findHomography(corners1, corners2);

    // Warp source image to destination based on homography
    warpPerspective(gray_image, img1_warp, H, {1280,875});
	cout<<"Size of the source image is "<<im_src.size()<<"\n";

    // Display images
    //imshow("Source Image", gray_image);
    namedWindow("Perspective Image",0);
    imshow("Perspective Image", img1_warp);
    imwrite("Perspective Image.jpg",img1_warp);
}

int main( int argc, char** argv)
{
    // Read source image.
    im_src = imread("empty.jpg");
	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
	display = gray_image.clone();
    // Four corners of the book in source image
	//Create a window
    namedWindow("My Window",0); 
     cout<<"Please select four points on the image : \n";
     //set the callback function for any mouse event
    setMouseCallback("My Window", CallBackFunc, NULL);

     //show the image
	imshow("My Window", display);
    // Wait until user press some key
	waitKey(0);
    if (n!=4){
        cout<<"You did not enter four points\n";
        return 0;
    }
    Mat cropedImage = img1_warp(Rect(472,52,329,779));
    namedWindow("Cropped Image",0);
	imshow("Cropped Image", cropedImage);
    imwrite("Cropped Image.jpg",cropedImage);
    waitKey(0);
    cout << "End of Programme\n";
    
}
