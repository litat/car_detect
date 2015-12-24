/*
This code provides faster car detection.

Also for the first time multiple cascade files are used to detect objects,
with a benefit that no two objects
are detected twice.

Ever car detected in an image goes through a two stage testing.

The number of checkcascades are set to 1.
It is desirable not to change this number.

USAGE: ./car_detect /path/to/VIDEO

ckeckcas.xml is the one trained with smallest size parameters
and the rest are the main cascades

*/

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <iterator>

using namespace std;
using namespace cv;

// main class
class cars
{
// variables kept public but precaution taken all over the code
public:

	// main input image
	Mat image_input;
	// the final result
	Mat image_main_result;
	// introduced to stop detection of same car more than once
	Mat storage;

	// the main cascade classifier
	CascadeClassifier cascade;
	// a test classifier, car detected by both main and test is stated as car
	CascadeClassifier checkcascade;

	// getting the input image
	void getimage(Mat src)
	{

		if(!src.data)
		{
			cout << "src not filled" << endl;
		}
		else
		{
			image_input = src.clone();
			// initialising storage
			storage = src.clone();
			// initialising result
			image_main_result = src.clone();
		}
	}

	// loading the main cascade
	void cascade_load(string cascade_string)
	{
		if(!cascade.load(cascade_string))
		{
			cout << endl << "Could not load classifier cascade" << endl;
		}
	}

	// loading the test/check cascade
	void checkcascade_load(string checkcascade_string)
	{
		if(!checkcascade.load(checkcascade_string))
		{
			cout << endl << "Could not load classifier checkcascade" << endl;
		}
	}

	// function to display output
	void display_output()
	{
		if(!image_main_result.empty())
		{
			String display_output_window_name = "Display Output";
			namedWindow(display_output_window_name);
			imshow(display_output_window_name, image_main_result);
		}
	}

	void drawMarks(vector<Rect> input_rectangles)
	{
		groupRectangles(input_rectangles, 0, 100);


		for (int i = 0; i < input_rectangles.size(); ++i)
		{
			Rect rect = input_rectangles[i];

			// draw rectangle
			// rectangle(image_main_result, rect, color, width);
			// drawRectangle(rect);

			// draw exclamation mark
			// drawExclamationMark(rect);

			// draw car icon
			drawCarIcon(rect);
		}
	}

	void drawRectangle(Rect rect) {
		Scalar color = Scalar(0, 0, 255);
		int width = rect.width/40;
		rectangle(image_main_result, rect, color, width);
	}

	void drawExclamationMark(Rect rect) {
		Scalar color = Scalar(0, 0, 255);
		Point rect_center = Point(rect.x+rect.width/2, rect.y+rect.height/2);
		int times = rect.width / 30;

		// exclamation mark
		vector<Point> exclamationMarkUpper;
		exclamationMarkUpper.push_back(rect_center + Point(-2, -7) * times);
		exclamationMarkUpper.push_back(rect_center + Point(-2, 2) * times);
		exclamationMarkUpper.push_back(rect_center + Point(2, 2) * times);
		exclamationMarkUpper.push_back(rect_center + Point(2, -7) * times);

		vector<Point> exclamationMarkLower;
		exclamationMarkLower.push_back(rect_center + Point(-2, 3) * times);
		exclamationMarkLower.push_back(rect_center + Point(-2, 7) * times);
		exclamationMarkLower.push_back(rect_center + Point(2, 7) * times);
		exclamationMarkLower.push_back(rect_center + Point(2, 3) * times);

		const Point *pts_upper = (const Point*) Mat(exclamationMarkUpper).data;
		int npts_upper = Mat(exclamationMarkUpper).rows;
		const Point *pts_lower = (const Point*) Mat(exclamationMarkLower).data;
		int npts_lower = Mat(exclamationMarkLower).rows;

		fillPoly(image_main_result, &pts_upper, &npts_upper, 1, color, CV_AA);
		fillPoly(image_main_result, &pts_lower, &npts_lower, 1, color, CV_AA);
		// polylines(image_main_result, &pts_upper, &npts_upper, 1, true,
		//           color, rect.width/60, CV_AA, 0);
		// polylines(image_main_result, &pts_lower, &npts_lower, 1, true,
		//           color, rect.width/60, CV_AA, 0);
	}

