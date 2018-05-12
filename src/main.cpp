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
#include "messagequeue.hpp"

#define CHANNEL_DELAY 1
#define FINDY_FTOI(x) (int) 10*x

Sound global_tones;
ros::NodeHandle *global_rnode;
fuzzyLogic *global_fuzzy_yolo;
std::string global_path;
MessageQueue *queue;


void blindy_findy_callback(const blindy_findy::distances msg);
void yolo_depth_pusion_callback(const yolo_depth_fusion::yoloObjects::ConstPtr &msg);


int main(int argc, char *argv[])
{
   for (int i = 0; i < argc; ++i) {
      std::cout << "argument nr=" << i << " argument=" << argv[i] << std::endl;
   }
   ROS_INFO("===========[ Init ]============== ");
   global_path = ros::package::getPath("priority_ai");
   ros::init(argc, argv, "priority_ai");
   ros::NodeHandle rnode;
   global_rnode = &rnode;
   rnode.setParam("name","priority_ai");
   ROS_INFO("Load sounds in tho the sound object");
   global_tones.add_sound(global_path  + "/resources/ping.wav","ping");
   global_tones.add_sound(global_path + "/resources/sonar.wav","sonar");
   global_tones.add_sound(global_path + "/resources/tom.wav","tom");
   ROS_INFO("Create the fuzzy classifier to yolo_depth_fusion module");
   fuzzyClass *near = new fuzzyClass(1.0,0.0);
   fuzzyClass *mid = new fuzzyClass(0.0,0.0);
   fuzzyClass *far = new fuzzyClass(0.0,1.0);
   fuzzyClass *left= new fuzzyClass(0.0,1.0);
   fuzzyClass *forward= new fuzzyClass(0.0,1.0);
   fuzzyClass *right= new fuzzyClass(0.0,1.0);
   near->add_point(0.5,0.0);
   mid->add_point(0.5,1.0);
   far->add_point(0.5,0.0);
   left->add_point(0.5,0.0);
   forward->add_point(0.5,1.0);
   right->add_point(0.5,0.0);
   ROS_INFO("Add the fussy class to the global fully logic object");
   global_fuzzy_yolo = new fuzzyLogic;
   global_fuzzy_yolo->add_classifier("near", near);
   global_fuzzy_yolo->add_classifier("mid", mid);
   global_fuzzy_yolo->add_classifier("far", far);

   global_fuzzy_yolo->add_classifier("left", left);
   global_fuzzy_yolo->add_classifier("forward", forward);
   global_fuzzy_yolo->add_classifier("right", right);

   ros::ServiceClient robclient = global_rnode->serviceClient<robospeak::sayString>("say_string");
   //MessageQueue queue(&robclient);
   ROS_INFO("Init message queue");
   queue = new MessageQueue(&robclient);
   ROS_INFO("Done with message queue");



   /* Subscribe to blindy_findy module */
   ros::Subscriber sub_blindy = rnode.subscribe("/distances",1,blindy_findy_callback);
   /* Subscirbe to yolo_depth_fusion mudule */
   ros::Subscriber yolo = rnode.subscribe("/yolo_depth_fusion/objects", 1, yolo_depth_pusion_callback);
     ros::spin();
   return 0;
}

void blindy_findy_callback(const blindy_findy::distances msg){
   ROS_INFO("blindy_findy_callback");
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

void yolo_depth_pusion_callback(const yolo_depth_fusion::yoloObjects::ConstPtr &msg){
    /* Connect to the robotspeak client
    * The say_string at the end needs to stay as it is
    * because its used on the other end.
   robospeak::sayString srv;

   srv.request.str = msg->list[0].classification;
   ROS_INFO_STREAM("Sending: " << srv.request.str << "\n");
   if(robclient.call(srv))
      ROS_INFO_STREAM("Return: " << srv.response.str << "\n");

    */
   //queue->push("Hello from yolo_depth_pusion_callback", 1);
   ROS_INFO("yolo_depth_pusion_callback");
   size_t counter=msg->list.size();
   ROS_INFO_STREAM("Got " << counter << "st yoloObjects");
   //queue->message_queue_lock.lock();
   if(counter > 0){
      queue->message_queue_lock.lock();
      for (size_t i = 0; i < counter; ++i) {
         ROS_INFO_STREAM("push message " << msg->list[i].classification << "to MessageQueue");
         queue->push(msg->list[i].classification, 1);
      }
      ROS_INFO("sort the data in the queue");
      //queue->sort();
      ROS_INFO("Unloc mutex");
      queue->message_queue_lock.unlock();
   }
}
