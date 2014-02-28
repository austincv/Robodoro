//amazebot.cpp

//Template from objectTrackingTutorial written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#define PI 3.14159265

using namespace cv;
using namespace std;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

bool identifiedObjects = false;
bool allObjectsTraversed = false;

//The class for our objects - this will be papers of different colors and shapes

class Paper
{
public:
	Paper(void)
	{
		HSV_min = Scalar(0,0,0);
		HSV_max = Scalar(256,256,256);
	}
	~Paper(void)
	{
	}
	
	int getXPos()
	{
		return xPos;
	}
	void setXPos(int x)
	{
		xPos = x;
	}
	int getYPos()
	{
		return yPos;
	}
	void setYPos(int y)
	{
		yPos = y;
	}
	Scalar getHSV_min()
	{
		return HSV_min;
	}
	Scalar getHSV_max()
	{
		return HSV_max;
	}
	void setHSV_min(Scalar min)
	{
		HSV_min = min;
	}
	void setHSV_max(Scalar max)
	{
		HSV_max = max;
	}

	bool isBotReached()
	{
		return botReached;
	}
	void setBotReached(bool reached)
	{
		botReached = reached;
	}
	
private:

	int xPos, yPos;
	string type;
	Scalar HSV_min;
	Scalar HSV_max;
	
	bool botReached;

};

class Bot
{
public:
	Bot(void)
	{
		botFound = false;
	}
	~Bot(void)
	{
	
	}

	int getXPosFront()
	{
		return xPosFront;
	}
	void setXPosFront(int x)
	{
		xPosFront = x;
	}
	int getYPosFront()
	{
		return yPosFront;
	}
	void setYPosFront(int y)
	{
		yPosFront = y;
	}


	int getXPosBack()
	{
		return xPosBack;
	}
	void setXPosBack(int x)
	{
		xPosBack = x;
	}
	int getYPosBack()
	{
		return yPosBack;
	}
	void setYPosBack(int y)
	{
		yPosBack = y;
	}

	Scalar getHSV_min_Front()
	{
		return HSV_min_Front;
	}
	Scalar getHSV_max_Front()
	{
		return HSV_max_Front;
	}
	void setHSV_min_Front(Scalar min)
	{
		HSV_min_Front = min;
	}
	void setHSV_max_Front(Scalar max)
	{
		HSV_max_Front = max;
	}

	Scalar getHSV_min_Back()
	{
		return HSV_min_Back;
	}
	Scalar getHSV_max_Back()
	{
		return HSV_max_Back;
	}
	void setHSV_min_Back(Scalar min)
	{
		HSV_min_Back = min;
	}
	void setHSV_max_Back(Scalar max)
	{
		HSV_max_Back = max;
	}
	
	bool isBotFound()
	{
		return botFound;
	}
	void setBotFound(bool found)
	{
		botFound = found;
	}	
private:

	int xPosFront, yPosFront;
	int xPosBack, yPosBack;
	string type;
	Scalar HSV_min_Front;
	Scalar HSV_max_Front;

	Scalar HSV_min_Back;
	Scalar HSV_max_Back;

	bool botFound;

};


void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed





}

string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars(){
	//create window for trackbars


	namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	//not so sure why the template used the following, commenting it for now
	/*sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	*/
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}
void drawObject(vector<Paper> thePapers,Mat &frame){

	for(int i=0; i<thePapers.size(); i++)
	{
		int x = thePapers.at(i).getXPos();
		int y = thePapers.at(i).getYPos();

		Scalar color;
         if(!thePapers.at(i).isBotReached())//if not reached
		color = Scalar(0,0,255);
	 else
		color = Scalar(255,255,0);//if reached


		circle(frame,Point(x,y),20,color,2);
			if(y-25>0)
			line(frame,Point(x,y),Point(x,y-25),color,2);
			else line(frame,Point(x,y),Point(x,0),color,2);
			if(y+25<FRAME_HEIGHT)
			line(frame,Point(x,y),Point(x,y+25),color,2);
			else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),color,2);
			if(x-25>0)
			line(frame,Point(x,y),Point(x-25,y),color,2);
			else line(frame,Point(x,y),Point(0,y),color,2);
			if(x+25<FRAME_WIDTH)
			line(frame,Point(x,y),Point(x+25,y),color,2);
			else line(frame,Point(x,y),Point(FRAME_WIDTH,y),color,2);	
			putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,color,2);

	}

}

