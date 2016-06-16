#include<vector>
#include<iostream>
#include<cmath>

using namespace std;

vector<float> getFeatHistogram(vector<float>& magnitudes, vector<float>& angles, int numBins = 9){
/* GETHISTOGRAM Computes a histogram for the supplied gradient vectors.
   
   This function takes the supplied gradient vectors and places them into a
   histogram with 'numBins' based on their unsigned orientation. 

   "Unsigned" orientation means that, for example, a vector with angle 
   -3/4 * pi will be treated the same as a vector with angle 1/4 * pi. 

   Each gradient vector's contribution is split between the two nearest bins,
   in proportion to the distance between the two nearest bin centers.
   
   A gradient's contribution to the histogram is equal to its magnitude;
   the magnitude is divided between the two nearest bin centers.

   Parameters:
     magnitudes - A column vector storing the magnitudes of the gradient 
                  vectors.
     angles     - A column vector storing the angles in radians of the 
                  gradient vectors (ranging from -pi to pi)
    numBins    - The number of bins to place the gradients into.
   
   Returns:
     A row vector of length 'numBins' containing the histogram.

   $Author of MATLAB original code: ChrisMcCormick $
   $Author of C++ translation: Ewerton Lopes
	*/

	// Compute the bin size in radians. 180 degress = pi.
	float binSize = M_PI / numBins;

	// The angle values will range from 0 to pi.
	float minAngle = 0;

	vector<int> leftBinIndex(angles.size());
	vector<int> rightBinIndex(angles.size());
	vector<int> leftBinCenter(angles.size());
	vector<float> rightPortions(angles.size());
	vector<float> leftPortions(angles.size());

	// Make the angles unsigned by adding pi (180 degrees) to all negative angles.
	for (unsigned int i=0; i < angles.size(); i++){	
		if (angles[i] < 0) {
			angles[i] = angles[i] + M_PI;
			cout << "Change angle to positive: " << angles[i] << endl << flush;
		}
	
		// The gradient angle for each pixel will fall between two bin centers.
		// For each pixel, we split the bin contributions between the bin to the left
		// and the bin to the right based on how far the angle is from the bin centers.

		// For each pixel's gradient vector, determine the indeces of the bins to the
		// left and right of the vector's angle.
		//
		// The histogram needs to wrap around at the edges--vectors on the far edges of
		// the histogram (i.e., close to -pi or pi) will contribute partly to the bin
		// at that edge, and partly to the bin on the other end of the histogram.
		// For vectors with an orientation close to 0 radians, leftBinIndex will be 0. 
		// Likewise, for vectors with an orientation close to pi radians, rightBinIndex
		// will be numBins + 1. We will fix these indeces after we calculate the bin
		// contribution amounts.
	
		leftBinIndex[i] = round((angles[i] - minAngle) / binSize);
		rightBinIndex[i] = leftBinIndex[i] + 1;
	
		// For each pixel, compute the center of the bin to the left.
		leftBinCenter[i] = ((leftBinIndex[i] - 0.5) * binSize) - minAngle;

		// For each pixel, compute the fraction of the magnitude
		// to contribute to each bin.
		rightPortions[i] = angles[i] - leftBinCenter[i];
		leftPortions[i] = binSize - rightPortions[i];
		rightPortions[i] = rightPortions[i] / binSize;
		leftPortions[i] = leftPortions[i] / binSize;

		// Before using the bin indeces, we need to fix the '0' and '10' values.
		// Recall that the histogram needs to wrap around at the edges--bin "0" 
		// contributions, for example, really belong in bin 9.
		// Replace index 0 with 9 and index 10 with 1.
		if (leftBinIndex[i] == 0) {
			leftBinIndex[i] = numBins-1;
		}

		if (rightBinIndex[i] == numBins){
			rightBinIndex[i] = 0;
		}	
	}

	// Create an empty row vector for the histogram.
	vector<float> H(numBins,0);

	// For each bin index...
	for (int i=0; i < numBins; i++){
		// Find the pixels with left bin == i
		vector<int> lpixels;
		
		for(unsigned int j=0;j < leftBinIndex.size();j++){	
			if (leftBinIndex[j] == i) lpixels.push_back(j);
		}
		    
		// For each of the selected pixels, add the gradient magnitude to bin 'i',
		// weighted by the 'leftPortion' for that pixel.
		for(unsigned int k=0; k < lpixels.size(); k++){
			H[i] = H[i] + (leftPortions[lpixels[k]] * magnitudes[lpixels[k]]);
		}

		// Find the pixels with right bin == i
		vector<int> rpixels;
		for(unsigned int j=0;j < rightBinIndex.size();j++){	
			if (rightBinIndex[j] == i) lpixels.push_back(j);
		}
		    
		// For each of the selected pixels, add the gradient magnitude to bin 'i',
		// weighted by the 'rightPortion' for that pixel.
		for(unsigned int k=0; k < rpixels.size(); k++){
			H[i] = H[i] + (rightPortions[rpixels[k]] * magnitudes[rpixels[k]]);
		}
	}

	return H;
}
