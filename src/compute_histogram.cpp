#include<vector>
#include<iostream>
#include<cmath>

using namespace std;

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
