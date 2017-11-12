#include "Header.h"


/// VARIABLES
VideoCapture cap(0); // open the video default video camera

Mat input, src, redImg, dst;

//Shape Scan
int squareScanCount = 0;
int circleScanCount = 0;
int triangleScanCount = 0;

//For MAIN
bool OpenCamera;
bool runScan;
bool runSquareScan=true;
bool runCircleScan=true;
char choice;
char choice01;
char colorchoice;

/// FUNCTIONS

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int circleScan() {
	
	vector<vector<Point> > contours;
	findContours(redImg.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	dst = src.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		// Detect and label circles
		double area = cv::contourArea(contours[i]);
		Rect r = cv::boundingRect(contours[i]);
		int radius = r.width / 2;

		if (abs(1 - ((double)r.width / r.height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2) {
			circleScanCount++;
			cout << "Circle";
		}
	}
	return 0;
}

void squareScan() {
	// Find contours
	vector<vector<Point> > contours;
	findContours(redImg.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	dst = src.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

			// Sort ascending the cosine values
			sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
				squareScanCount++;
				cout << "Square";
			}
		}
	}
}

void triangleScan() {
	// Find contours
	vector<vector<Point> > contours;
	findContours(redImg.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	dst = src.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		if (approx.size() == 3)
		{
			triangleScanCount++;
			cout << "Triangle";
		}
	}
}





/// MAIN

int main(int argc, char* argv[])
{


	if (!cap.isOpened()) {  // if not success, exit program
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Turn on camera? y/n" << endl;
	cin >> choice;


	if (choice == 'y')
	{
		cout << "To turn off camera and quit press 'Esc'" << endl;
		OpenCamera = true;

	}


	else if (choice == 'n') {
		return 0;
	}




	while (OpenCamera == true) {

	
		bool bSuccess = cap.read(input); // read a new frame from video
		imshow("Camera", input);
		
		if (!bSuccess) { //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		
		/*
		cout << "Start Scanning? y/n" << endl;
		cin >> choice01;

		if (choice01 == 'y') {
			runScan = true;
		}
		if (choice == 'n') {
			return 0;
		} 
		 
		*/
		if (runScan == true) {




			cout << "What color have you been assigned? (r/b/g)" << endl;
			cin >> colorchoice;

			if (colorchoice == 'r') {
				cout << "Red is your color!" << endl;
			}
			if (colorchoice == 'b') {
				cout << "Blue is your color!" << endl;
			}

			if (colorchoice == 'g') {
				cout << "Green is your color!" << endl;
		}




			//SCANNING FOR COLOR

			//IF correct Color 

			cout << "Are you ready to scan the first marker?" << endl;
			cin >> choice;
			while (choice == 'y') {
				bool bSuccess = cap.read(input); // read a new frame from video
				imshow("Camera Scanning for Marker 1", input);
				cout << "SCANNING FOR MARKER 01!" << endl;  //FIRST MARKER SCAN
					cout << "SCANNING FOR CIRCLE!" << endl; //TEST
				circleScan();
				if (circleScanCount > 50) {
					break;
				}
				



				cout << "Are you ready to scan the second marker?" << endl;
				cin >> choice;
				while (choice == 'y') {
					circleScanCount = 0;
					cout << "SCANNING FOR MARKER 02!" << endl;  //SECOND MARKER SCAN
					cout << "SCANNING FOR Triangle!" << endl; //TEST
					triangleScan();
					if (triangleScanCount > 25) {
						break;
					}
				}





				cout << "Are you ready to scan the third marker?" << endl;
				cin >> choice;

				while (choice == 'y') {
					triangleScanCount = 0;
					cout << "SCANNING FOR MARKER 03!" << endl;  //THIRD MARKER SCAN
					cout << "SCANNING FOR Square!" << endl; //TEST
					squareScan();
					if (squareScanCount > 25) {
						break;
					}

				}



				cout << "Are you ready to scan the fourth marker?" << endl;
				cin >> choice;
				while (choice == 'y') {
					squareScanCount = 0;
					cout << "SCANNING FOR MARKER 04!" << endl;  //FOURTH MARKER SCAN
					cout << "SCANNING FOR Circle and Triangle!" << endl; //TEST
					circleScan();
					triangleScan();
					if (circleScanCount < 10 && triangleScanCount < 25) {
						break;
					}
				}



				cout << "Are you ready to scan the fifth marker?" << endl;
				cin >> choice;
				while (choice == 'y') {
					circleScanCount = 0;
					triangleScanCount = 0;
					cout << "SCANNING FOR MARKER 05!" << endl;  //FIFTH MARKER SCAN
					cout << "SCANNING FOR Circle and Square!" << endl; //TEST
					circleScan();
					squareScan();
					if (circleScanCount < 10 && squareScanCount < 25) {
						break;
					}

				}


				cout << "Are you ready to scan the sixth marker?" << endl;
				cin >> choice;
				while (choice == 'y') {
					circleScanCount = 0;
					squareScanCount = 0;
					cout << "SCANNING FOR MARKER 06!" << endl;  //SIXTH MARKER SCAN
					cout << "SCANNING FOR Square and Triangle!" << endl; //TEST
					circleScan();
					triangleScan();
					if (circleScanCount < 10 && triangleScanCount < 25) {
						break;
					}
				}



				cout << "Are you ready to scan the seventh marker?" << endl;
				cin >> choice;
				while (choice == 'y') {
					circleScanCount = 0;
					triangleScanCount = 0;
					cout << "SCANNING FOR MARKER 07!" << endl;  //SEVENTH MARKER SCAN
					cout << "SCANNING FOR Square and Triangle!" << endl; //TEST
					circleScan();
					triangleScan();
					squareScan();
					cout << "You have completed the RUN!" << endl;

				}
				if (choice == 'n') {
					cout << "GET READY FOR SCANNING" << endl;
				}

			} 






			cout << "CircleAmount:" << circleScanCount << endl;
			cout << "SquareAmount:" << squareScanCount << endl;
			cout << "TriangleAmount:" << triangleScanCount << endl;

		




		}





		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}
	

