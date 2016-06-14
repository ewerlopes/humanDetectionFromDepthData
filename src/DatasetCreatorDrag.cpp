#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "opencv2/opencv.hpp"
#include <string>
#include <ctime>
#include <vector>
#include <glob.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace cv;


Mat src,img,ROI;
Rect cropRect(0,0,0,0);
Point P1(0,0);
Point P2(0,0);

vector<string> files;
const char* winName="Original Image";
bool clicked=false;
int i=0;// for counter for files.

void saveLog(){
	cout << "Exiting and saving log..." << endl;
	cout << "Verifying if ./log directory exists..." << endl;

	struct stat sb;
	if (stat("./log", &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		cout << "./log directory exists!" << endl;
		cout << "Skipping directory creation..." << endl;
	}else{
		cout << "./log directory does not exists!" << endl;
		cout << "creating ./log directory... ";
		const int dir_err = mkdir("log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
			cout << "Error creating directory!" << endl;
			cout << "LOG FILE COULD NOT BE CREATED!!!";
			exit(1);
		}
		cout << "DONE!" << endl;
	}
	
	cout << "Saving log... " << endl;
	time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	ofstream log;
	log.open ("./log/log.txt");
	log << (now->tm_year + 1900) << '-' 
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday << " -- "
		<< now->tm_hour << ':'
		<< now->tm_min << ':'
		<< now->tm_sec
		<< "$:\t current image: "
		<< files[i] << "\n";
	cout << "DONE" << endl;
	log.close();
	cout << "Program exit with success!" << endl;
	exit(0);
}

void onInterrupt(int s){
	cout << "User interruption detected!" << endl;
	saveLog();
	exit(1); 
}

Mat createColorMap(Mat img){
    unsigned short min = 500;
    unsigned short max = 4500;

    cv::Mat img0 = cv::Mat::zeros(img.size().height, img.size().width, CV_8UC1);
    cv::Mat img1;
    double scale = 255.0 / (max - min);
    img.convertTo(img0, CV_8UC1, scale);
    applyColorMap(img0, img1, cv::COLORMAP_JET);
    return img1;
}

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

void checkBoundary(){
       //check croping rectangle exceed image boundary
       if(cropRect.width>img.cols-cropRect.x)
         cropRect.width=img.cols-cropRect.x;

       if(cropRect.height>img.rows-cropRect.y)
         cropRect.height=img.rows-cropRect.y;

        if(cropRect.x<0)
         cropRect.x=0;

       if(cropRect.y<0)
         cropRect.height=0;
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

void showHelp(){
	system("clear");
    cout<<"**** CLICK AND DRAG FOR SELECTION ****"<<endl<<endl;
    cout<<"* Press 's' to save"<<endl;
	cout<<"* Press 'n' to move to next picture"<<endl;
	cout<<"* Press 'p' to move to previous picture"<<endl<<endl;

	cout<<"* Press 'h' to move to previous picture"<<endl<<endl;

    cout<<"-> Press '8' to move up"<<endl;
    cout<<"-> Press '2' to move down"<<endl;
    cout<<"-> Press '6' to move right"<<endl;
    cout<<"-> Press '4' to move left"<<endl<<endl;

    cout<<"-> Press 'w' increase top"<<endl;
    cout<<"-> Press 'x' increase bottom"<<endl;
    cout<<"-> Press 'd' increase right"<<endl;
    cout<<"-> Press 'a' increase left"<<endl<<endl;

    cout<<"-> Press 't' decrease top"<<endl;
    cout<<"-> Press 'b' decrease bottom"<<endl;
    cout<<"-> Press 'h' decrease right"<<endl;
    cout<<"-> Press 'f' decrease left"<<endl<<endl;

    cout<<"-> Press 'r' to reset"<<endl;
    cout<<"-> Press 'Esc' to quit"<<endl<<endl;
}

void showImage(){
    img=createColorMap(src.clone());
    checkBoundary();
    if(cropRect.width>0&&cropRect.height>0){
        ROI=src(cropRect);
 	system("clear");	// clear the console
	showHelp();
	std::ostringstream str;
        str << "Current cropped image size...\n";
    	str << "Width: " << cropRect.width << "\tHeight: " << cropRect.height << endl;
	str << endl;
	cout << str.str();
    imshow("Selected image",createColorMap(ROI));
    }

    rectangle(img, cropRect, Scalar(0,255,0), 1, 8, 0 ); // the selection green rectangle 
    imshow(winName,img);
}


void onMouse( int event, int x, int y, int f, void* ){
    switch(event){

        case  CV_EVENT_LBUTTONDOWN  :
                                        clicked=true;

                                        P1.x=x;
                                        P1.y=y;
                                        P2.x=x;
                                        P2.y=y;
                                        break;

        case  CV_EVENT_LBUTTONUP    :
                                        P2.x=x;
                                        P2.y=y;
                                        clicked=false;
                                        break;

        case  CV_EVENT_MOUSEMOVE    :
                                        if(clicked){
                                        P2.x=x;
                                        P2.y=y;
                                        }
                                        break;

        default                     :   break;


    }


    if(clicked){
     if(P1.x>P2.x){ cropRect.x=P2.x;
                       cropRect.width=P1.x-P2.x; }
        else {         cropRect.x=P1.x;
                       cropRect.width=P2.x-P1.x; }

        if(P1.y>P2.y){ cropRect.y=P2.y;
                       cropRect.height=P1.y-P2.y; }
        else {         cropRect.y=P1.y;
                       cropRect.height=P2.y-P1.y; }
    }

    showImage();
}

/* To read data from a text file. 
	filename is the name of the text file
	rows and cols show dimensions of the matrix
	written in the text file */
Mat ReadDepthImageFromTxt(string filename, int height,int width)
{
    float m;
    Mat out = Mat::zeros(height, width, CV_32FC1);//Matrix to store values

    ifstream fileStream(filename);
    int cnt = 0;//index starts from 0
    while (fileStream >> m)
    {
        int tempheight = cnt / width;
        int tempwidth = cnt % width;
        out.at<float>(tempheight, tempwidth) = m;
        cnt++;
    }
    return out;
}

int main()
{

	struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = onInterrupt;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

	files = globVector("*.txt");
    showHelp();
	int width = 512;
    int height = 424;

	for (i=0;i< files.size(); i++){
		src = ReadDepthImageFromTxt(files[i], height, width);
		Mat depthImage = createColorMap(src);
		
		setMouseCallback(winName,onMouse,NULL );
		imshow(winName,depthImage);

		while(1){
			char c=waitKey();
			if(c=='s'&&ROI.data){
		 		
				std::stringstream output;
	 			output << "./dataset/" << i << ".txt";
				struct stat sb;
				if (stat("./dataset", &sb) == 0 && S_ISDIR(sb.st_mode))
				{
					cout << "./dataset directory exists!" << endl;
					cout << "Skipping directory creation..." << endl;
				}else{
					cout << "./dataset directory does not exists!" << endl;
					cout << "creating ./dataset directory... ";
					const int dir_err = mkdir("dataset", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					if (-1 == dir_err)
					{
						cout << "Error creating directory!" << endl;
						exit(1);
					}
				}
				cout << "DONE!" << endl;
				writeMatToFile(ROI,output.str().c_str());
		 		cout <<"/t"<< output << "was saved in ./dataset!" << endl;
			}

			if(c=='n'){
				// break for loop and go to next image on the 'files' list]
				cout << "Advancing to next image.. " << endl << flush;
				break;
			}
			
			if(c=='p'){
				// decrease for loop counter
				cout << "Returning to previous image.. " << endl << flush;
				i = i - 2;
				break;
			}	
			if(c=='6') cropRect.x++;
			if(c=='4') cropRect.x--;
			if(c=='8') cropRect.y--;
			if(c=='2') cropRect.y++;
		
				
			if(c=='w') { cropRect.y--; cropRect.height++;}
			if(c=='d') cropRect.width++;
			if(c=='x') cropRect.height++;
			if(c=='a') { cropRect.x--; cropRect.width++;}

			if(c=='t') { cropRect.y++; cropRect.height--;}
			if(c=='h') cropRect.width--;
			if(c=='b') cropRect.height--;
			if(c=='f') { cropRect.x++; cropRect.width--;}

			if(c==27){
				saveLog();
			}

			if(c=='r') {cropRect.x=0;cropRect.y=0;cropRect.width=0;cropRect.height=0;}
	 	    showImage();
		}
	}
    return 0;
}