void drawBot(Bot theBot,Mat &frame){

	int x = theBot.getXPosFront();
	int y = theBot.getYPosFront();

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
		if(y-25>0)
		line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
		if(y+25<FRAME_HEIGHT)
		line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
		if(x-25>0)
		line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
		if(x+25<FRAME_WIDTH)
		line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y)+"FRONT",Point(x,y+30),1,1,Scalar(0,255,0),2);

	x = theBot.getXPosBack();
	y = theBot.getYPosBack();

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
		if(y-25>0)
		line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
		if(y+25<FRAME_HEIGHT)
		line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
		if(x-25>0)
		line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
		if(x+25<FRAME_WIDTH)
		line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
		else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y)+"BACK",Point(x,y+30),1,1,Scalar(0,255,0),2);

}


void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);

}

vector<Paper> trackPapers(Mat threshold,Mat HSV, Mat &cameraFeed){

	vector <Paper> papers;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){
					
					Paper paper;

					paper.setXPos(moment.m10/area);
					paper.setYPos(moment.m01/area);
					
					papers.push_back(paper);
				
					//x = moment.m10/area;
					//y = moment.m01/area;
					objectFound = true;

				}else objectFound = false;

			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(papers,cameraFeed);
			}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
	return papers;
}

//trackBot
Bot trackBot(Mat thresholdFront,Mat thresholdBack,Mat HSV, Mat &cameraFeed){

	Bot bot;

	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	double refArea = 0;
	bool frontFound = false;
	bool backFound = false;

	//For finding Front of the bot

	//find contours of filtered image using openCV findContours function
	findContours(thresholdFront,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object

	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){
					
					bot.setXPosFront(moment.m10/area);
					bot.setYPosFront(moment.m01/area);
							
					//x = moment.m10/area;
					//y = moment.m01/area;
					frontFound = true;

				}else frontFound = false;

			}

		}else putText(cameraFeed,"ADJUST BOT FRONT FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}

//back

	//find contours of filtered image using openCV findContours function
	findContours(thresholdBack,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object

	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){
					
					bot.setXPosBack(moment.m10/area);
					bot.setYPosBack(moment.m01/area);
							
					//x = moment.m10/area;
					//y = moment.m01/area;
					backFound = true;

				}else backFound = false;

			}


		}else putText(cameraFeed,"ADJUST BOT BACK FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}

//backend

		//let user know you found an bot
		if(frontFound ==true && backFound ==true){
			bot.setBotFound(true);
			//draw bot location on screen
			drawBot(bot,cameraFeed);
		}
	return bot;
}

