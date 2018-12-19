#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/video/video.hpp>
#include<iostream>
#include "fmod.hpp"
#include <Windows.h>
#include "common.h"
#include <stdlib.h> // srand, rand 
#include <time.h>   



using namespace std;
using namespace cv;

//our sensitivity value to be used in the threshold() function
const static int SENSITIVITY_VALUE = 40;
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
const static int BLUR_SIZE = 20;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = { 0,0 };
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0, 0, 0, 0);


string intToString(int number) {

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

int random(int min, int max) //range : [min, max)
{
	static bool first = true;
	if (first)
	{
		srand(time(NULL)); //seeding for the first time only!
		first = false;
	}
	return min + rand() % ((max + 1) - min);
}


int main()
{
	string input;
	

	FMOD::System *system;
	FMOD::Sound *sound1, *sound2, *sound3, *sound4, *sound5, *sound6, *sound7, *sound8, *sound9, *sound10;
	FMOD::Sound *soundstart;
	FMOD::Sound *Clapping;
	FMOD::Channel *channel = 0;
	FMOD_RESULT result;
	unsigned int version;
	int selection = 0;
	void *extradriverdata = 0;
	FMOD_SPEAKERMODE speakermode = FMOD_SPEAKERMODE_5POINT1;


	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, extradriverdata);
	system->getSoftwareFormat(0, &speakermode, 0);
	system->createSound("start.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &soundstart);
	system->createSound("tennisserve.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound1);
	system->createSound("correctSound.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound2);
	system->createSound("wrongSound.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound3);
	system->createSound("Clapping.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Clapping);
	system->createSound("yourscoreis.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound4);
	system->createSound("zero.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound5);
	system->createSound("one.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound6);
	system->createSound("two.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound7);
	system->createSound("three.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound8);
	system->createSound("four.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound9);
	system->createSound("five.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound10);
	
	cout << "Plese press g and enter to start a new game" << endl;
	system->playSound(soundstart, 0, true, &channel);
	channel->setPaused(false);
	cin >> input;
	
	bool gameStart = false;
	if (input == "g")
	{
		gameStart = true;
		Sleep(3000);
	}

	bool objectDetected = false;
	bool trackingEnabled = true;
	bool pause = false;
	Mat frame1, frame2;
	Mat grayImage1, grayImage2;
	Mat differenceImage;
	Mat thresholdImage;
	//video capture object.
	VideoCapture capture;
	int count = 0; //use for score
	capture.open(0); // 0 for internal, 1 for external

	if (!capture.isOpened())
	{
		cout << "ERROR ACQUIRING VIDEO FEED\n";
		getchar();
		return -1;
	}

	if (gameStart) 
	{
		srand((int)time(0)); //create random 
		int c = 0;
		for (int k = 1; k <= 10; k++)
		{
			cout << "Round " << k << endl;
			int random_number;
			int random_time;
			random_number = rand() % 4; //use time to do random number
			c++;
			random_time = random(1000, 7000);
			int matching_num; // use for matching
			int matching_num1;
			switch (random_number)
			{
			case 0:
				cout << "upper left" << endl;
				//Sleep(random_time);
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(1.0f, 0, 0, 0, 0, 0, 0, 0);
				channel->setPaused(false);
				//Sleep(1000);
				//Sleep(random_time);
				//cout << random_time << endl;
				matching_num = 0;
				break;
			case 1:
				cout << "upper right" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(0, 1.0f, 0, 0, 0, 0, 0, 0);
				channel->setPaused(false);
				//Sleep(1000);
				//Sleep(random_time);
				matching_num = 1;
				break;
			case 2:
				cout << "lower left" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(0, 0, 0, 0, 1.0f, 0, 0, 0);
				channel->setPaused(false);
				//Sleep(1000);
				//Sleep(random_time);
				matching_num = 2;
				break;
			case 3:
				cout << "lower right" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(0, 0, 0, 0, 0, 1.0f, 0, 0);
				channel->setPaused(false);
				//Sleep(1000);
				//Sleep(random_time);
				matching_num = 3;
				break;
			default:
				cout << "error with random num" << endl;
				break;
			}

			for (int i = 0; i < 50; i++)
			{
				capture.read(frame1);
				cv::cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
				capture.read(frame2);
				cv::cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
				cv::absdiff(grayImage1, grayImage2, differenceImage);
				cv::threshold(differenceImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
				cv::blur(thresholdImage, thresholdImage, cv::Size(BLUR_SIZE, BLUR_SIZE));
				cv::threshold(thresholdImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
				//if tracking enabled, search for contours in our thresholded image
				if (trackingEnabled)
				{
					bool objectDetected = false;
					Mat temp;
					thresholdImage.copyTo(temp);
					//these two vectors needed for output of findContours
					vector< vector<Point> > contours;
					vector<Vec4i> hierarchy;
					//find contours of filtered image using openCV findContours function
					//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
					findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);// retrieves external contours

					if (contours.size() > 0)objectDetected = true;
					else objectDetected = false;

					if (objectDetected) {
						vector< vector<Point> > largestContourVec;
						largestContourVec.push_back(contours.at(contours.size() - 1));
						objectBoundingRectangle = boundingRect(largestContourVec.at(0));
						int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
						int ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;

						//update the objects positions by changing the 'theObject' array values
						theObject[0] = xpos, theObject[1] = ypos;
					}
					//make some temp x and y variables so we dont have to type out so much
					int x = theObject[0];
					int y = theObject[1];

					//draw some crosshairs around the object
					circle(frame1, Point(x, y), 20, Scalar(0, 255, 0), 2);
					line(frame1, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
					line(frame1, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
					line(frame1, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
					line(frame1, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
					line(frame1, Point(320, 0), Point(320, 500), (255, 0, 0), 3);
					line(frame1, Point(0, 245), Point(640, 245), (255, 0, 0), 3);

					//write the position of the object to the screen
					putText(frame1, "Tracking object at (" + intToString(x) + "," + intToString(y) + ")", Point(x, y), 1, 1, Scalar(255, 0, 0), 2);

					if (x >= 320 && y <= 245)
					{
						matching_num1 = 0;
					}
					else if (x <= 320 && y <= 245)
					{
						matching_num1 = 1;
					}
					else if (x >= 320 && y >= 245)
					{
						matching_num1 = 2;
					}
					else if (x <= 320 && y >= 245)
					{
						matching_num1 = 3;
					}
				}

				//show our captured frame
				imshow("Frame1", frame1);

				switch (waitKey(10))
				{

				case 27: //'esc' key has been pressed, exit program.
					return 0;

				case 112: //'p' has been pressed. this will pause/resume the code.
					pause = !pause;
					if (pause == true)
					{
						cout << "Code paused, press 'p' again to resume" << endl;
						while (pause == true)
						{
							//stay in this loop until
							switch (waitKey())
							{
								//a switch statement inside a switch statement? Mind blown.
							case 112:
								//change pause back to false
								pause = false;
								cout << "Code resumed." << endl;
								break;
							}
						}
					}
					else cout << "Code resumed." << endl;
					break;
				} // for switch
			} // for i
			while (matching_num == matching_num1)
			{
				system->playSound(sound2, 0, true, &channel);
				channel->setPaused(false);
				Sleep(3000);
				count++;
				break;
			}
			if (matching_num != matching_num1)
			{
				system->playSound(sound3, 0, true, &channel);
				channel->setPaused(false);
				Sleep(3000);
			}

		} //for k

		system->playSound(Clapping, 0, true, &channel);
		channel->setPaused(false);
		Sleep(3500);
		
		cout << "Score: " << count << endl;
		system->playSound(sound4, 0, true, &channel);
		channel->setPaused(false);
		Sleep(1000);
		switch (count)
		{
		case 0:
			system->playSound(sound5, 0, true, &channel);
			channel->setPaused(false);
			Sleep(3000);
			break;
		case 1:
			system->playSound(sound6, 0, true, &channel);
			channel->setPaused(false);
			Sleep(3000);
			break;
		case 2:
			system->playSound(sound7, 0, true, &channel);
			channel->setPaused(false);
			Sleep(3000);
			break;
		case 3:
			system->playSound(sound8, 0, true, &channel);
			channel->setPaused(false);
			Sleep(3000);
			break;
		case 4:
			system->playSound(sound9, 0, true, &channel);
			channel->setPaused(false);
			Sleep(3000);
			break;
		case 5:
			system->playSound(sound10, 0, true, &channel);
			channel->setPaused(false);
			Sleep(3000);
			break;
		default:
			cout << "error with score" << endl;
			break;
		}
	}
} //for main