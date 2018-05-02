#include "rostopic.hpp"
#include "easylogging++.h"

void yolo_callback(std_msgs::String::ConstPtr &msg){
   LOG(INFO) << "yolo_callback with '" << msg->data.c_str() << "'\n";
}

void blindy_callback(std_msgs::String::ConstPtr &msg){
   LOG(INFO) << "bilindy_callbac with '" << msg->data.c_str() << "'\n";
   
}