int main(int argc, char* argv[])
{
	//if we would like to calibrate our filter values, set to true.
	bool calibrationMode = false;

	//read calibrated data from file

	string paperColors;
	vector<int> numbers;
	int number;

	ifstream calibrationFile ("paperColors.txt");
	if (calibrationFile.is_open())
		{
		while ( calibrationFile >> number)
			{
      				numbers.push_back(number);
				calibrationFile.get();
    			}
    		calibrationFile.close();
  		}
	else cout << "Unable to open file"; 

	//check if there are 6 values then add it to the calibration
	if(numbers.size()==6)
	for (int i=0; i < numbers.size()/6; i++) 
	{
		H_MIN = numbers[i];
		H_MAX = numbers[i+1];
		S_MIN = numbers[i+2];
		S_MAX = numbers[i+3];
		V_MIN = numbers[i+4];
		V_MAX = numbers[i+5];
	cout<<"Added paper colors from calibration file \n";
	}


	
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat frontThreshold;
	Mat backThreshold;
	Mat HSV;

	if(calibrationMode){
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop

		//vector for storing papers
		vector<Paper> papers;

	while(1){


		Bot aMazeBot;
		
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);


		//track objects based on the HSV values. - red
		//inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
		if(!identifiedObjects)
		{
			inRange(HSV,Scalar(0,81,0),Scalar(33,256,256),threshold);
			morphOps(threshold);
			imshow("objects",threshold);
			papers = trackPapers(threshold,HSV,cameraFeed);
		}
		else
		{	//draw the saved objects	
			drawObject(papers,cameraFeed);
		}

		//create threshold for bot front - green
		inRange(HSV,Scalar(79,42,0),Scalar(97,229,256),frontThreshold);
		morphOps(frontThreshold);
		imshow("front",frontThreshold);

		//create threshold for bot back - blue
		inRange(HSV,Scalar(95,42,4),Scalar(114,229,256),backThreshold);
		morphOps(backThreshold);
		imshow("back",backThreshold);
		
		//track the bot
		aMazeBot = trackBot(frontThreshold,backThreshold,HSV,cameraFeed);

		//cout<<papers.size()<<endl;
		//cout<<endl;
			
		if(aMazeBot.isBotFound() && papers.size() > 0)
		{
			//finding the rotation angle towards the papers
			int x1,y1; //back of bot
			int x2,y2; //front of bot
			int x3,y3; //object
		
			x1 = aMazeBot.getXPosBack();
			y1 = aMazeBot.getYPosBack();

			x2 = aMazeBot.getXPosFront();
			y2 = aMazeBot.getYPosFront();

			double distanceToObj;
			//find the object coordinate to navigate to
			for(int i=0; i<papers.size();i++)
			{
				if(!papers.at(i).isBotReached()) // if the bot has not reached the object
				{
					x3 = papers.at(i).getXPos();
					y3 = papers.at(i).getYPos();
					distanceToObj = pow((x2-x3),2)+pow((y2-y3),2);
					cout<<"Square of the dist to obj : "<<distanceToObj<<endl;

					if(distanceToObj < 2000)
					{	//we have reached so find the next object
						papers.at(i).setBotReached(true);
						if(i == (papers.size()-1))//check if the object is the last
						{				
							//this was the last object			
							allObjectsTraversed = true;
						}
					}
					else
					{
						break; //come out of the for loop and instruct the bot to get to this 
					}
				}
			}
			//dot product to find the angle of rotation for the bot to reach the object
			double dotProduct;
			double angle;



			
			dotProduct = ((x3-x1)*(x2-x1)+(y3-y1)*(y2-y1))/(sqrt(pow((x3-x1),2)+pow((y3-y1),2))*sqrt(pow((x2-x1),2)+pow((y2-y1),2)));

			//testing if the output is correct
			//cout<<"dotProduct :"<<dotProduct<<endl;
			//cout<<"("<<x1<<","<<y1<<")"<<"("<<x2<<","<<y2<<")"<<"("<<x3<<","<<y3<<")"<<endl;

			angle = acos (dotProduct) * 180.0 / PI;
			//cout<<" Angle :"<<(int)angle<<endl;

			//cross product to find the left or right turn
			double crossProduct;

			crossProduct = (x2-x1)*(y3-y1)-(y2-y1)*(x3-x1);
			
			if(identifiedObjects)
			{
				//write output to file
				   ofstream myfile;
				   myfile.open ("directions");
				int direction;
				if(crossProduct>0)
				{	
					//cout<<"Right"<<endl;
					cout<<-1*(int)angle<<endl;
					direction = (int)(angle/4)+45;
					//converting to range 0 to 90
				
				}			
				else 
				{
					//cout<<"Left"<<endl;
					cout<<(int)angle<<endl;
					//converting to range 0 to 90
					direction = (int)(angle/-4)+45;
				}
			

				//check if direction has come straight
				if(direction == 45)
				{
					direction = 92; // 92 is for moving straight
				}
			
				myfile << direction;
				//close the file
				myfile.close();
			}
			else //we havent found objects so just wait
			{
				ofstream myfile;
			 	myfile.open ("directions");
				//bot not found so don't move
				cout<<"0"<<endl;
				myfile <<"45";
				myfile.close();
			}
				
		}
		else
		{
			ofstream myfile;
		 	myfile.open ("directions");
			//bot not found so don't move
			cout<<"0"<<endl;
			myfile <<"45";
			myfile.close();
		}

		//show frames 
		//imshow(windowName2,threshold);

		imshow(windowName,cameraFeed);
		//imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
	       int c = waitKey(30);
	       if( (char)c == 'c' ) { break; }
	       if( (char)c == 'i' ) { identifiedObjects = true; }
	       if( (char)c == 'u' ) { identifiedObjects = false; }

	}

	return 0;
}

