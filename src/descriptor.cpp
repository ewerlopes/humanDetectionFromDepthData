#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include<fstream>

using namespace std;
using namespace cv;

/*Threshold is in mm. So, 30mm corresponds to 3cm*/
vector<float> getFeatHistogram(vector<float>& magnitudes, vector<float>& angles, int numBins = 9){

	// Compute the bin size in radians. 180 degress = pi.
	float binSize = M_PI / numBins;

	// The angle values will range from 0 to pi.
	float minAngle = 0;

	vector<int> leftBinIndex;
	vector<int> rightBinIndex;
	vector<int> leftBinCenter;
	vector<float> rightPortions;
	vector<float> leftPortions;

	for (unsigned int i=0; i < angles.size(); i++){
		if (angles[i] < 0) {
			// Make the angles unsigned by adding pi (180 degrees) to all negative angles.
			angles[i] += M_PI;
		}

		// Essentially the bin to which the angle belongs to.
		leftBinIndex.push_back(round((angles[i] - minAngle) / binSize));

		// Before using the bin indeces, we need to fix the indeces values.
		// Recall that the histogram needs to wrap around at the edges -- bin "0"
		// contributions, for example, really belong in bin 9.
		// Replace index 0 with 9 and index 10 with 1.

		if (leftBinIndex[i] == 9){
			leftBinIndex[i] = 0;
		}

		// the bin next to the bin the angle belongs to.
		rightBinIndex.push_back(leftBinIndex[i] + 1);

		if (rightBinIndex[i] > numBins-1){
			rightBinIndex[i] = 0;
		}

		// For each pixel, compute the center of the bin to the left.
		leftBinCenter.push_back(abs(((leftBinIndex[i] - 0.5) * binSize) - minAngle));

		// For each pixel, compute the fraction of the magnitude
		// to contribute to each bin.
		float tempRightPortions = angles[i] - leftBinCenter[i];
		float tempLeftPortions = abs(binSize - tempRightPortions);
		rightPortions.push_back(tempRightPortions / binSize);
		leftPortions.push_back(tempLeftPortions / binSize);
	}

	// Create an empty row vector for the histogram.
	vector<float> H(numBins,0);
	// For each bin index...
	for (int i=0; i < numBins; i++){
		// Find the pixels with left bin == i

		// For each of the selected pixels, add the gradient magnitude to bin 'i',
		// weighted by the 'leftPortion' for that pixel.
		for(unsigned int j=0;j < leftBinIndex.size();j++){
			if (leftBinIndex[j] == i){
				H[i] = H[i] + (leftPortions[j] * magnitudes[j]);
			}
		}

		// Find the pixels with right bin == i
		for(unsigned int j=0;j < rightBinIndex.size();j++){
			if (rightBinIndex[j] == i)
				// For each of the selected pixels, add the gradient magnitude to bin 'i',
				// weighted by the 'rightPortion' for that pixel.
				H[i] = H[i] + (rightPortions[j] * magnitudes[j]);
		}
	}

	return H;
}


vector<float> getFeatVector(Mat& srcImage, bool showImageAsDebug=false){

	Mat img = srcImage.clone();

    vector<Rect> rects;

	// Cell size. Equivalent to the one in the HOG descriptor.
    int step = 8;

	vector<float> hists;
	Mat featImage(Size(img.cols, img.rows), CV_8UC3); // used to see the features as an image

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

				pPxl = img.ptr<float>(y-1);
				cPxl = img.ptr<float>(y);
				nPxl = img.ptr<float>(y+1);
		    	for(int x = j; x < j+step; x++){
					Vec3b & color = featImage.at<Vec3b>(Point(x,y));
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

					/*****************************************************
					/{    Getting visible mat file
					//    -- Note there's a change in the pixel value
					//	  from {-1,0,1} to {0,122,155}
					******************************************************/
					if(showImageAsDebug){					
						if (dy==1) color[1] = 255;
						else if (dy == 0) color[1]= 122;
						else color [1] = 0;

						if (dx==1) color[0] = 255;
						else if (dx == 0) color[0]= 122;
						else color[0] = 0;

						color[2] = 0;
					}
					/*---------------------------------------------------}*/
				}
			}
			// =====================================
			// 	Getting the histogram for the cell
			// =====================================
			//  cout << endl<<  i << "-" << j << endl;
			//  cout << "Magnitudes: " << magnitudes.size() << endl;
			//  cout << "Angles: " << angles.size() << endl;
			 vector<float> bins = getFeatHistogram(magnitudes,angles);
			//  std::cout << "Bins: ";
			//  for(int g=0; g < bins.size(); g++){
			//  	std::cout << bins[g] << " ";
			//  }
			//  cout << "\nCell histogram Size: " << bins.size() << endl;
			 hists.insert(std::end(hists), std::begin(bins), std::end(bins));
			//  cout << "All histograms - Size: " << hists.size() << endl << endl;

		}
	}

	/* NOTE: in order to actually see the image, uncomment the
	 * following block of code */
	if (showImageAsDebug){
		for (int r = 0; r < rects.size();r++) {
			// the selection green rectangle
			rectangle(featImage, rects[r], Scalar(0,255,0),1,8,0);
		}
		while(1){
			imshow("Feat",featImage);
			char c=waitKey();
			if (c == 27){
				break;
			}
		}
	}

    return hists;
}
