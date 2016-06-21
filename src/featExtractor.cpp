#include<iostream>
#include<fstream>
#include "opencv2/opencv.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <glob.h>
#include <cmath>
#include "compute_histogram.cpp"

using namespace std;
using namespace cv;


float getMagnitude(int x, int y){
	return sqrt(pow(x,2) + pow(y,2));
}

float getAngle(int x, int y){
	return atan2(y,x);
}

/* To read current directory list files with pattern */
vector<string> globVector(const string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

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

/*Threshold is in mm. So, 30mm corresponds to 3cm*/
void sltp_descriptor(Mat& image, int threshold = 30){
    float* pPxl;
	float* cPxl;
	float* nPxl;
	float dx, dy;
	Mat result = image.clone();
	Mat featImage(Size(image.cols, image.rows), CV_8UC3);
	vector<float> magnitudes;
	vector<float> angles;
	for(int y=1;y<(result.size().height-1);y++){
		pPxl = result.ptr<float>(y-1);
		cPxl = result.ptr<float>(y);
		nPxl = result.ptr<float>(y+1);
    	for(int x=1;x<(result.size().width-1);x++){
			Vec3b & color = featImage.at<Vec3b>(Point(x,y));

			dx = cPxl[x+1]-cPxl[x-1];
			dy = nPxl[x] - pPxl[x];

			if (dx >= threshold) dx = 1;
			else if (abs(dx) < threshold) dx=0;
			else if (dx <= -threshold) dx = -1;

			if (dy >= threshold) dy = 1;
			else if (abs(dy) < threshold) dy=0;
			else if (dy <= -threshold) dy = -1;

			magnitudes.push_back(getMagnitude(dx,dy));
			angles.push_back(getAngle(dy,dx));

			/*Getting visible mat file
			-- Note there's a change in the pixel value
				from {-1,0,1} to {0,122,155}*/
			if (dy==1) color[1] = 255;
			else if (dy == 0) color[1]= 122;
			else color [1] = 0;

			if (dx==1) color[0] = 255;
			else if (dx == 0) color[0]= 122;
			else color[0] = 0;

			color[2] = 0;
			/*--------------*/

			image = featImage;
			//cout << "(" << dx << "," << dy << ")"; // debug output
		}
	}
	vector<float> hist = getFeatHistogram(magnitudes, angles);
	unsigned int histsize = hist.size();
	cout << "\nHistogram size: " << histsize << endl << flush;
	for (unsigned int i=0; i < histsize; i++){
		cout << hist[i] << " " << flush;
	}
}

int main(int argc, char * argv[]){

	/*
	if (argc != 2){
		cout << "Wrong number of arguments!" << endl;
		cout << "USAGE: ./featExtractor <filename> " << endl;
		exit(1);
	}*/

	vector<string> files = globVector("*.txt");

	for (unsigned int i=0;i< files.size(); i++){
		Mat src = ReadMatFromTxt(files[i]);
		Mat img = src.clone();
		cout << endl;
		cout << "Image " << files[i] << " loaded!" << endl << flush;
		cout << "Width: " << img.size().width << " -- Height: " << img.size().height
			 << endl << flush;
		cout << "-- PRESS n TO CONTINUE -- " << endl << flush;
		sltp_descriptor(img);


		Mat resz(Size(65,129),CV_32FC1);
		resizeImage(src,resz);
		sltp_descriptor(resz);

		while(1){
			imshow("Features",img);
			imshow("Resized", resz);
			char c=waitKey();
			if (c == 27){
				cout << "**Exiting program upon request" << endl;
				exit(0);
			}
			if (c == 'n'){
				break;
			}

			if (c == 'p'){
				i = i -2;
				break;
			}
		}
	}
	return 0;
}
