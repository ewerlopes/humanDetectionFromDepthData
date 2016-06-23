#include<iostream>
#include<fstream>
#include "opencv2/opencv.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <glob.h>
#include <cmath>
#include "descriptor.cpp"

using namespace std;
using namespace cv;

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

int main(int argc, char * argv[]){

	vector<string> files = globVector("*.txt");

	for (unsigned int i=0;i< files.size(); i++){
		Mat src = ReadMatFromTxt(files[i]);
		Mat img = src.clone();
		cout << endl;
		cout << "Image " << files[i] << " loaded!" << endl << flush;
		cout << "Width: " << img.size().width << " -- Height: " << img.size().height
			 << endl << flush;
		cout << "-- PRESS n TO CONTINUE -- " << endl << flush;

		Mat resz(Size(65,129),CV_32FC1);
		resizeImage(src,resz);

		vector<float> result = getFeatVector(resz);
		std::cout << "Histograms values (size: " << result.size() <<")..." << std::endl;
		for(int g=0; g < result.size(); g++){
			std::cout << result[g] << " " << flush;
		}

		// while(1){
		// 	imshow("Features",resz);
		// 	char c=waitKey();
		// 	if (c == 27){
		// 		cout << "**Exiting program upon request" << endl;
		// 		exit(0);
		// 	}
		// 	if (c == 'n'){
		// 		break;
		// 	}
		//
		// 	if (c == 'p'){
		// 		i = i -2;
		// 		break;
		// 	}
		// }
	}
	return 0;
}
