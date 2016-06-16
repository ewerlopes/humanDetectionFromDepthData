#include <vector>
#include<fstream>
#include "opencv2/opencv.hpp"
#include <tuple>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace cv;


void writeMatToFile(cv::Mat& m, const char* filename)
{
    ofstream fout(filename);
    if(!fout)
    {
        cout<<"File Not Opened"<<endl;  return;
    }

	fout << m.size().width << " " << m.size().height << endl;
    for(int i=0; i<m.rows; i++)
    {
        for(int j=0; j<m.cols; j++)
        {
            fout<<m.at<float>(i,j)<<"\t";
        }
        fout<<endl;
    }
    fout.close();
}

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


vector<Rect> sliding_window(Mat& image,int winWidth,int winHeight){
	vector<Rect> rects;
	int step = 16;
	for(int i=0;i<image.size().height;i+=step){
		if((i+winHeight)>image.size().height){
			break;
		}
    	for(int j=0;j< image.size().width;j+=step){
        	if((j+winWidth)>image.size().width){
				break;
			}
		 	Rect rect(j,i,winWidth,winHeight);
         	rects.push_back(rect);
		}
	} 
	return rects;
}

void pyramid(Mat image, vector<Mat>& results, float scale=0.75, int minWidth = 150, int minHeight = 150){
	
	results.push_back(image.clone());
	// keep looping over the pyramid
	Mat src = image.clone();
	Mat dst;
	while(1){
		// resize image
		cout << "Resizing..." << endl;
		resize(src, dst, Size(), scale, scale, CV_INTER_AREA);
		
		// if the resized image does not meet the supplied minimum
		// size, then stop constructing the pyramid
		if ((dst.size().height < minHeight) || (dst.size().width < minWidth)){
			cout << "FAILED TO RESIZE! \t Reached minimum size for images!" << endl;
			cout << "Pyramid created with success!" << endl;			
			break;
		}
 
		cout << "DONE!\tNew img. size --\tHeight: " << dst.size().height << " Width: " << dst.size().width << endl;
		// stack the next image in the pyramid
		results.push_back(dst.clone());
		src = dst;
	}
}

int main(int argc, char * argv[]){
    int width = 512;
    int height = 424;
    Mat img = ReadMatFromTxt("./frame/string228.txt", height, width);
    unsigned short min = 500;
    unsigned short max = 4500;

	// Applying color map
    cv::Mat img0 = cv::Mat::zeros(img.size().height, img.size().width, CV_8UC1);
    cv::Mat img1;
    double scale = 255.0 / (max - min);
    img.convertTo(img0, CV_8UC1, scale);
    applyColorMap(img0, img1, cv::COLORMAP_JET);
    img = img1;
    // -------

	vector<Mat> slices;
	pyramid(img, slices);
	for(unsigned int i=0; i < slices.size(); i++){
		cout << "\n\nScanning image..." << endl << flush;
		cout << "Img. Width: " << slices[i].size().width << endl << flush;
		cout << "Img. Height: " << slices[i].size().height << endl << flush;
		
		int winWidth = 128;
		int winHeight = 256;
		
		cout << "Sliding Window -> \tHeight: " << winHeight << " Width: " << winWidth << endl << flush;
		vector<Rect> swindows = sliding_window(slices[i], winWidth, winHeight);
		
		for(unsigned int j=0; j < swindows.size(); j++){
		
			/* THIS IS WHERE YOU WOULD PROCESS YOUR WINDOW, SUCH AS APPLYING A
			   MACHINE LEARNING CLASSIFIER TO CLASSIFY THE CONTENTS OF THE
			   WINDOW*/
		
			// since we do not have a classifier, we'll just draw the window
			Mat copySlices = slices[i].clone();
			rectangle(copySlices, swindows[j], Scalar(0,255,0),1,8,0); // the selection green rectangle 
			imshow("Window",copySlices);
			char c = waitKey(1);
			if(c==27){
				cout << "Skipping..." << endl << flush;				
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
    return 0;
}
