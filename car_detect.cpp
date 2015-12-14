/*


 * @file car_detect.cpp
 * @author Abhishek Kumar Annamraju



This code provides faster car detection.

Also for the first time multiple cascade files are used to detect objects,
 with a benefit that no two objects
are detected twice.

Ever car detected in an image goes through a two stage testing.

The number of checkcascades are set to 1.It is desirable not to change this number.

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


void help()
{
	cout << endl << "USAGE: ./car_detect /path/to/VIDEO" << endl;
	cout << endl << "ckeckcas.xml is the one trained with\
	smallest size parameters and the rest are the main cascades" << endl;
}

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

	int num;

	// getting the input image
	void getimage(Mat src)
	{

		if(!src.data)
		{
			cout << "src not filled" << endl ;
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
		cascade.load(cascade_string);

		if(!cascade.load(cascade_string))
		{
			cout << endl << "Could not load classifier cascade" << endl;
		}
		else
		{
			cout << "cascade : " << cascade_string << " loaded" << endl;
		}
	}

	// loading the test/check cascade
	void checkcascade_load(string checkcascade_string)
	{
		checkcascade.load(checkcascade_string);

		if(!checkcascade.load(checkcascade_string))
		{
			cout << endl << "Could not load classifier checkcascade" << endl;
		}
		else
		{
			cout<< "checkcascade : " << checkcascade_string << " loaded" << endl;
		}
	}

	// function to display input
	void display_input()
	{
		String display_input_window_name = "display_input";
		namedWindow(display_input_window_name);
		imshow(display_input_window_name, image_input);
	}

	// function to display output
	void display_output()
	{
		if(!image_main_result.empty())
		{
			String display_output_window_name = "display_output";
			namedWindow(display_output_window_name);
			imshow(display_output_window_name, image_main_result);
		}
	}

	void setnum()
	{
		num = 0;
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
		const static Scalar colors[] = { CV_RGB(0, 0, 255), \
			CV_RGB(0, 255, 0), \
			CV_RGB(255, 0, 0), \
			CV_RGB(255, 255, 0), \
			CV_RGB(255, 0, 255), \
			CV_RGB(0, 255, 255), \
			CV_RGB(255, 255, 255), \
			CV_RGB(128, 0, 0), \
			CV_RGB(0, 128, 0), \
			CV_RGB(0, 0, 128), \
			CV_RGB(128, 128, 128), \
			CV_RGB(0, 0, 0)};

			Mat gray;

			cvtColor(img, gray, CV_BGR2GRAY);

			Mat resize_image(cvRound (img.rows), cvRound(img.cols), CV_8UC1);

			resize(gray, resize_image, resize_image.size(), 0, 0, INTER_LINEAR);
			equalizeHist(resize_image, resize_image);
			// detection using main classifier
			cascade.detectMultiScale( resize_image, cars, 1.1, 2, 0, Size(10, 10));

			for(vector<Rect>::const_iterator main = cars.begin();\
			    main != cars.end();\
			    main++, i++)
			{
				Mat resize_image_reg_of_interest;
				vector<Rect> nestedcars;
				Point center;
				Scalar color = colors[i%8];

				// getting points for bouding a rectangle over the car detected by main
				int x0 = cvRound(main->x);
				int y0 = cvRound(main->y);
				int x1 = cvRound((main->x + main->width-1));
				int y1 = cvRound((main->y + main->height-1));

				if(checkcascade.empty())
					continue;

				resize_image_reg_of_interest = resize_image(*main);
				checkcascade.detectMultiScale( resize_image_reg_of_interest, \
				                              nestedcars, \
				                              1.1, 2, 0, \
				                              Size(30, 30));

				// testing the detected car by main using checkcascade
				for( vector<Rect>::const_iterator sub = nestedcars.begin();\
				    sub != nestedcars.end();\
				    sub++)
				{
					// getting center points for bouding a circle over the car detected by checkcascade
					center.x = cvRound((main->x + sub->x + sub->width*0.5));
					cen_x = center.x;
					center.y = cvRound((main->y + sub->y + sub->height*0.5));
					cen_y = center.y;
					// if centre of bounding circle is inside the rectangle boundary
					// over a threshold the the car is certified
					if(cen_x>(x0+15) && cen_x<(x1-15) && cen_y>(y0+15) && cen_y<(y1-15))
					{
						rectangle( image_main_result, cvPoint(x0, y0),
						          cvPoint(x1, y1),
						          // detecting boundary rectangle over the final result
						          color, 3, 8, 0);

						// masking the detected car to detect second car if present
						Rect region_of_interest = Rect(x0, y0, x1-x0, y1-y0);
						temp = storage(region_of_interest);
						temp = Scalar(255, 255, 255);

						// num if number of cars detected
						num = num+1;
					}
				}
			}

			if(image_main_result.empty())
			{
				cout << endl << "result storage not successful" << endl;
			}
		}
	};

	int main(int argc, const char** argv)
	{
		// double t = 0;
		// starting timer
		// t = (double)cvGetTickCount();

		// creating a object
		cars detectcars;

		// load the test cascade
		detectcars.checkcascade_load("./cascades/checkcas.xml");

		// set number of cars detected as 0
		detectcars.setnum();

		// Applying various cascades for a finer search.
		string cascades[] = {"./cascades/cas1.xml", \
		"./cascades/cas2.xml", \
		"./cascades/cas3.xml", \
		"./cascades/cas4.xml"};
		for (int i = 0; i < 4; i++)
		{
			string cas = cascades[i];
			detectcars.cascade_load(cas);
		}

		Mat image1, image;

		String input_file_name = argv[1];
		VideoCapture capture(input_file_name);
		if (capture.isOpened())
		{
			cout << "Capture opened." << endl;
			for (;;)
			{
				capture >> image1;
				if (image1.empty())
				{
					break;
				}

				// resizing image to get best experimental results
				resize(image1, image, Size(640, 360), 0, 0, INTER_LINEAR);

				// get the image
				detectcars.getimage(image);
				detectcars.findcars();

				// stopping the timer
				// t = (double)cvGetTickCount() - t;

				// displaying the final result
				detectcars.display_output();

				if (waitKey(1) >= 0)
				{
					capture.release();
					cout << "Capture released." << endl;
					break;
				}
			}
		} else {
			cout << "No capture" << endl;
		}

		return 0;
	}
