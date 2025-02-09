#include <iostream>
#include <ros/ros.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "XYToPixel.h"
#include "slam/Data.h"
#include "nav_msgs/Path.h"
#include <cmath>
#include "ros/package.h"

class Local_path{
	private:
		ros::NodeHandle nh;
		ros::Publisher publisher;
		ros::Subscriber subscriber;
		std::stringstream path_stream;

	public:
		//A local path. It's coordinates origin set as car's position and y-axis set as car's heading
		nav_msgs::Path local_path;
		cv::Mat glob_path;
		//Constructor for local_path_publisher
		Local_path(){
			publisher = nh.advertise<nav_msgs::Path>("/globpath_nearby", 1000);
			subscriber = nh.subscribe("/filtered_data", 1000, &Local_path::callback, this);
			path_stream << ros::package::getPath("slam") << "/src/global_path/glob_path.png";
			glob_path = cv::imread(path_stream.str());
		}

		void callback(const slam::Data data){
			geometry_msgs::PoseStamped loc_pose;
			int curr_pixel_x{}, curr_pixel_y{};
			double pix_heading{};
		       	if(data.theta >= 0) pix_heading = data.theta;
			else pix_heading = data.theta + 2*M_PI;
			double head_coor_x, head_coor_y;

			head_coor_x = (0.5)*sin(pix_heading);
			head_coor_y = (0.5)*cos(pix_heading);

			XYToPixel(curr_pixel_x, curr_pixel_y, data.x, data.y);
			double point_pixel_x{}, point_pixel_y{};
			
			local_path.poses.clear();
			for(int j=0; j<600; j++){
				point_pixel_x = curr_pixel_x + j*head_coor_y;
				point_pixel_y = curr_pixel_y - j*head_coor_x;
				for(int i=1; i<300; i++){
					point_pixel_x += head_coor_x;
					point_pixel_y += head_coor_y;
					
					cv::Vec3b bgr = glob_path.at<cv::Vec3b>(int(point_pixel_y), int(point_pixel_x));
					//loc_pose.pose.position.x = 150+i/2;
					//loc_pose.pose.position.y = 300-j/2;

					if(bgr[1] == 0) {
						loc_pose.pose.position.x =i/2;
						loc_pose.pose.position.y = j/2;
						if(bgr[0] == int(pix_heading*180/M_PI)/2) loc_pose.pose.position.z = 0;
						else if(bgr[0] < int(pix_heading*180/M_PI)/2) loc_pose.pose.position.z = 180 - (int(pix_heading*180/M_PI)/2-bgr[0]);
						else loc_pose.pose.position.z = (-1)*(int(pix_heading*180/M_PI)/2 - bgr[0]);
						local_path.poses.push_back(loc_pose);
						//local_path.at<Vec3b>(150 + i/2, 300-j/2);
					}
				}
			}
			for(int j=0; j<600; j++){
				point_pixel_x = curr_pixel_x + j*head_coor_y;
				point_pixel_y = curr_pixel_y - j*head_coor_x;
				for(int i=1; i<300; i++){
					point_pixel_x -= head_coor_x;
					point_pixel_y -= head_coor_y;
					cv::Vec3b bgr = glob_path.at<cv::Vec3b>(int(point_pixel_y), int(point_pixel_x));
					if(bgr[1] == 0){
						loc_pose.pose.position.x = -i/2;
						loc_pose.pose.position.y = j/2;
						if(bgr[0] == int(pix_heading*180/M_PI)/2) loc_pose.pose.position.z = 0;
						else if(bgr[0] < int(pix_heading*180/M_PI)/2) loc_pose.pose.position.z = 180 - (int(pix_heading*180/M_PI)/2-bgr[0]);
						else loc_pose.pose.position.z = (-1)*(int(pix_heading*180/M_PI)/2 - bgr[0]);
						local_path.poses.push_back(loc_pose);
						//local_path.at<Vec3b>(150 - i/2, 300 - j/2);
					}
				}
			}
			publisher.publish(local_path);
			std::cout << local_path.poses.size() << std::endl;
		}

};

int main(int argc, char **argv){
	ros::init(argc, argv, "locpath_publisher");
	Local_path locpath_publisher;
	ros::spin();
}
