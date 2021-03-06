//
// Created by chaoz on 22/10/17.
//

#include "StateDisplay.h"


StateDisplay::StateDisplay(int videoSrc) {
    colorBlack.val[COLOR_BLUE] = 0;
    colorBlack.val[COLOR_GREEN] = 0;
    colorBlack.val[COLOR_RED] = 0;
    colorOrange.val[COLOR_BLUE] = 0;
    colorOrange.val[COLOR_GREEN] = 140;
    colorOrange.val[COLOR_RED] = 255;
    colorRed.val[COLOR_BLUE] = 0;
    colorRed.val[COLOR_GREEN] = 0;
    colorRed.val[COLOR_RED] = 255;
    colorBlue.val[COLOR_BLUE] = 255;
    colorBlue.val[COLOR_GREEN] = 0;
    colorBlue.val[COLOR_RED] = 0;
    colorGreen.val[COLOR_BLUE] = 0;
    colorGreen.val[COLOR_GREEN] = 255;
    colorGreen.val[COLOR_RED] = 0;
    MyWindowName = "ChaoZ";
    MyLaserOffset = 125;
    MyLaserMaxRange = 8000;
    int scaleFactor = 13;
    MyScaleX = MyScaleY = scaleFactor;
    MyFrontLength = 313 / scaleFactor;
    MyHalfWidth = 253 / scaleFactor;
    MyRobotPosition = Point((int) (MyLaserMaxRange / MyScaleX),
                            (int) ceil((MyLaserMaxRange + MyLaserOffset) / MyScaleY));
    MyLaserPosition = MyRobotPosition;
    MyLaserPosition.y -= MyLaserOffset / MyScaleY;
    MyBackground = Mat(MyRobotPosition.y + 1,
                       (int) (2 * MyLaserMaxRange / MyScaleX) + 1, CV_8UC3, Scalar(255, 255, 255));
    rectangle(MyBackground, Point(MyRobotPosition.x - MyHalfWidth, MyRobotPosition.y),
              Point(MyRobotPosition.x + MyHalfWidth, MyRobotPosition.y - MyFrontLength), Scalar(0, 69, 255), -1);
    circle(MyBackground, MyLaserPosition, MyLaserPosition.x, Scalar(0, 255, 0), 2);
    rectangle(MyBackground,Point(1,1),Point(21,23),Scalar(0,255,0),-1);
    rectangle(MyBackground,Point(1,25),Point(21,47),Scalar(0,255,255),-1);
    rectangle(MyBackground,Point(1,49),Point(21,71),Scalar(255,0,0),-1);
    rectangle(MyBackground,Point(1,73),Point(21,95),Scalar(0,69,255),-1);
    rectangle(MyBackground,Point(1,97),Point(21,119),Scalar(0,0,0),-1);
    rectangle(MyBackground,Point(1,121),Point(21,143),Scalar(255,0,255),-1);
    putText(MyBackground, "8m Range", Point(21, 24), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
    putText(MyBackground, "Open Space", Point(21, 48), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
    putText(MyBackground, "Estimated Path", Point(21, 72), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
    putText(MyBackground, "Pioneer 3at", Point(21, 96), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
    putText(MyBackground, "Obstacle" , Point(21,120),FONT_HERSHEY_PLAIN, 2, Scalar(0,0,0));
    putText(MyBackground, "Following Path", Point(21,144),FONT_HERSHEY_PLAIN,2,Scalar(0,0,0));

    putText(MyBackground, "mm/s", Point(MyBackground.cols-51,12),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0));
    putText(MyBackground, "deg/s", Point(MyBackground.cols-51,24),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0));
    putText(MyBackground, "deg", Point(MyBackground.cols-51,36),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0));
    putText(MyBackground, "TransVel:", Point(MyBackground.cols-191,12),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0));
    putText(MyBackground, "RotVel:", Point(MyBackground.cols-191,24),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0));
    putText(MyBackground, "TurnAng:", Point(MyBackground.cols-191,36),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0));

    MyBackground.copyTo(MyImage);
    MySize.width = (int) (2 * MyLaserMaxRange / MyScaleX);
    MySize.height = (int) (MyLaserMaxRange / MyScaleY);
    namedWindow(MyWindowName, CV_GUI_NORMAL | WINDOW_AUTOSIZE);
    cvSetWindowProperty(MyWindowName.c_str(), CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    cap = VideoCapture(videoSrc);
    if (!cap.isOpened()) { // if not success, exit program
        cout << "Cannot open the video cam" << endl;
        exit(EXIT_FAILURE);
    }
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);

    moveWindow(MyWindowName,0,0);
    moveWindow("MyVideo", 4000,2000);
}

