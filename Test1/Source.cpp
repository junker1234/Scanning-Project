#include "Header.h"


/// VARIABLES
VideoCapture cap(0); // open the video default video camera

Mat input, src, redImg, dst;

//Shape Scan
int squareScanCount = 0;
int circleScanCount = 0;
int triangleScanCount = 0;

//For MAIN
bool OpenCamera, runScan;
bool runSquareScan = true;
bool runCircleScan = true;
char choice;
char choice01;
char colorchoice;
bool m1, m2, m3, m4, m5, m6, m7, redRoute, blueRoute, greenRoute, ready, b1, b2, b3, b4, b5, b6, b7;

//Button variables
Mat3b canvas;
string buttonText("Click when you are ready to scan a new marker");
string winName = "Reset";
Rect button, M1;

/// FUNCTIONS
void callBackFunc(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		if (button.contains(Point(x, y))) {
			circleScanCount = 0;
			triangleScanCount = 0;
			squareScanCount = 0;
			m1 = false;
			m2 = false;
			m3 = false;
			m4 = false;
			m5 = false;
			m6 = false;
			m7 = false;
			cout << "Reset" << endl;
			rectangle(canvas(button), button, Scalar(0, 0, 255), 2);
		}
	}
	if (event == EVENT_LBUTTONUP) {
		rectangle(canvas, button, Scalar(200, 200, 200), 2);
	}

	imshow(winName, canvas);
	waitKey(1);
}

static double angle(Point pt1, Point pt2, Point pt0) {
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

	for (int i = 0; i < contours.size(); i++) {
		// Approximate contour with accuracy proportional to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		// Detect and label circles
		double area = contourArea(contours[i]);
		Rect r = boundingRect(contours[i]);
		int radius = r.width / 2;

		if (abs(1 - ((double)r.width / r.height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.03) {
			while (circleScanCount < 20) {
				circleScanCount++;
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

	for (int i = 0; i < contours.size(); i++) {
		// Approximate contour with accuracy proportional to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		if (approx.size() >= 4 && approx.size() <= 6) {
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

			// Use the degrees obtained above and the number of vertices to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
				while (squareScanCount < 20) {
					squareScanCount++;
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

	for (int i = 0; i < contours.size(); i++) {
		// Approximate contour with accuracy proportional to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
			continue;

		if (approx.size() == 3) {
			while (triangleScanCount < 20) {
				triangleScanCount++;
				break;
			}
		}
	}
}

void markerCheck() {
	if (circleScanCount == 20 && triangleScanCount == 0 && squareScanCount == 0 && m1 == false) {
		cout << "Marker 1" << endl;
		m1 = true;
		b1 = true;
	}

	if (circleScanCount == 0 && triangleScanCount == 0 && squareScanCount == 20 && m2 == false) {
		cout << "Marker 2" << endl;
		m2 = true;
		b2 = true;
	}

	if (circleScanCount == 0 && triangleScanCount == 20 && squareScanCount == 0 && m3 == false) {
		cout << "Marker 3" << endl;
		m3 = true;
		b3 = true;
	}

	if (circleScanCount == 20 && triangleScanCount == 20 && squareScanCount == 0 && m4 == false) {
		cout << "Marker 4" << endl;
		m4 = true;
		b4 = true;
	}

	if (circleScanCount == 20 && triangleScanCount == 0 && squareScanCount == 20 && m5 == false) {
		cout << "Marker 5" << endl;
		m5 = true;
		b5 = true;
	}

	if (circleScanCount == 0 && triangleScanCount == 20 && squareScanCount == 20 && m6 == false) {
		cout << "Marker 6" << endl;
		m6 = true;
		b6 = true;
	}

	if (circleScanCount == 20 && triangleScanCount == 20 && squareScanCount == 20 && m7 == false) {
		cout << "Marker 7" << endl;
		m7 = true;
		b7 = true;
	}
}

int main(int argc, char* argv[]) {
	if (!cap.isOpened()) {  // if not success, exit program
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	cout << "Turn on camera? y/n" << endl;
	cin >> choice;
	if (choice == 'y' || choice == 'Y') {
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

		if (m1) {
			putText(input, "Correct marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

			/*cout << markercount << endl;

			if (markercount == 2) {
			markercount++;
			putText(input, "Go to marker " + markercount, Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
			putText(input, "You are at the wrong marker, please go to marker " + markercount, Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}*/ 

		}
		if (m2) {
			if (!b1) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
				putText(input, "Correct marker - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}
		if (m3) {
			if (!b1) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b2) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
				putText(input, "Correct marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}
		if (m4) {
			if (!b1) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b2) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
				putText(input, "Correct marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}
		if (m5) {
			if (!b1) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b2) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b4) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
				putText(input, "Correct marker - Find the square and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}
		if (m6) {
			if (!b1) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b2) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b4) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b5) {
				putText(input, "Wrong marker - Find the square and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
				putText(input, "Correct marker - Find the square, circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}
		if (m7) {
			if (!b1) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b2) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b3) {
				putText(input, "Wrong marker - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b4) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b5) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (!b6) {
				putText(input, "Wrong marker - Find the square and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else {
				putText(input, "You found the last marker!", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}
		// Your button
		button = Rect(0, input.rows, input.cols, 50);

		// The canvas
		canvas = Mat3b(input.rows + button.height, input.cols, Vec3b(0, 0, 0));

		// Draw the button
		canvas(button) = Vec3b(0, 0, 0);
		putText(canvas(button), buttonText, Point(button.width*0.2, button.height*0.7), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

		// Draw the image
		input.copyTo(canvas(Rect(0, 0, input.cols, input.rows)));




		// Setup callback function
		namedWindow(winName);
		setMouseCallback(winName, callBackFunc);

		imshow(winName, canvas);

		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}


