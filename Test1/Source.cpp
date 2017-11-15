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
bool runSquareScan = true;
bool runCircleScan = true;
char choice;
char choice01;
char colorchoice;
bool m1, m2, m3, m4, m5, m6, m7, redRoute, blueRoute, greenRoute;




/// FUNCTIONS

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void circleScan() {
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
			while (circleScanCount < 20) {
				circleScanCount++;
				//cout << "Circle"<<circleScanCount<<endl;
				break;
			}
		}
	}
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
				while (squareScanCount < 20) {
					squareScanCount++;
					//cout << "Square"<<squareScanCount<<endl;
					break;
				}
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
			while (triangleScanCount < 20) {
				triangleScanCount++;
				//cout << "Triangle"<<triangleScan<<endl;
				break;
			}
		}
	}
}

void marker01() {
	circleScan();
}					
void marker02() {
	triangleScan();
}
void marker03() {
	squareScan();
}
void marker04() {
	circleScan();
	triangleScan();
}
void marker05() {
	circleScan();
	squareScan();
}
void marker06() {
	squareScan();
	triangleScan();
}
void marker07() {
	circleScan();
	triangleScan();
	squareScan();
}
					
					

void markerCheck() {

	if (m1 == true && circleScanCount == 20) {
		cout << "Marker 1" << endl;
		m1 = true;
		circleScanCount = 0;
		triangleScanCount = 0;
		squareScanCount = 0;
	}
	if (m1 == true && m2 == false && m3 == false && squareScanCount == 20) {
		cout << "Marker 2" << endl;
		m2 = true;
		circleScanCount = 0;
		triangleScanCount = 0;
		squareScanCount = 0;
	}

	if (m2 == true && m3 == false && m4 == false && triangleScanCount == 20) {
		cout << "Marker 3" << endl;
		m3 = true;
		circleScanCount = 0;
		triangleScanCount = 0;
		squareScanCount = 0;
	}

	if (m3 == true && m4 == false && m5 == false && squareScanCount == 20 && circleScanCount == 20) {
		circleScanCount = 0;
		cout << "Marker 4" << endl;
		m4 = true;
		circleScanCount = 0;
		triangleScanCount = 0;
		squareScanCount = 0;
	}

	if (m4 == true && m5 == false && m6 == false && squareScanCount == 20 && triangleScanCount == 20) {
		cout << "Marker 5" << endl;
		m5 = true;
		circleScanCount = 0;
		triangleScanCount = 0;
		squareScanCount = 0;
	}

	if (m5 == true && m6 == false && m7 == false && circleScanCount == 20 && triangleScanCount == 20) {
		cout << "Marker 6" << endl;
		m6 = true;
		circleScanCount = 0;
		triangleScanCount = 0;
		squareScanCount = 0;
	}

	if (m6 == true && m7 == false && circleScanCount == 20 && triangleScanCount == 20 && squareScanCount == 20) {
		cout << "Marker 7" << endl;
		m7 = true;
	}
}


void markerCheck02() {

	cout << "Are you ready to scan the first marker?" << endl;
	cin >> choice;
	while (choice == 'y') {

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
}







/// MAIN

int main(int argc, char* argv[]) {
	if (!cap.isOpened()) {  // if not success, exit program
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	cout << "Turn on camera? y/n" << endl;
	cin >> choice;


	if (choice == 'y' || choice == 'Y')
	{
		cout << "To turn off camera and quit press 'Esc'" << endl;
		OpenCamera = true;

	}


	else if (choice == 'n' || choice == 'N') {
		return 0;
	}

	while (OpenCamera == true) {
		bool bSuccess = cap.read(input); // read a new frame from video
		if (!bSuccess) { //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		//DECIDES COLOR 
		blueRoute = true;
		redRoute = false;
		greenRoute = false;

		cvtColor(input, src, CV_BGR2HSV);
		if (blueRoute)
			inRange(src, Scalar(100, 150, 0), Scalar(140, 255, 255), redImg);


		if (redRoute)
			inRange(src, Scalar(0, 100, 100), Scalar(15, 255, 255), redImg);

		if (greenRoute)
			inRange(src, Scalar(29, 86, 6), Scalar(64, 255, 255), redImg);

		
			squareScan();
		
			circleScan();
		
			triangleScan();


		markerCheck();
		imshow("Shapes", input);

		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}