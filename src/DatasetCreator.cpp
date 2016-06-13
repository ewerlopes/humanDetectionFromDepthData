#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void onMouse( int event, int x, int y, int, void* );
using namespace cv;

Mat img = cv::imread("b.jpg"); // force grayscale
Mat thresh=Mat::zeros(img.size(),CV_8UC1);

int main(int argc, char **argv)
{

    if(!img.data) {
        std::cout << "File not found" << std::endl;
        return -1;
    }

   threshold(img,thresh,50,255,THRESH_TOZERO);  

   namedWindow("thresh");
   setMouseCallback( "thresh", onMouse, 0 );

   while(1){
      imshow("thresh",thresh);
      int key = cv::waitKey(1);
      if (key > 0 && ((key & 0xFF) == 27)){
          break;
      }
   }
}

void onMouse( int event, int x, int y, int, void* )
{
    if( event != CV_EVENT_LBUTTONDOWN )
            return;

    Point pt = Point(x,y);
    std::cout<<"x="<<pt.x<<"\t y="<<pt.y<<"\t PixelValue="<< int(thresh.at<uchar>(y,x))<<"\n";

}
