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
#include <ros/package.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rostopic.hpp"
#include "blindy_findy/distances.h"
#include "yolo_depth_fusion/yoloObject.h"
#include "yolo_depth_fusion/yoloObjects.h"
#include "robospeak/sayString.h"



#define CHANNEL_DELAY 1
#define FINDY_FTOI(x) (int) 10*x


Sound global_tones;
std::string global_path;
void blindy_findy_callback(const blindy_findy::distances msg);
void say(std::string text);

int main(int argc, char *argv[])
{
   for (int i = 0; i < argc; ++i) {
      std::cout << "argument nr=" << i << " argument=" << argv[i] << std::endl;
   }
   global_path = ros::package::getPath("priority_ai");
   ros::init(argc, argv, "priority_ai");
   ros::NodeHandle rnode;
   rnode.setParam("name","priority_ai");
   rnode.param<float>("volume",100);
   ROS_INFO_STREAM("\n==============================\nPath=" << global_path << "\n==============================");
   global_tones.add_sound(global_path  + "/resources/ping.wav","ping");
   global_tones.add_sound(global_path + "/resources/sonar.wav","sonar");
   global_tones.add_sound(global_path + "/resources/tom.wav","tom");
   ros::Subscriber sub_blindy = rnode.subscribe("/distances",1000,blindy_findy_callback);
   ros::ServiceClient robclient = rnode.serviceClient<robospeak::sayString>("say_string");
   robospeak::sayString srv;
   ros::Rate loop_rate(20);
   while(ros::ok()){
      srv.request.str = "Hello from priority ai";
      ROS_INFO_STREAM("Sending: " << srv.request.str << "\n");
      if(robclient.call(srv))
         ROS_INFO_STREAM("Return: " << srv.response.str << "\n");
      ros::spinOnce();
      loop_rate.sleep();
   }


   return 0;
}

void blindy_findy_callback(const blindy_findy::distances msg){
   static int counter = 0;
   float right = msg.distR;
   float mid = msg.distM;
   float left = msg.distL;
   int iright = FINDY_FTOI(right);
   int imid = FINDY_FTOI(mid);
   int ileft = FINDY_FTOI(left);
   //ROS_INFO_STREAM("counter=" << counter << "st " << counter%(CHANNEL_DELAY * iright) << "\n");
   usleep(10);
   if (counter%(CHANNEL_DELAY * iright)== 0){
      ROS_INFO("Right");
      global_tones.play_sound("tom",50,right);
   }
   if (counter%(CHANNEL_DELAY*imid)== 0){
      ROS_INFO("Mid");
      global_tones.play_sound("tom",90,mid);
   }
   if (counter%(CHANNEL_DELAY*ileft) == 0){
      ROS_INFO("Left");
      global_tones.play_sound("tom",180-50,left);
   }
   counter++;
}

void say(std::string text){

}