	void drawCarIcon(Rect rect) {
		Point rect_center = Point(rect.x+rect.width/2, rect.y+rect.height/2);
		double times = rect.width / 40;

		vector<Point> carIcon;
		carIcon.push_back(rect_center + Point(-5,-7)*times);
		carIcon.push_back(rect_center + Point(-7,-4)*times);
		carIcon.push_back(rect_center + Point(-9,-4)*times);
		carIcon.push_back(rect_center + Point(-9,-2)*times);
		carIcon.push_back(rect_center + Point(-8,-2)*times);
		carIcon.push_back(rect_center + Point(-8,0)*times);
		carIcon.push_back(rect_center + Point(-8,7)*times);
		carIcon.push_back(rect_center + Point(-6,7)*times);
		carIcon.push_back(rect_center + Point(-6,5)*times);
		carIcon.push_back(rect_center + Point(6,5)*times);
		carIcon.push_back(rect_center + Point(6,7)*times);
		carIcon.push_back(rect_center + Point(8,7)*times);
		carIcon.push_back(rect_center + Point(8,0)*times);
		carIcon.push_back(rect_center + Point(8,-2)*times);
		carIcon.push_back(rect_center + Point(9,-2)*times);
		carIcon.push_back(rect_center + Point(9,-4)*times);
		carIcon.push_back(rect_center + Point(7,-4)*times);
		carIcon.push_back(rect_center + Point(5,-7)*times);

		const Point *pts = (const Point*) Mat(carIcon).data;
		int npts = Mat(carIcon).rows;

		Scalar color = Scalar(0, 255, 0);
		fillPoly(image_main_result, &pts, &npts, 1, color, CV_AA);
		// polylines(image_main_result, &pts, &npts, 1, true, color, 3, CV_AA, 0);
	}

	// main function
	void findcars()
	{
		int i = 0;

		Mat img = storage.clone();
		// for region of interest.
		// If a car is detected(after testing) by one classifier,
		// then it will not be available for other one
		Mat temp;

		if(img.empty())
		{
			cout << endl << "Image is empty." << endl;
		}

		int cen_x;
		int cen_y;
		vector<Rect> cars;

		Mat gray;

		cvtColor(img, gray, CV_BGR2GRAY);

		GaussianBlur(gray, gray, Size(7, 7), 3);

		Mat resize_image(cvRound (img.rows), cvRound(img.cols), CV_8UC1);

		resize_image = gray;
		cascade.detectMultiScale(resize_image, cars, 1.1, 15, 0, Size(20, 30));

		for(vector<Rect>::const_iterator main = cars.begin();
		    main != cars.end();
		    main++, i++)
		{
			Mat resize_image_reg_of_interest;
			vector<Rect> nestedcars;
			Point center;

			// getting points for bouding a rect over the car detected by main
			int x0 = cvRound(main->x);
			int y0 = cvRound(main->y);
			int x1 = cvRound((main->x + main->width-1));
			int y1 = cvRound((main->y + main->height-1));

			vector<Rect> input_rectangles;

			if(checkcascade.empty())
				continue;

			resize_image_reg_of_interest = resize_image(*main);
			// testing the detected car by main using checkcascade
			checkcascade.detectMultiScale(resize_image_reg_of_interest,
			                              nestedcars,
			                              1.1, 1, 0,
			                              Size(5, 50));

			for(vector<Rect>::const_iterator sub = nestedcars.begin();
			    sub != nestedcars.end();
			    sub++)
			{
				// getting center points for bouding a circle over the car detected by checkcascade
				center.x = cvRound((main->x + sub->x + sub->width*0.5));
				cen_x = center.x;
				center.y = cvRound((main->y + sub->y + sub->height*0.5));
				cen_y = center.y;
				// if centre of bounding circle is inside the rect boundary
				// over a threshold the the car is certified
				if(cen_x>(x0+10) && cen_x<(x1-10) && cen_y>(y0+10) && cen_y<(y1-10))
				{
					input_rectangles.push_back(Rect(cvPoint(x0, y0), cvPoint(x1, y1)));

					// masking the detected car to detect second car if present
					Rect region_of_interest = Rect(x0, y0, x1-x0, y1-y0);
					temp = storage(region_of_interest);
					temp = Scalar(255, 255, 255);
				}
			}

			drawMarks(input_rectangles);
		}

		if(image_main_result.empty())
		{
			cout << endl << "result storage not successful" << endl;
		}
	}
};

Mat image;
cars detectcars;

void load_cascade() {
	detectcars.checkcascade_load("./cascades/checkcas.xml");

	// Applying various cascades for a finer search.
	string cascades[] = {
		"./cascades/cas1.xml",
		"./cascades/cas2.xml",
		"./cascades/cas3.xml",
		"./cascades/cas4.xml"
	};
	for (int i = 0; i < 4; i++)
	{
		string cas = cascades[i];
		detectcars.cascade_load(cas);
	}
}

void videoCaptureWrap(string input_file_name, void (*callback)(void)){
	VideoCapture capture(input_file_name);
	if (capture.isOpened())
	{
		for (;;)
		{
			capture >> image;
			if (image.empty())
			{
				break;
			}

			callback();

			if (waitKey(1) >= 0)
			{
				break;
			}
		}
	} else {
		cout << "No capture" << endl;
	}
	capture.release();
}

void imageReadWrap(string input_file_name, void(*callback)(void)){
	image = imread(input_file_name);

	callback();

	waitKey(0);
}

void run_find_car(){
	// get the image
	detectcars.getimage(image);
	detectcars.findcars();
	// displaying the final result
	detectcars.display_output();
}

int main(int argc, const char** argv)
{
	// load the test cascade
	load_cascade();

	String input_file_name = argv[1];

	videoCaptureWrap(input_file_name, &run_find_car);
	imageReadWrap(input_file_name, &run_find_car);

	return 0;
}