void StateDisplay::DisplayImage() {
    rectangle(MyImage, Point(MyRobotPosition.x - MyHalfWidth, MyRobotPosition.y),
              Point(MyRobotPosition.x + MyHalfWidth, MyRobotPosition.y - MyFrontLength), Scalar(0, 0, 255), -1);
    imshow(MyWindowName, MyImage);
    waitKey(5);
}

void StateDisplay::DisplayBackground() {
    imshow(MyWindowName, MyBackground);
    waitKey(10);
}

void StateDisplay::UpdateSurrounding(double *scan) {
    Point obstacle[181];
    double radToDegree = DEGREE_TO_RAD;
    double rad = 0.0;
    for (int i = 0; i < 181; i++) {
        rad = -i * radToDegree;
        if (scan[i] > MyLaserMaxRange) {
            scan[i] = MyLaserMaxRange;
        }
        obstacle[i].x = ((int) (scan[i] * cos(rad) / MyScaleX) + MyLaserPosition.x);
        obstacle[i].y = ((int) (scan[i] * sin(rad) / MyScaleY) + MyLaserPosition.y);


        if (scan[i] < 350) {
            line(MyImage, MyLaserPosition, obstacle[i], Scalar(0, 0, 255));
        }
//        MyImage.at<Vec3b>(obstacle[i]) = colorBlack;

        if (i > 0) {
//            if (i < 45 || i > 135) {
//                if (norm(obstacle[i - 1] - obstacle[i]) > 100 / MyScaleY) {
//                    line(MyImage, obstacle[i - 1], obstacle[i], Scalar(0, 0, 255));
//                    continue;
//                }
//            }
            line(MyImage, obstacle[i - 1], obstacle[i], Scalar(0, 0, 0),2);
        }
    }
}

