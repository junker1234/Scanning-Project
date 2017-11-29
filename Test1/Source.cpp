#include "Header.h"

/// VARIABLES
VideoCapture cap(0); // open the video default video camera

Mat input, hsvImg, thrImg, dst;

bool OpenCamera, runScan, m1, m2, m3, m4, m5, m6, m7, redRoute, blueRoute, greenRoute, ready, b1, b2, b3, b4, b5, b6, b7;
char choice, colorchoice;

//Shape Scan
int squareScanCount = 0;
int circleScanCount = 0;
int triangleScanCount = 0;
int arr[7];

//String variables for correctly scanned markers
string marker1 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Now find the square";
string marker2 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Now find the triangle";
string marker3 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Now find the circle and triangle";
string marker4 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Now find the circle and square";
string marker5 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Now find the triangle and square";
string marker6 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Now find the triangle, square and circle";
string marker7 = "Marker " + to_string(arr[0] + 1) + " scanned correctly. Congratulation you are done!";

//Button variables
Mat3b canvas;
string buttonText("Click when you are ready to scan a new marker");
string winName = "Reset";
Rect button, M1;

/// FUNCTIONS

//Moves all elements of array one to the right
void moveArrayRight(int arr[], int size) {
	int temp;
	for (int i = 0; i < (size - 1); i++) {
		temp = arr[size - 1];
		arr[size - 1] = arr[i];
		arr[i] = temp;
	}
}

//
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

			if (b1) {
				arr[0] = 1;
				b1 = false;
			}if (b2) {
				arr[0] = 2;
				b2 = false;
			}if (b3) {
				arr[0] = 3;
				b3 = false;
			}if (b4) {
				arr[0] = 4;
				b4 = false;
			}if (b5) {
				arr[0] = 5;
				b5 = false;
			}if (b6) {
				arr[0] = 6;
				b6 = false;
			}if (b7) {
				arr[0] = 7;
				b7 = false;
			}

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

//
static double angle(Point pt1, Point pt2, Point pt0) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

//Detects circles
void circleScan() {
	vector<vector<Point> > contours;
	findContours(thrImg.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	dst = hsvImg.clone();

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

//Detects squares
void squareScan() {
	// Find contours
	vector<vector<Point> > contours;
	findContours(thrImg.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	dst = hsvImg.clone();

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

//Detects triangles
void triangleScan() {
	// Find contours
	vector<vector<Point> > contours;
	findContours(thrImg.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	dst = hsvImg.clone();

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

//Checks which marker is scanned
void markerCheck() {
	if (circleScanCount == 20 && triangleScanCount == 0 && squareScanCount == 0 && m1 == false) {
		cout << "Marker 1" << endl;
		m1 = true;
		//b1 = true;
	}

	if (circleScanCount == 0 && triangleScanCount == 0 && squareScanCount == 20 && m2 == false) {
		cout << "Marker 2" << endl;
		m2 = true;
		//b2= true;
	}

	if (circleScanCount == 0 && triangleScanCount == 20 && squareScanCount == 0 && m3 == false) {
		cout << "Marker 3" << endl;
		m3 = true;
		//b3 = true;
	}

	if (circleScanCount == 20 && triangleScanCount == 20 && squareScanCount == 0 && m4 == false) {
		cout << "Marker 4" << endl;
		m4 = true;
		//b4 = true;
	}

	if (circleScanCount == 20 && triangleScanCount == 0 && squareScanCount == 20 && m5 == false) {
		cout << "Marker 5" << endl;
		m5 = true;
		//b5 = true;
	}

	if (circleScanCount == 0 && triangleScanCount == 20 && squareScanCount == 20 && m6 == false) {
		cout << "Marker 6" << endl;
		m6 = true;
		//b6 = true;
	}

	if (circleScanCount == 20 && triangleScanCount == 20 && squareScanCount == 20 && m7 == false) {
		cout << "Marker 7" << endl;
		m7 = true;
		//b7 = true;
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

		//Decide which route to take
		blueRoute = true;
		redRoute = false;
		greenRoute = false;

		//Create the threshold
		cvtColor(input, hsvImg, CV_BGR2HSV);
		if (blueRoute)
			inRange(hsvImg, Scalar(100, 150, 0), Scalar(140, 255, 255), thrImg);

		if (redRoute)
			inRange(hsvImg, Scalar(0, 100, 100), Scalar(15, 255, 255), thrImg);

		if (greenRoute)
			inRange(hsvImg, Scalar(29, 86, 6), Scalar(64, 255, 255), thrImg);


		//Call the functions to detect the shapes + which marker is scanned
		squareScan();
		circleScan();
		triangleScan();
		markerCheck();

		//If marker 1 is scanned
		if (m1) {
			if (arr[0] == 0) {
				b1 = true;
				putText(input, marker1, Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				putText(input, "This marker has already been scanned - Find the square", Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				putText(input, "Wrong marker - Find the triangle", Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				putText(input, "Wrong marker - Find the triangle and square", Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				putText(input, "Wrong marker - Find the triangle, square and circle", Point(input.rows*0.25, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.25), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}

		//If marker 2 is scanned
		if (m2) {
			if (arr[0] == 0) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				b2 = true;
				putText(input, "Marker 2 scanned correctly. Now find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				putText(input, "This marker has already been scanned - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				putText(input, "Wrong marker - Find the triangle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				putText(input, "Wrong marker - Find the triangle, square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}

		//If marker 3 is scanned
		if (m3) {
			if (arr[0] == 0) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				b3 = true;
				putText(input, "Marker 3 scanned correctly. Now find the triangle and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				putText(input, "This marker has already been scanned - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				putText(input, "Wrong marker - Find the triangle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				putText(input, "Wrong marker - Find the triangle, square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}

		//If marker 4 is scanned
		if (m4) {
			if (arr[0] == 0) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				putText(input, "Wrong marker - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				b4 = true;
				putText(input, "Marker 4 scanned correctly. Now find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				putText(input, "This marker has already been scanned - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				putText(input, "Wrong marker - Find the triangle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				putText(input, "Wrong marker - Find the triangle, square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}

		//If marker 5 is scanned
		if (m5) {
			if (arr[0] == 0) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				putText(input, "Wrong marker - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				b5 = true;
				putText(input, "Marker 5 scanned correctly. Now find the square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				putText(input, "This marker has already been scanned - Find the triangle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				putText(input, "Wrong marker - Find the triangle, square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}

		//If marker 6 is scanned
		if (m6) {
			if (arr[0] == 0) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				putText(input, "Wrong marker - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				putText(input, "Wrong marker - Find the triangle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				b6 = true;
				putText(input, "Marker 6 scanned correctly. Now find the triangle, square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				putText(input, "This marker has already been scanned - Find the triangle, square and circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
		}

		//If marker 7 is scanned
		if (m7) {
			if (arr[0] == 0) {
				putText(input, "Wrong marker - Find the circle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 1) {
				putText(input, "Wrong marker - Find the square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 2) {
				putText(input, "Wrong marker - Find the triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 3) {
				putText(input, "Wrong marker - Find the circle and triangle", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 4) {
				putText(input, "Wrong marker - Find the circle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 5) {
				putText(input, "Wrong marker - Find the triangle and square", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 6) {
				b7 = true;
				putText(input, "Marker 7 scanned correctly. You are done", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
			}
			else if (arr[0] == 7) {
				putText(input, "You are already done, GO HOME", Point(input.rows*0.5, input.cols*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
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

		//Show window
		imshow(winName, canvas);
		imshow("s", thrImg);


		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}


