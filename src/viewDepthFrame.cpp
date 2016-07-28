//To read data from a text file. 
//filename is the name of the text file
//rows and cols show dimensions of the matrix written in the text file

#include<fstream>
#include "opencv2/opencv.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <string>     // std::string, std::to_string

using namespace std;
using namespace cv;


Mat ReadMatFromTxt(string filename)
{
    float m;
    ifstream fileStream(filename);

	int height = 424;
	int width  = 512;

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

void sliceImage(vector<Mat>& imageBuffer, Mat &image){
	
	// limit slice to 4.5 meters (4500)
	for (int distance = 0; distance < 4500; distance += 30){
		// used to see the features as an image		
		Mat featImage(Size(image.cols, image.rows), CV_8UC1);
		for (int y = 0; y < image.rows; y++){
			float* cPxl = image.ptr<float>(y);
			for(int x = 0; x < image.cols; x++){
				Vec3b & color = featImage.at<Vec3b>(Point(x,y));
				if (cPxl[x] == distance){
					color[0] = 255;
				}
			}
		}
		cout << "Slice distance of: " << distance << endl;
		imageBuffer.push_back(featImage);
	}
}

int main(int argc, char * argv[]){
    
	if (argc != 2){
		cout << "Wrong number of arguments!" << endl;
		cout << "USAGE: ./imageViewer <filename> " << endl;
		exit(1);
	}

    Mat src = ReadMatFromTxt(argv[1]);
	Mat img = src.clone();
    
    /* applying color map */
    unsigned short min = 500;
    unsigned short max = 4500;

    cv::Mat img0 = cv::Mat::zeros(img.size().height, img.size().width, CV_8UC1);
    cv::Mat img1;
    double scale = 255.0 / (max - min);
    img.convertTo(img0, CV_8UC1, scale);
    applyColorMap(img0, img1, cv::COLORMAP_JET);
    img = img1;
    // ------- 
	
	
	cout << "Image " << argv[1] << " loaded!" << endl << flush;
	cout << "Width: " << img.size().width << " -- Height: " << img.size().height << endl << flush;
    
	bool wasSliced = false;
	while(1){
   	
		imshow("Retrieved", img);

		char c=waitKey();
    	
		if (c == 27)
        	exit(0);
		
		if (c == 's' && !wasSliced){
			cout << "Slicing depth frame..." << endl;
			vector<Mat> slices;
			sliceImage(slices, src);
			for (int s=0; s < slices.size(); s++){
				imshow(to_string(s), slices[0]);
			}
			wasSliced = true;
		}
		
 	}
    return 0;
}
