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
int theObject1[5] = { 0,0 };
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0, 0, 0, 0);
Rect objectBoundingRectangle1 = Rect(0, 0, 0, 0);

string intToString(int number) 
{

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

int main()
{
	string input;

	FMOD::System *system;
	FMOD::Sound *sound1, *sound2, *sound3, *sound4, *sound5, *sound6, *sound7, *sound8, *sound9, *sound10;
	FMOD::Sound *soundstart;
	FMOD::Sound *Clapping, *Player2;
	FMOD::Sound *scoreP1, *scoreP2, *winnerP1, *winnerP2;
	FMOD::Channel *channel = 0;
	FMOD_RESULT result;
	void *extradriverdata = 0;
	FMOD_SPEAKERMODE speakermode = FMOD_SPEAKERMODE_5POINT1;

	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, extradriverdata);
	system->getSoftwareFormat(0, &speakermode, 0);
	system->createSound("start.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &soundstart);
	system->createSound("tennisserve.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound1);
	system->createSound("tennis2.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Player2);
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
	system->createSound("score_player2.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &scoreP2);
	system->createSound("score_player1.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &scoreP1);
	system->createSound("winnerP1.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &winnerP1);
	system->createSound("winnerP2.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &winnerP2);

	cout << "Plese press g and enter to start a new game" << endl;
	system->playSound(soundstart, 0, true, &channel);
	channel->setPaused(false);
	cin >> input;

	bool gameStart = false;
	if (input == "g")
	{
		gameStart = true;
		Sleep(3000); // game will start after 3 second
	}

	bool objectDetected = false;
	bool objectDetected1 = false;
	bool trackingEnabled = true;
	bool trackingEnabled1 = true;
	bool pause = false;
	Mat frame1, frame2; //player1
	Mat frame3, frame4; //player2
	Mat grayImage1, grayImage2;
	Mat grayImage3, grayImage4;
	Mat differenceImage;
	Mat differenceImage1;
	Mat thresholdImage;
	Mat thresholdImage1;
	//video capture object.
	VideoCapture capture;
	VideoCapture capture1;
	int player1_count = 0; //use for score
	int player2_count = 0;
	capture.open(1); // 1 for external camera
	capture1.open(0);

	if (!capture.isOpened())
	{
		cout << "ERROR ACQUIRING VIDEO FEED\n";
		getchar();
		return -1;
	}

	if (gameStart)
	{
		srand((int)time(0)); //only used in the first time

		for (int k = 1; k <= 5; k++)
		{
			cout << "Round " << k << endl;
			int random_number = rand() % 4;
			int matching_num; // use for matching
			int matching_num1;

			if (random_number == 0)
			{
				cout << "P1 - upper left" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(1.0f, 0, 0, 0, 0, 0, 0, 0);
				channel->setPaused(false);
				Sleep(1000);
				matching_num = 0;
			}
			if (random_number == 1)
			{
				cout << "P1 - upper right" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(0, 1.0f, 0, 0, 0, 0, 0, 0);
				channel->setPaused(false);
				Sleep(1000);
				matching_num = 1;
			}
			if (random_number == 2)
			{
				cout << "P1 - lower left" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(0, 0, 0, 0, 1.0f, 0, 0, 0);
				channel->setPaused(false);
				Sleep(1000);
				matching_num = 2;
			}
			if (random_number == 3)
			{
				cout << "P1 - lower right" << endl;
				system->playSound(sound1, 0, true, &channel);
				channel->setMixLevelsOutput(0, 0, 0, 0, 0, 1.0f, 0, 0);
				channel->setPaused(false);
				Sleep(1000);
				matching_num = 3;
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
					findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);// retrieves external contours

					if (contours.size() > 0)
					{
						objectDetected = true;
					}
					else objectDetected = false;

					if (objectDetected)
					{
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
				imshow("Player 1", frame1);

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
				} 
			} 
			while (matching_num == matching_num1)
			{
				system->playSound(sound2, 0, true, &channel);
				channel->setPaused(false);
				Sleep(2000);
				player1_count++;
				break;
			}
			if (matching_num != matching_num1)
			{
				system->playSound(sound3, 0, true, &channel);
				channel->setPaused(false);
				Sleep(2000);
			}

			for (int x = 0; x < 1; x++)
			{
				random_number = rand() % 4;
				switch (random_number)
				{
				case 0:
					cout << "P2 - upper left" << endl;
					system->playSound(Player2, 0, true, &channel);
					channel->setMixLevelsOutput(1.0f, 0, 0, 0, 0, 0, 0, 0);
					channel->setPaused(false);
					Sleep(1000);
					matching_num = 1;
					break;
				case 1:
					cout << "P2 - upper right" << endl;
					system->playSound(Player2, 0, true, &channel);
					channel->setMixLevelsOutput(0, 1.0f, 0, 0, 0, 0, 0, 0);
					channel->setPaused(false);
					Sleep(1000);
					matching_num = 0;
					break;
				case 2:
					cout << "P2 - lower left" << endl;
					system->playSound(Player2, 0, true, &channel);
					channel->setMixLevelsOutput(0, 0, 0, 0, 1.0f, 0, 0, 0);
					channel->setPaused(false);
					Sleep(1000);
					matching_num = 3;
					break;
				case 3:
					cout << "P2 - lower right" << endl;
					system->playSound(Player2, 0, true, &channel);
					channel->setMixLevelsOutput(0, 0, 0, 0, 0, 1.0f, 0, 0);
					channel->setPaused(false);
					Sleep(1000);
					matching_num = 2;
					break;
				default:
					cout << "error with random num" << endl;
					break;

				} 

				for (int i = 0; i < 50; i++)
				{
					capture1.read(frame3);
					cv::cvtColor(frame3, grayImage3, COLOR_BGR2GRAY);

					capture1.read(frame4);
					cv::cvtColor(frame4, grayImage4, COLOR_BGR2GRAY);

					cv::absdiff(grayImage3, grayImage4, differenceImage1);
					cv::threshold(differenceImage1, thresholdImage1, SENSITIVITY_VALUE, 255, THRESH_BINARY);
					cv::blur(thresholdImage1, thresholdImage1, cv::Size(BLUR_SIZE, BLUR_SIZE));
					cv::threshold(thresholdImage1, thresholdImage1, SENSITIVITY_VALUE, 255, THRESH_BINARY);
					//if tracking enabled, search for contours in our thresholded image
					if (trackingEnabled1)
					{
						bool obkectDetected1 = false;
						Mat temp1;
						thresholdImage1.copyTo(temp1);
						vector< vector<Point> > contours1;
						vector<Vec4i> hierarchy1;
						findContours(temp1, contours1, hierarchy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

						if (contours1.size() > 0)
							objectDetected1 = true;

						else
							objectDetected1 = false;

						if (objectDetected1)
						{
							vector< vector<Point> > largestContourVec1;
							largestContourVec1.push_back(contours1.at(contours1.size() - 1));
							objectBoundingRectangle1 = boundingRect(largestContourVec1.at(0));
							int wpos = objectBoundingRectangle1.x + objectBoundingRectangle1.width / 2;
							int zpos = objectBoundingRectangle1.y + objectBoundingRectangle1.height / 2;

							//update the objects positions by changing the 'theObject' array values
							theObject[3] = wpos, theObject[4] = zpos;
						}
						//make some temp x and y variables so we dont have to type out so much
						int w = theObject[3];
						int z = theObject[4];

						//draw some crosshairs around the object
						circle(frame3, Point(w, z), 20, Scalar(0, 255, 0), 2);
						line(frame3, Point(w, z), Point(w, z - 25), Scalar(0, 255, 0), 2);
						line(frame3, Point(w, z), Point(w, z + 25), Scalar(0, 255, 0), 2);
						line(frame3, Point(w, z), Point(w - 25, z), Scalar(0, 255, 0), 2);
						line(frame3, Point(w, z), Point(w + 25, z), Scalar(0, 255, 0), 2);
						line(frame3, Point(320, 0), Point(320, 500), (255, 0, 0), 3);
						line(frame3, Point(0, 245), Point(640, 245), (255, 0, 0), 3);

						//write the position of the object to the screen
						putText(frame3, "Tracking object at (" + intToString(w) + "," + intToString(z) + ")", Point(w, z), 1, 1, Scalar(255, 0, 0), 2);

						if (w >= 320 && z <= 245)
						{
							matching_num1 = 0;
						}
						else if (w <= 320 && z <= 245)
						{
							matching_num1 = 1;
						}
						else if (w >= 320 && z >= 245)
						{
							matching_num1 = 2;
						}
						else if (w <= 320 && z >= 245)
						{
							matching_num1 = 3;
						}
					}

					//show our captured frame
					imshow("Player 2", frame3);

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
								switch (waitKey())
								{
								case 112:
									pause = false;
									cout << "Code resumed." << endl;
									break;
								}
							}
						}
						else cout << "Code resumed." << endl;
						break;
					} 
				}

				while (matching_num == matching_num1)
				{
					system->playSound(sound2, 0, true, &channel);
					channel->setPaused(false);
					Sleep(2000);
					player2_count++;
					break;
				}
				if (matching_num != matching_num1)
				{
					system->playSound(sound3, 0, true, &channel);
					channel->setPaused(false);
					Sleep(2000);
				}
			} 
		} 
	}

	system->playSound(Clapping, 0, true, &channel);
	channel->setPaused(false);
	Sleep(3500);

	cout << "Score of Player 1: " << player1_count << endl;
	cout << "Score of Player 2: " << player2_count << endl;

	system->playSound(scoreP1, 0, true, &channel);
	channel->setPaused(false);
	Sleep(3000);
	switch (player1_count)
	{
	case 0:
		system->playSound(sound5, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 1:
		system->playSound(sound6, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 2:
		system->playSound(sound7, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 3:
		system->playSound(sound8, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 4:
		system->playSound(sound9, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
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

	system->playSound(scoreP2, 0, true, &channel);
	channel->setPaused(false);
	Sleep(3000);
	switch (player2_count)
	{
	case 0:
		system->playSound(sound5, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 1:
		system->playSound(sound6, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 2:
		system->playSound(sound7, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 3:
		system->playSound(sound8, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 4:
		system->playSound(sound9, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	case 5:
		system->playSound(sound10, 0, true, &channel);
		channel->setPaused(false);
		Sleep(2000);
		break;
	default:
		cout << "error with score" << endl;
		break;
	}

		if (player1_count > player2_count)
		{
			system->playSound(winnerP1, 0, true, &channel);
			channel->setPaused(false);
			Sleep(2000);
			cout << "P1 win!" << endl;
		}
		else if (player1_count < player2_count)
		{
			system->playSound(winnerP2, 0, true, &channel);
			channel->setPaused(false);
			Sleep(2000);
			cout << "P2 win!" << endl;
		}

	}
