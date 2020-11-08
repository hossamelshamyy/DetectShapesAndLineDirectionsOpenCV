#include <QCoreApplication>
#include <QDebug>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "vector"

using namespace cv;
using namespace std;


int triCount = 0;
int squareCount = 0;
int circleCount = 0;
int linesCount = 0;

int redCount = 0;
int blueCount = 0;
int yellowCount = 0;
int greenCount = 0;

void setLabel(cv::Mat& im, const std::string label, vector<Point>& contour);
void detectShapesAndColors(Mat,vector<vector<Point>>);
vector<int> detectLinesSlope(Mat,vector<Vec4i>);
int main()
{

    //  Open and display an image
    Mat image;
    image = imread("D:\\Qt projects\\opencvtest3\\test1.png",IMREAD_COLOR);
    imshow( "Image Before", image );

    //  Convert image to 1 Channel
    Mat grayImage(image);
    cvtColor(image,grayImage,COLOR_BGR2GRAY);

    //  Detect Edges
    Mat cannyImage;
    Canny(grayImage, cannyImage, 0, 70, 5);

    //  find contours and store all points in contours vector
    vector<vector<Point> > contours;
    findContours(cannyImage, contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    detectShapesAndColors(image,contours);

    // find lines and store all points in lines vector
    vector<Vec4i> lines;
    HoughLinesP(cannyImage, lines, 1, CV_PI/180, 500, 1000, 1000);
    vector<int> slopes = detectLinesSlope(image,lines);
    // Draw lines on the image




    qDebug() << "-------------------------";

    if(slopes[0] == slopes[2])
        qDebug() << "The direction is Forward";
    else if(lines[0][0] > lines[2][0])
        qDebug() << "The direction is Left";
    else
        qDebug() << "The direction is Right";

    qDebug() << "-------------------------";

    qDebug() << "Triangles : " << triCount;
    qDebug() << "Squares : " << squareCount;
    qDebug() << "Circles : " << circleCount;
    qDebug() << "Lines : " << linesCount;

    qDebug() << "-------------------------";

    qDebug() << "Red : " << redCount;
    qDebug() << "Blue : " << blueCount;
    qDebug() << "Green : " << greenCount;
    qDebug() << "Yellow : " << yellowCount;

    qDebug() << "-------------------------";

    imshow( "Image After",image);


    waitKey(0);


    return 0;
}

void detectShapesAndColors(Mat image,vector<vector<Point>> contours){
    vector<Point> approx;
    for (uint i = 0; i < contours.size(); i++)
    {
        // Approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, 3, true);
        Rect br = boundingRect(contours[i]);
        Moments m = moments(contours[i],false);
        Point p(br.x + (br.width / 2), br.y + (br.height / 2));
        Point2f pf(m.m10/m.m00 , m.m01/m.m00 );



        Vec3b color = image.at<Vec3b>(Point(p.x,p.y));

        uchar r = color.val[2];
        uchar g = color.val[1];
        uchar b = color.val[0];


        if(approx.size() == 3 || approx.size() == 4 || approx.size() == 16){
            if(r == 0 && g == 255 && b == 0)
                greenCount++;
            else if (r == 255 && g == 0 && b == 0)
                redCount++;
            else if(b == 255 && g == 0 && r == 0)
                blueCount++;
            else if (r == 255&& g == 242 && b ==0)
                yellowCount++;
        }
        int s1 = norm(approx[0]-approx[1])/10;
        int s2 = norm(approx[1]-approx[2])/10;
        int s3 = norm(approx[2]-approx[3])/10;
        int s4 = norm(approx[3]-approx[0])/10;


        switch(approx.size()){
        case 3 :
            setLabel(image,"Triangle",contours[i]);
            triCount++;
            break;
        case 4 :
            if(s1 == s2 && s2 == s3 && s3 == s4){
                squareCount++;
                setLabel(image,"Square",contours[i]);

            }
            else if(s1 ==s3 || s2 == s4){
                linesCount++;
                setLabel(image,"Line",contours[i]);

            }
            break;
        case 16 :
            setLabel(image,"Circle",contours[i]);
            circleCount++;
            break;
        }

    }
}
vector<int> detectLinesSlope(Mat image ,vector<Vec4i> lines){
    vector<int> slopes;
    for (size_t i=0; i<lines.size(); i++) {
        Vec4i l = lines[i];
        Point P1(l[0],l[1]);
        Point P2(l[2],l[3]);
        int angle = abs(atan2(P2.y - P1.y,P2.x - P1.x) * 180.0 / CV_PI);
        slopes.push_back(angle);

        line(image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);

    }
    return slopes;
}
void setLabel(Mat& im, const string label, vector<Point>& contour)
{
    int fontface = FONT_HERSHEY_SIMPLEX;
    double scale = 0.5;
    int thickness = 1;
    int baseline = 0;

    Size text = getTextSize(label, fontface, scale, thickness, &baseline);
    Rect r = boundingRect(contour);

    Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255,255,255), FILLED);
    putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}
