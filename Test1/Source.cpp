#include "Header.h"


/// VARIABLES
VideoCapture cap(0); // open the video default video camera

//For MAIN
bool OpenCamera;
bool runScan;
bool runSquareScan=true;
bool runCircleScan=true;
char choice;
char colorchoice;
//For CIRCLES
int circleAmount = 0;
vector<Vec3f> circles; //Vector for circle


Mat frame; //The frame our camera takes
Mat frame_gray; // The frame our circle scan uses


Mat dst01, dst02, cdst01, cdst02; //For line detection



//FOR SQUARE:

int squareAmount = 0;
int thresh = 50, N = 5;
const char* wndname = "Square Detection Demo";
vector<vector<Point> > squares;


//FOR TRIANGLES
int triangleAmount = 0;


/// FUNCTIONS

int circleScan() {
	
	if (!frame.data) { //If there is no data close program
		return -1;
	}

	cvtColor(frame, frame_gray, CV_BGR2GRAY); //Convert from color to gray

	GaussianBlur(frame_gray, frame_gray, Size(9, 9), 2, 2); //applies gaussian blur

	HoughCircles(frame_gray, circles, CV_HOUGH_GRADIENT, 1, frame_gray.rows / 8, 200, 100, 0, 0);   /// Apply the Hough Transform to find the circles

																									/// Draw the circles detected

	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		cout << "1 circle Detected" << endl;
		circleAmount++;
	}

}


///FUNCTIONS FOR SQUARES
double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void findSquares(const Mat& image, vector<vector<Point> >& squares)
{
	squares.clear();
	Mat timg(image);
	medianBlur(image, timg, 9); //Apply filter for noise reduction
	Mat gray0(timg.size(), CV_8U), gray;

	vector<vector<Point> > contours;

	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		for (int l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				Canny(gray0, gray, 5, thresh, 5);
				// dilate canny output to remove potential
				// holes between edge segments
				dilate(gray, gray, Mat(), Point(-1, -1));
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				gray = gray0 >= (l + 1) * 255 / N;
			}

			// find contours and store them all as a list
			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			vector<Point> approx;

			// test each contour
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(contourArea(Mat(approx))) > 1000 &&
					isContourConvex(Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						// find the maximum cosine of the angle between joint edges
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quandrange
					// vertices to resultant sequence
					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}

void drawSquares(Mat& image, const vector<vector<Point> >& squares)
{
	for (size_t i = 0; i < squares.size(); i++)
	{
		const Point* p = &squares[i][0];
		cout << "1 square Detected" << endl;
		squareAmount++;
		int n = (int)squares[i].size();
		//dont detect the border
		if (p->x > 3 && p->y > 3)
			polylines(image, &p, &n, 1, true, Scalar(255, 0, 0), 3, LINE_AA);
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
		 runScan = true;
		}
	

	else if (choice == 'n') {
		return 0;
	}


	
	
	while (OpenCamera==true) {
	
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) { //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}



		if(runScan==true) {
			
			if (runSquareScan == true) {
				

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
					cout << "SCANNING FOR MARKER 01!" << endl;  //FIRST MARKER SCAN
					
					cout << "SCANNING FOR CIRCLE!" << endl; //TEST
					circleScan();
					if (circleAmount > 10) {
								break;
							}
						}



				cout << "Are you ready to scan the second marker?" << endl;
				cin >> choice;
				while (choice == 'y') {
					circleAmount = 0;
					cout << "SCANNING FOR MARKER 02!" << endl;  //SECOND MARKER SCAN
					cout << "SCANNING FOR Triangle!" << endl; //TEST
					//triangleScan();
					if (triangleAmount > 25) {
						break;
					}
				}





					cout << "Are you ready to scan the third marker?" << endl;
					cin >> choice;
				
					while (choice == 'y') {
						triangleAmount = 0;
						cout << "SCANNING FOR MARKER 03!" << endl;  //THIRD MARKER SCAN
						cout << "SCANNING FOR Square!" << endl; //TEST
						findSquares(frame, squares);
						drawSquares(frame, squares);
						if (squareAmount > 25) {
							break;
						}
					
					}
					
					
					
					cout << "Are you ready to scan the fourth marker?" << endl;
					cin >> choice;
					while (choice == 'y') {
						squareAmount = 0;
						cout << "SCANNING FOR MARKER 04!" << endl;  //FOURTH MARKER SCAN
						cout << "SCANNING FOR Circle and Triangle!" << endl; //TEST
						circleScan();
						//triangleScan();
						if (circleAmount < 10 && triangleAmount < 25) {
							break;
						}
					}
					
					
					
					cout << "Are you ready to scan the fifth marker?" << endl;
					cin >> choice;
					while (choice == 'y') {
						circleAmount = 0;
						triangleAmount = 0;
						cout << "SCANNING FOR MARKER 05!" << endl;  //FIFTH MARKER SCAN
						cout << "SCANNING FOR Circle and Square!" << endl; //TEST
						circleScan();
						findSquares(frame, squares);
						drawSquares(frame, squares);
						if (circleAmount < 10 && squareAmount < 25) {
							break;
						}

					}
					
					
					cout << "Are you ready to scan the sixth marker?" << endl;
					cin >> choice;
					while (choice == 'y') {
						circleAmount = 0;
						squareAmount = 0;
						cout << "SCANNING FOR MARKER 06!" << endl;  //SIXTH MARKER SCAN
						cout << "SCANNING FOR Square and Triangle!" << endl; //TEST
						circleScan();
						//triangleScan();
						if (circleAmount < 10 && triangleAmount < 25) {
							break;
						}
					}

					
					
					cout << "Are you ready to scan the seventh marker?" << endl;
					cin >> choice;
					while (choice == 'y') {
						circleAmount = 0;
						triangleAmount = 0;
						cout << "SCANNING FOR MARKER 07!" << endl;  //SEVENTH MARKER SCAN
						cout << "SCANNING FOR Square and Triangle!" << endl; //TEST
						circleScan();
						//triangleScan();
						findSquares(frame, squares);
						drawSquares(frame, squares);
						cout << "You have completed the RUN!" << endl;

					}
					if (choice == 'n') {
						cout << "GET READY FOR SCANNING" << endl;
					}

				}
		
		}

			imshow("Square and Circles", frame);
			namedWindow("Square and Circles", CV_WINDOW_AUTOSIZE);

		
	//	namedWindow("Camera with Squares", CV_WINDOW_AUTOSIZE);
		//imshow("Camera with Squares", frame);
		//namedWindow("Camera with Circles", CV_WINDOW_AUTOSIZE); //create a window called "Camera"
		//imshow("Camera with Circles", frame); //Assign orig_image to window Camera
		cout << "CircleAmount:" << circleAmount << endl;
		cout << "SquareAmount:" << squareAmount << endl;
		
	

		
			
			
			 
		} 

			


		
		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			return 0;
		} 
	}
	
}
