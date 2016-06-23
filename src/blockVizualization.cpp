#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include "compute_histogram.cpp"
#include<fstream>

using namespace std;
using namespace cv;

void resizeImage(Mat& img, Mat & dst){
	Mat src = img.clone();
	resize(src, dst, dst.size(), 0, 0, CV_INTER_AREA);
}

Mat ReadMatFromTxt(string filename){
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

	Mat resz(Size(65,129),CV_32FC1);
	resize(src, resz, resz.size(), 0, 0, CV_INTER_AREA);

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

	vector<float> hists;

	for(int i=1;i<129;i+=step){
		if((i+step) > 129){
			break;
		}

        for(int j=1;j<65;j+=step){
        	if((j+step) > 65){
				break;
			}

		 	Rect rect(j,i,step,step);
         	rects.push_back(rect);
			// ===============================
			// 			STLP descriptor
			// ===============================
			float* pPxl;
			float* cPxl;
			float* nPxl;
			float dx, dy;
			vector<float> magnitudes;
			vector<float> angles;
			int threshold = 30;
			for (int y = i; y < i+step; y++){

				pPxl = resz.ptr<float>(y-1);
				cPxl = resz.ptr<float>(y);
				nPxl = resz.ptr<float>(y+1);
		    	for(int x = j; x < j+step; x++){

					dx = cPxl[x+1]-cPxl[x-1];
					dy = nPxl[x] - pPxl[x];

					if (dx >= threshold) dx = 1;
					else if (abs(dx) < threshold) dx=0;
					else if (dx <= -threshold) dx = -1;

					if (dy >= threshold) dy = 1;
					else if (abs(dy) < threshold) dy=0;
					else if (dy <= -threshold) dy = -1;

					//cout << "(" << dx << "," << dy << ")"; // debug output
					magnitudes.push_back(sqrt(pow(dx,2) + pow(dy,2)));
					angles.push_back(atan2(dy,dx));
				}
			}
			// =====================================
			// 	Getting the histogram for the cell
			// =====================================
			 cout << endl<<  i << "-" << j << endl;
			 cout << "Magnitudes: " << magnitudes.size() << endl;
			 cout << "Angles: " << angles.size() << endl;
			 vector<float> bins = getFeatHistogram(magnitudes,angles);
			 std::cout << "Bins: ";
			 for(int g=0; g < bins.size(); g++){
			 	std::cout << bins[g] << " ";
			 }
			 cout << "\nCell histogram Size: " << bins.size() << endl;
			 hists.insert(std::end(hists), std::begin(bins), std::end(bins));
			 cout << "All histograms - Size: " << hists.size() << endl << endl;
		}
	}

    for (int r = 0; r < rects.size();r++) {
		// the selection green rectangle
        rectangle(resz, rects[r], Scalar(0,255,0),1,8,0);
    }

	std::cout << "Histograms values" << std::endl;
	for(int g=0; g < hists.size(); g++){
		std::cout << hists[g] << " " << flush;
		if ((g%9 == 0) && (g!=0)){
			cout << endl;
		}
	}

    while(1){
        imshow("Features",resz);
        char c=waitKey();
        if (c == 27){
            cout << "**Exiting program upon request" << endl;
            exit(0);
        }
    }

}
