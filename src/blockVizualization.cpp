#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <boost/algorithm/string.hpp>
#include<fstream>

using namespace std;
using namespace cv;

void resizeImage(Mat& img, Mat & dst){
	Mat src = img.clone();
	resize(src, dst, dst.size(), 0, 0, CV_INTER_AREA);
}

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

int main(int argc, char * argv[]){

	if (argc != 2){
		cout << "Wrong number of arguments!" << endl;
		cout << "USAGE: ./featExtractor <filename> " << endl;
		exit(1);
	}

    Mat src = ReadMatFromTxt(argv[1]);
    Mat img = src.clone();

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

    vector<Rect> rects;

    cout << endl;
    cout << "Image " << argv[1] << " loaded!" << endl << flush;
    cout << "Width: " << img.size().width << " -- Height: " << img.size().height
         << endl << flush;

    int step = 8;

    for(int i=0;i<img.size().height;i+=step){
		if((i+step)>img.size().height){
			break;
		}

        for(int j=0;j< img.size().width;j+=step){
        	if((j+step)>img.size().width){
				break;
			}
		 	Rect rect(j,i,step,step);
         	rects.push_back(rect);
		}
	}

    for (int r = 0; r < rects.size();r++) {
        rectangle(img, rects[r], Scalar(0,255,0),1,8,0); // the selection green rectangle
    }

    while(1){
        imshow("Features",img);
        char c=waitKey();
        if (c == 27){
            cout << "**Exiting program upon request" << endl;
            exit(0);
        }
    }

}
