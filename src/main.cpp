#include <string>
#include <iostream>
#include "sound.hpp"
#include "unistd.h"
#include "fuzzy_logic.hpp"
#include "test.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <vector>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rostopic.hpp"
#include "blindy_findy/distances.h"
#include "yolo_depth_fusion/yoloObject.h"
#include "yolo_depth_fusion/yoloObjects.h"
#include "test.hpp"




Sound global_tones;
void blindy_findy_callback(const blindy_findy::distances msg);

int main(int argc, char *argv[])
{
   for (int i = 0; i < argc; ++i) {
      std::cout << "argument nr=" << i << " argument=" << argv[i] << std::endl;
   }
   ros::init(argc, argv, "priority_ai");
   ros::NodeHandle rnode;
   rnode.setParam("name","priority_ai");
   rnode.param<float>("volume",100);
   global_tones.add_sound("resources/ping.wav");
   global_tones.add_sound("resources/sonar.wav");
   global_tones.add_sound("resources/tom.wav");
   global_tones.play_sound(2,0,0);
   //test_sound();
   //ros::Subscriber sub_blindy = rnode.subscribe("/blindy_findy/distances",10,blindy_findy_callback);
   ros::Subscriber sub_blindy = rnode.subscribe("/distances",1000,blindy_findy_callback);
   ros::spin();

   return 0;
}

void blindy_findy_callback(const blindy_findy::distances msg){
   static int counter = 0;
   if (counter > 20000) {
      float right = msg.distR;
      float mid = msg.distM;
      float left = msg.distL;
      global_tones.play_sound(0,120,right);
      global_tones.play_sound(0,0,mid);
      global_tones.play_sound(0,60,left);
      ROS_INFO_STREAM("left " << left << " mid " << mid << " right " << right << "\n");
      counter = 0;
   }
   counter++;
}
