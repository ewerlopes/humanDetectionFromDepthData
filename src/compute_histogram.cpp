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

	// Make the angles unsigned by adding pi (180 degrees) to all negative angles.
	for (unsigned int i=0; i < angles.size(); i++){
		if (angles[i] < 0) {
			angles[i] += M_PI;
		}

        leftBinIndex.push_back(round((angles[i] - minAngle) / binSize));
		rightBinIndex.push_back(leftBinIndex[i] + 1);

		// For each pixel, compute the center of the bin to the left.
		leftBinCenter.push_back(((leftBinIndex[i] - 0.5) * binSize) - minAngle);


		// For each pixel, compute the fraction of the magnitude
		// to contribute to each bin.
		rightPortions.push_back(angles[i] - leftBinCenter[i]);
		leftPortions.push_back(binSize - rightPortions[i]);
		rightPortions.push_back(rightPortions[i] / binSize);
		leftPortions.push_back(leftPortions[i] / binSize);

		// Before using the bin indeces, we need to fix the '0' and '10' values.
		// Recall that the histogram needs to wrap around at the edges--bin "0"
		// contributions, for example, really belong in bin 9.
		// Replace index 0 with 9 and index 10 with 1.

		if (leftBinIndex[i] < 0) {
			leftBinIndex[i] = numBins-1;
		}

		if (rightBinIndex[i] == numBins-1){
			rightBinIndex[i] = 0;
		}
	}
	// Create an empty row vector for the histogram.
	vector<float> H(numBins,0);
	// For each bin index...
	for (int i=0; i < numBins; i++){
		// Find the pixels with left bin == i

		// For each of the selected pixels, add the gradient magnitude to bin 'i',
		// weighted by the 'leftPortion' for that pixel.

		for(unsigned int j=0;j < leftBinIndex.size();j++){
			if (leftBinIndex[j] == i)
				H[i] = H[i] + (leftPortions[j] * magnitudes[j]);

			// Find the pixels with right bin == i
			if (rightBinIndex[j] == i)
				// For each of the selected pixels, add the gradient magnitude to bin 'i',
				// weighted by the 'rightPortion' for that pixel.
				H[i] = H[i] + (rightPortions[j] * magnitudes[j]);
		}
	}
	/*NOTE: leftBinIndex and rightBinIndex BOTH HAVE THE SAME SIZE, THEREFORE THERE IS NO
	PROBLEM IN MAKING THE ABOVE FOR LOOP ITERATE OVER JUST THE SIZE OF ONE OF THE
	VARIABLES.*/
	return H;
}
