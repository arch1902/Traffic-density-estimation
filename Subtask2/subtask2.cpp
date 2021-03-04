#include "opencv2/opencv.hpp" 
#include<string>
#include<algorithm>

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




int main( int argc, char** argv)
{
    VideoCapture cap("trafficvideo.mp4");
    
    Mat bg = imread("bg.jpg");
    cvtColor( bg, bg, COLOR_BGR2GRAY );
    imshow("imgFrame3", bg);

    if(!cap.isOpened()){
        cout << "Error in opening video file" << endl;
        return -1;
    }



    while(cap.isOpened()){
        cap.read(imgFrame);
        if(imgFrame.empty()) break;
        imshow("imgFrame", imgFrame);
        imgFrame = crop(imgFrame);
        imgFrame = GaussianBlur(imgFrame,Size(21,21), 0)

        // Ptr<BackgroundSubtractor> pBackSub=createBackgroundSubtractorKNN();
        // pBackSub->apply(imgFrame,fgMask);

        // cout<<bg.size()<<" "<<imgFrame.size()<<endl;
        // cout<<bg.channels()<<" "<<imgFrame.channels()<<endl;

        absdiff(bg,imgFrame,imgFrame2);

        imshow("imgFrame2", imgFrame2);
        waitKey(100);
        
    }

    cap.release();
    destroyAllWindows();


}
