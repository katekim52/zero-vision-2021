#include "ros/ros.h"
#include "ros/time.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include "slam/Yoloinfo.h"
#include "slam/Yolomaster.h"
#include "geometry_msgs/Point.h"
#include "math.h"

using namespace std;
using namespace cv;

class YoloSender{
    
    private:
    ros::NodeHandle nh_;
    ros::Publisher pub_;
    ros::Subscriber sub_;
    vector<int> label;
    vector<double> width;
    vector<double> height;
    vector<geometry_msgs::Point> points;
    Mat image;
    int img_row;
    int img_col;

    public:
    YoloSender(){
        pub_ = nh_.advertise<slam::Yolomaster>("/yolo_info", 10);
        //get_picture();

    }

    void send_info(){
        initialize_info();
        Mat image = imread("/home/jungwonsuhk/vision/signa10586.jpg",IMREAD_COLOR);
        img_col = image.cols;
        img_row = image.rows;
        push_info(); // Need to be revised
        
        slam::Yolomaster rt;
        vector<slam::Yoloinfo> rt_yolo;
        // vector<int> rt_label;
        // vector<int> rt_width;
        // vector<int> rt_height;
        vector<geometry_msgs::Point> rt_points;
        rt.header.stamp = ros::Time::now();
        for(int i=0; i<label.size(); i++){
            slam::Yoloinfo yolo;
            rt_yolo.push_back(yolo);
            rt_yolo[i].label.push_back(label.at(i));
            rt_yolo[i].width.push_back(width.at(i));
            rt_yolo[i].height.push_back(height.at(i));
            rt_yolo[i].points.push_back(points.at(i));
            // rt_label.push_back(label.at(i));
            // rt_width.push_back(width.at(i));
            // rt_height.push_back(height.at(i));
            // rt_points.push_back(points.at(i));
        }
        //cout << rt_label.at(0) << "<" << rt_width.at(0) << "<" << rt_height.at(0) << endl;

        // for(int j=0; j<label.size(); j++){
        //     int start_x = rt_points.at(j).x - rt_width.at(j)/2;
        //     int start_y = rt_points.at(j).y - rt_height.at(j)/2;
        //     int end_x = rt_points.at(j).x + rt_width.at(j)/2;
        //     int end_y = rt_points.at(j).y + rt_height.at(j)/2;
        //     rectangle(image, Point(start_x, start_y), Point(end_x, end_y), Scalar(255,0,0), 3);
        // }

        //imshow("hello", image);
        //waitKey(0);
        rt.yolomaster = rt_yolo;
        // rt.points = rt_points;
        // rt.label = rt_label;
        // rt.width = rt_width;
        // rt.height = rt_height;
        
        cout << "Width" << rt.yolomaster[0].width.at(0) << "Height" << rt.yolomaster[0].height.at(0) << endl;
        pub_.publish(rt);
        cout << "Sended" << endl;
    }
    
    void initialize_info(){
        label.clear();
        width.clear();
        height.clear();
        points.clear();
    }

    //From YOLO File, Extract Information :: Message Type (double width[], double height[])
    void push_info(){
        label.push_back(4);
        label.push_back(7);
        geometry_msgs::Point pt1, pt2;
        pt1.x = int(0.776563 * img_col);
        pt1.y = int(0.350694 * img_row);
        pt2.x = int(0.5 * img_col);
        pt2.y = int(0.5 * img_row);
        points.push_back(pt1);
        points.push_back(pt2);
        width.push_back(int(0.059375 * img_col));
        width.push_back(int(0.069375 * img_col));
        height.push_back(int(0.073611 * img_row));
        height.push_back(int(0.083611 * img_row));
    }

};


int main(int argc, char **argv){
    ros::init(argc, argv, "yolo_sender");
    ros::start();
    ros::Rate loop{1};
    YoloSender yolo_sender;
    while(ros::ok())
    {
        yolo_sender.send_info();
        ros::spinOnce();
        loop.sleep();
    }
    //ros::spin();
}