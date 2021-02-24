#include "opencv2/opencv.hpp" 
#include<string>
#include<algorithm>


using namespace cv;
using namespace std;

vector<Point2f> corners1, corners2;
Mat gray_image;
Mat display;
Mat im_src;
Mat img1_warp;
string Image_name;
void perspective();
int n ;

bool compx( Point2f a, Point2f b ){
        return a.x < b.x;
}
bool compy( Point2f a, Point2f b ){
        return a.y < b.y;
}


void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{ 
     if  ( n<4 && (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) ) {
        corners1.push_back(Point2f(x, y));
        n+=1;
        String to_display = "("+to_string(x)+","+to_string(y)+")";
        putText(display,to_display,Point2f(x,y),FONT_HERSHEY_SIMPLEX,1,{0,255,0},3);
        imshow("My Window", display);
        cout<<x<<" "<<y<<endl;
        if (n==4){
            //Feeds the corners in correct order even when the user inputs them in any order
            sort(corners1.begin(),corners1.end(),compx);
            sort(corners1.begin(),corners1.begin()+2,compy);
            sort(corners1.begin()+2,corners1.end(),compy);
            reverse(corners1.begin()+2,corners1.end());
            //for(int i = 0;i<4;i++){
            //    cout<<corners1[i].x<<" "<<corners1[i].y<<endl;
            //}
            perspective();
            return;
        }
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
	//cout<<"Size of the source image is "<<im_src.size()<<"\n";

    // Create a window for the perspective image and saves it
    namedWindow("Perspective Image",0);
    imshow("Perspective Image", img1_warp);
    imwrite(Image_name+"_Perspective Image.jpg",img1_warp);

    cout<<"Press any key to obtain the cropped image\n";
}

int main( int argc, char** argv)
{
    // Read source image.
    Image_name = argv[1];
    Image_name = Image_name.substr(0,Image_name.size()-4);
    im_src = imread(argv[1]);

    //If the asked file is not read
    if(im_src.empty()){
        cout<<"Cannot open the image file\n";
        return -1;
    }

    //converts the image to gray scale from RGB/BGR
	cvtColor( im_src, gray_image, COLOR_BGR2GRAY );
	display = gray_image.clone();

	//Create a window
    namedWindow("My Window",0); 
     cout<<"Please select four points on the image : \n";
     //set the callback function for any mouse event
    setMouseCallback("My Window", CallBackFunc, NULL);

     //show the image
	imshow("My Window", display);

    // Wait until user press some key after the perspective image is displayed
	waitKey(0);

    //If the user did not enter four point throw the code will not procced further
    if (n!=4){
        cout<<"You did not enter four points\n";
        return 0;
    }

    //Crop the perspective image then display and save the obtained image 
    Mat cropedImage = img1_warp(Rect(472,52,329,779));
    namedWindow("Cropped Image",0);
	imshow("Cropped Image", cropedImage);
    imwrite(Image_name+"_Cropped Image.jpg",cropedImage);
    waitKey(0);

    cout << "End of Programme\n";
    
}
