#include <vector>
#include<fstream>
#include "opencv2/opencv.hpp"
#include <tuple>
#include <string>

using namespace std;
using namespace cv;


Mat ReadMatFromTxt(string filename, int rows,int cols)
{
    float m;
    Mat out = Mat::zeros(rows, cols, CV_32FC1);//Matrix to store values

    ifstream fileStream(filename);
    int cnt = 0;//index starts from 0
    while (fileStream >> m)
    {
        int temprow = cnt / cols;
        int tempcol = cnt % cols;
        out.at<float>(temprow, tempcol) = m;
        cnt++;
    }
    return out;
}


vector<Rect> get_sliding_windows(Mat& image,int winWidth,int winHeight)
{
  vector<Rect> rects;
  int step = 16;
  for(int i=0;i<image.rows;i+=step)
  {
      if((i+winHeight)>image.rows){break;}
      for(int j=0;j< image.cols;j+=step)    
      {
          if((j+winWidth)>image.cols){break;}
          Rect rect(j,i,winWidth,winHeight);
          rects.push_back(rect);
      }
  } 
  return rects;
}

vector<Mat> pyramid(Mat image, float scale=0.5, int minWidth = 30, int minHeight = 30){
	vector<Mat> images;
	images.push_back(image);	
	// keep looping over the pyramid
	Mat src = image.clone();
	Mat dst;
	while(1){
		// resize image
		cout << "Resizing..." << endl;
		resize(src, dst, dst.size(), scale, scale, CV_INTER_AREA);
 
		// if the resized image does not meet the supplied minimum
		// size, then stop constructing the pyramid
		if ((dst.size().height < minHeight) || (dst.size().width < minWidth))
			cout << "reached minimum" << endl;			
			break;
 		
		cout << "Heigt: " << dst.size().height << " Width: " << dst.size().width << endl;
		// stack the next image in the pyramid
		images.push_back(dst);
		src = dst.clone();
	}
}

int main(int argc, char * argv[]){
    int width = 512;
    int height = 424;
    Mat img = ReadMatFromTxt("./frame/string15.txt", height, width);
    unsigned short min = 500;
    unsigned short max = 4500;

    cv::Mat img0 = cv::Mat::zeros(img.size().height, img.size().width, CV_8UC1);
    cv::Mat img1;
    double scale = 255.0 / (max - min);
    img.convertTo(img0, CV_8UC1, scale);
    applyColorMap(img0, img1, cv::COLORMAP_JET);
    img = img1;
    // -------

	vector<Mat> slices = pyramid(img);
	for(int i=0; slices.size(); i++){
		imshow(to_string(i),slices[i]);	
	}

    while(1){
    imshow("Retrieved", img);
    char c=waitKey();
    if (c == 27)
        break;
 	}
    return 0;
}
