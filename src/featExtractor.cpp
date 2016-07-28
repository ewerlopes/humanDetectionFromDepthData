#include <iostream>
#include <string>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <glob.h>
#include <cmath>
#include <sys/stat.h>
#include <unistd.h>
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

    int cflag = 0;
    int tflag = 0;
    int cvalue = 0;
    int index;
    int c;

    opterr = 0;

    if (argc != 2){
        cout << "Wrong number of parameters!" << endl;
        cout << "See help by running: ./featExtractor -h" << endl;
        exit(-1);
    }

    while ((c = getopt (argc, argv, "hc:t")) != -1){
        switch (c)
        {
            case 'c':
                if (tflag){
                    cerr << "Argument error!" << endl
                         << "-c and -t are mutually exclusive!";
                    exit(-1);
                }
                cflag = 1;
                cvalue = atoi(optarg);
                break;
            case 't':
                if (cflag){
                    cerr << "Argument error!" << endl
                         << "-c and -t are mutually exclusive!";
                    exit(-1);
                }
                tflag = 1;
                break;
            case 'h':
                cout << "*** FEATURE EXTRACTOR ***" << endl;
                cout << "\n-c\t: <class> A number to describe the class of the feature.";
                cout << "\n-t\t: extract features from a unknown class intance.";
                cout << "\n-h\t: help info." << endl;
                return 0;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    }

	vector<string> files = globVector("*.txt");
    ofstream featFile;

    struct stat sb;
	if (stat("./result", &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		cout << "./result directory already exists!" << endl;
		cout << "Skipping directory creation..." << endl;
	}else{
		cout << "./result directory does not exists!" << endl;
		cout << "creating ./result directory... ";
		const int dir_err = mkdir("result", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
			cout << "Error creating directory!" << endl;
			exit(1);
		}
	}

    featFile.open("./result/features.txt");

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
        //std::stringstream output;

        // putting the class
        if (!tflag){
            featFile << argv[1] << "  ";
        }
        else{
            featFile << "1000   ";
        }

		std::cout << "\nSaving features for " << files[i] << endl
                  << "Histograms values (size: " << result.size() <<")..." << std::endl;

        for(int g=0; g < result.size(); g++){
			featFile << (g+1) << ":" << result[g] << " ";
		}

        featFile << endl;
        cout << "DONE!" << endl;
	}

    featFile.close();
	return 0;
}
