//To read data from a text file. 
//filename is the name of the text file
//rows and cols show dimensions of the matrix written in the text file

#include<fstream>
#include "opencv2/opencv.hpp"
#include <boost/algorithm/string.hpp>
#include <string>

using namespace std;
using namespace cv;


Mat ReadMatFromTxt(string filename)
{
    float m;
    ifstream fileStream(filename);

	// Set up a place to store our data read from the file
    string line;

    // Read and throw away the first line simply by doing
    // nothing with it and reading again
    getline(fileStream, line);
	
	vector<string> results;
	boost::split(results, line, boost::is_any_of(" "));
	int height = stoi(results[1]);		// height is the second number in the first line
	int width = stoi(results[0]);			// width is the first number in the first line
    int cnt = 0;//index starts from 0

	Mat out = Mat::zeros(height, width, CV_32FC1);//Matrix to store values
    
	while (fileStream >> m)
    {
        int temprow = cnt / width;
        int tempcol = cnt % width;
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

void resizeImage(Mat& img, Mat & dst){
	Mat src = img.clone();
	cout << "Resizing..." << endl;
	resize(src, dst, dst.size(), 0, 0, CV_INTER_AREA);
}

int main(int argc, char * argv[]){
    
	if (argc != 2){
		cout << "Wrong number of arguments!" << endl;
		cout << "USAGE: ./imageViewer <filename> " << endl;
		exit(1);
	}

    Mat img = ReadMatFromTxt(argv[1]);
    
	/* applying color map
    unsigned short min = 500;
    unsigned short max = 4500;

    cv::Mat img0 = cv::Mat::zeros(img.size().height, img.size().width, CV_8UC1);
    cv::Mat img1;
    double scale = 255.0 / (max - min);
    img.convertTo(img0, CV_8UC1, scale);
    applyColorMap(img0, img1, cv::COLORMAP_JET);
    img = img1;
    // ------- */
	
	bool showResized = false;
	Mat resz(Size(64,128),CV_32FC1);
	
	cout << "Image " << argv[1] << " loaded!" << endl << flush;
	cout << "Width: " << img.size().width << " -- Height: " << img.size().height << endl << flush;
    cout << "Press r to resize the image!" << endl << endl;
	while(1){
   	
		imshow("Retrieved", img);

		if (showResized){
			imshow("Resized", resz);
		}    	
		char c=waitKey();
    	if (c == 27)
        	break;
		if (c == 'r' && showResized==false){
			showResized = !showResized;
			resizeImage(img,resz);
			cout << "DONE! \t New Width: " << resz.size().width << " -- New Height: "
				 << resz.size().height
				 << endl << flush;
		}
 	}
    return 0;
}