int StateDisplay::SearchFreeSpace(double *scan, double distThres, int countThres, double angle) {
    int count = 0;
    int mid = -1;
    int followMid = -1;
    int followCount;
    double radToDegree = DEGREE_TO_RAD;
    double dist;
    g_leftClosest=20000.1;
    g_rightClosest=20000.1;
    double rad;
    double sumRange = 0.0;
    double halfSumRange = 0.0;
    int i;
    Point midPoint, freeSpacePoint;
    for (i = 0; i < 181; i++) {
        if (scan[i] > distThres) {
            if (count == 0) mid = -1;
            count++;
            sumRange += scan[i];
            rad = -i * radToDegree;
            freeSpacePoint.x = ((int) (scan[i] * cos(rad) / MyScaleX) + MyLaserPosition.x);
            freeSpacePoint.y = ((int) (scan[i] * sin(rad) / MyScaleY) + MyLaserPosition.y);
            if (scan[i] > distThres) {
                line(MyImage, MyLaserPosition, freeSpacePoint, Scalar(0, 255, 255),2);
            }
        }
        if (scan[i] <= distThres) {
            if (count > countThres) {
                halfSumRange = 0;
//                for (int j = i - count; j < i; j++) {
//                    halfSumRange += scan[j];
//                    if (halfSumRange > sumRange / 2) {
//                        mid = j;
//                        break;
//                    }
//                }
                mid = i - count / 2;
                rad = -mid * radToDegree;
                if (scan[i] > MyLaserMaxRange) {
                    scan[i] = MyLaserMaxRange;
                }
                midPoint = Point(((int) (scan[mid] * cos(rad) / MyScaleX) + MyLaserPosition.x),
                                 ((int) (scan[mid] * sin(rad) / MyScaleY) + MyLaserPosition.y));
                if (followMid == -1) {
                    followMid = mid;
                    followCount = count;
                }
                if (abs(mid - angle) < abs(followMid - angle)) {
                    followMid = mid;
                    followCount = count;
                }
            }
            count = 0;
            sumRange = 0;

        }
    }

    if (count > countThres && followMid == -1) {
        followMid = i - count / 2;
        followCount = count;
        if (scan[followMid] > MyLaserMaxRange) {
            scan[followMid] = MyLaserMaxRange;
        }
    }
    for(int i=0;i<followMid-followCount/2;i++){
        if(i>60){
            break;
        }
        if(scan[i]>distThres){
            continue;
        }
        dist = scan[i]*cos(i*M_PI/180);
        if(dist<g_rightClosest){
            g_rightClosest = dist;
        }
    }
    for(int i = followMid+followCount/2;i<181;i++){
        if(i<120){
            i=120;
        }
        if(scan[i]>distThres){
            continue;
        }
        dist = scan[i]*cos((180-i)*M_PI/180);
        if(dist<g_leftClosest){
           g_leftClosest = dist;
        }
    }

    if(g_rightClosest<distThres||g_leftClosest<distThres){
        double closest = g_rightClosest<g_leftClosest? g_rightClosest:g_leftClosest;
        double ratio = (-g_rightClosest+g_leftClosest)/closest;
        if(ratio>1) ratio = 1;
        if(ratio<-1) ratio = -1;
        followMid += ratio*(followCount/2);
        cout<<"ratio:"<<ratio<<" closest:"<<closest<<endl;
    }

    stringstream strstream;
    if(followMid!=-1){
        rad = -followMid * radToDegree;
        midPoint = Point(((int) (scan[followMid] * cos(rad) / MyScaleX) + MyLaserPosition.x),
                         ((int) (scan[followMid] * sin(rad) / MyScaleY) + MyLaserPosition.y));
        line(MyImage, MyLaserPosition, midPoint, Scalar(255, 0, 255),2);
        if(followMid<90&&followMid>=10) strstream<<"   "<<(90-followMid);
        if(followMid<10) strstream<<"    "<<(90-followMid);
        if(followMid>=100) strstream<<"  -"<<(followMid-90);
        if(followMid<100&&followMid>90) strstream<<"   -"<<(followMid-90);
        if(followMid==90) strstream<<"    0";
    }
    if(followMid==-1) strstream<<" --- ";
    putText(MyImage,strstream.str(),Point(MyImage.cols-101,36),FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
    return followMid;
}

void StateDisplay::AddWayPoint(double dist, double ang, Scalar color) {
    Point waypoint;
    double rad;
    rad = ang * DEGREE_TO_RAD;
    if (dist > MyLaserMaxRange) {
        dist = MyLaserMaxRange - 15;
    }
    waypoint.x = ((int) (dist * -sin(rad) / MyScaleX) + MyRobotPosition.x);
    waypoint.y = ((int) (dist * -cos(rad) / MyScaleY) + MyRobotPosition.y);
    if (waypoint.y > MyRobotPosition.y) {
        waypoint.y = MyRobotPosition.y;
    }
    circle(MyImage, waypoint, 2, color, -1);
}

void StateDisplay::Clear() {
    MyImage = MyBackground.clone();
}

void StateDisplay::SaveImage(string name) {
    imwrite(name, MyImage);
}

void StateDisplay::AddLaserPoint(double dist, double ang, Scalar color) {
    Point waypoint;
    double rad;
    rad = ang * DEGREE_TO_RAD;
    if (dist > MyLaserMaxRange) {
        dist = MyLaserMaxRange - 15;
    }
    waypoint.x = ((int) (dist * -sin(rad) / MyScaleX) + MyLaserPosition.x);
    waypoint.y = ((int) (dist * -cos(rad) / MyScaleY) + MyLaserPosition.y);
    if (waypoint.y > MyLaserPosition.y) {
        waypoint.y = MyLaserPosition.y;
    }
    circle(MyImage, waypoint, 2, color, -1);
}

void StateDisplay::MotionEstimate(double linearVel, double angularVel) {
//    if(linearVel*1000.0<50) linearVel = 50/1000.0;
    double angleStep = angularVel * TIME_STEP;
    double lengthStep = linearVel * TIME_STEP * 1000.0;
    Point lastEstimate;
    Point estimatePath;
    double angle, length;
    lastEstimate = MyRobotPosition;
    for (int i = 1; i < 100; i++) {
        angle = angleStep*i;
        length = lengthStep;
        estimatePath = Point((int)(sin(-angle) * length / MyScaleX + lastEstimate.x),
                             (int)(-cos(angle) * length / MyScaleY + lastEstimate.y));
        if (estimatePath.y > MyRobotPosition.y) break;
        if (estimatePath.y < 0) break;
        if (abs(estimatePath.x - MyRobotPosition.x) > MyRobotPosition.x) break;
        if (sqrt(pow(estimatePath.x-MyRobotPosition.x,2)+pow(estimatePath.y-MyRobotPosition.y,2))>200) break;
        line(MyImage,estimatePath,lastEstimate,Scalar(255,0,0),2);
//        circle(MyImage,estimatePath,1,Scalar(255,0,0),-1);
        lastEstimate = estimatePath;
    }
    stringstream strstream;
    strstream<<(int)(linearVel*1000);
    putText(MyImage,strstream.str(),Point(MyImage.cols-101,12),FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
    stringstream strstream1;
    strstream1<<(int)(angularVel*180/M_PI);
    putText(MyImage,strstream1.str(),Point(MyImage.cols-101,24),FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));


}

string StateDisplay::readQR() {
    Mat frame, grey;
    string result;
    bool bSuccess = cap.read(frame); // read a new frame from video
    if (!bSuccess) {
        cout << "Cannot read a frame from video stream" << endl;
        exit(EXIT_FAILURE);
    }
    cvtColor(frame, grey, CV_BGR2GRAY);

    int width = frame.cols;
    int height = frame.rows;
//        for(int i=0;i<grey.cols;i++){
//            for(int j=0;j<grey.rows;j++){
//                if(grey.at<uchar>(i,j)<80){
//                    grey.at<uchar>(i,j) = 0;
//                }
//            }
//        }
    uchar *raw = (uchar *) grey.data;
    // wrap image data
    Image image(width, height, "Y800", raw, width * height);
    // scan the image for barcodes
    int n = scanner.scan(image);
    // extract results
    for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
        vector<Point> vp;
        result = symbol->get_data();
        int n = symbol->get_location_size();
        for (int i = 0; i < n; i++) {
            vp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
        }
        RotatedRect r = minAreaRect(vp);
        Point2f pts[4];
        r.points(pts);
        for (int i = 0; i < 4; i++) {
            line(frame, pts[i], pts[(i + 1) % 4], Scalar(255, 0, 0), 3);
        }
    }


    imshow("MyVideo", frame); //show the frame in "MyVideo" window
//    frame.copyTo(MyImage(Rect(0,MyImage.rows-height/2,width/2,height/2)));
    waitKey(1);
    return result;
}

int StateDisplay::AddRoomText(string QRMessage,bool clear){
    static int count = 0;
    if(clear){
        count = 0;
        rectangle(MyBackground,Point(MyBackground.cols-48,MyBackground.rows-18),Point(MyBackground.cols-102,MyBackground.rows-122),Scalar(255,255,255),-1);
        rectangle(MyBackground,Point(MyBackground.cols-48,MyBackground.rows-18),Point(MyBackground.cols-102,MyBackground.rows-122),Scalar(0,200,200),2);
        return count;
    }
    putText(MyImage,QRMessage,Point(MyImage.cols-201,72),FONT_HERSHEY_PLAIN,1,Scalar(0,0,0),2);
    rectangle(MyBackground,Point(MyBackground.cols-50,MyBackground.rows-20-10*count),Point(MyBackground.cols-100,MyBackground.rows-17-10*(count+1)),Scalar(50,205,50),-1);
    count++;
    return count;
}
