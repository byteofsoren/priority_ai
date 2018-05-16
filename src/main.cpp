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
#include <algorithm>
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
int priority_calculation(int x, int y, float distance);


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
   global_fuzzy_yolo->add_classifier("close", near);
   global_fuzzy_yolo->add_classifier("middle", mid);
   global_fuzzy_yolo->add_classifier("far", far);

   global_fuzzy_yolo->add_classifier("left", left);
   global_fuzzy_yolo->add_classifier("middle", forward);
   global_fuzzy_yolo->add_classifier("right", right);

   ros::ServiceClient robclient = global_rnode->serviceClient<robospeak::sayString>("say_string");
   //MessageQueue queue(&robclient);
   ROS_INFO("Init message queue");
   queue = new MessageQueue(&robclient);
   ROS_INFO("Done with message queue");
   queue->message_queue_lock.lock();
   usleep(10);
   queue->push("Kit have started", 1);
   queue->message_queue_lock.unlock();
   usleep(10);

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
   //ROS_INFO_STREAM("Got " << counter << "st yoloObjects");
   //queue->message_queue_lock.lock();
   if(counter > 0){
      ROS_INFO("yolo_depth_pusion_callback: Lock mutex");
      queue->message_queue_lock.lock();
      usleep(10);
      ROS_INFO("yolo_depth_pusion_callback: Lock mutex done");
      for (size_t i = 0; i < counter; ++i) {
         ROS_INFO_STREAM("push message " << msg->list[i].classification << "to MessageQueue");
         queue->push(msg->list[i].classification, priority_calculation(msg->list[i].x,  msg->list[i].y,msg->list[i].distance ));
      }
      ROS_INFO("yolo_depth_pusion_callback sort the data in the queue");
      queue->sort();
      usleep(100);
      ROS_INFO("yolo_depth_pusion_callback: unLock mutex");
      queue->message_queue_lock.unlock();
      usleep(10);
      ROS_INFO("yolo_depth_pusion_callback: unLock mutex done:");
      sleep(1);
      ROS_INFO("return");
   }
}


int priority_calculation(int x, int y, float distance){
   /* priority_calulation assains a value dependent on a set of rules.
    * It does this by first recalculate the int x,y,w and h to
    * floats from 0.0 to 1.0 and then fuzzy fi the images
    */
   if (distance < 0) {
      distance = 0;
   } else if (distance > 20){
      distance = 20;
   }
   distance = distance/20;
   float picture_width = 672;
   float picture_height = 376;
   /* Format the x and y cordinates to float from 0.0 to 1.0 */
   float xf = x/ picture_width;
   float yf = 1.0 - y/ picture_height;
   /* Fuzzy rules */
   /* Rule 1: IF x.middle AND y.low AND distance.close THEN very high */
   float numerator=0;
   float denominator = 0;
   float current;
   //ROS_INFO("===[ Priority calculation ]===");
   //ROS_INFO_STREAM("priority_calculation: fx=" << xf << " y=" << yf << " distance=" << distance);

   current = std::min({global_fuzzy_yolo->classyfyBy_label(xf,"midle"),
                                 global_fuzzy_yolo->classyfyBy_label(yf,"low"),
                                 global_fuzzy_yolo->classyfyBy_label(distance, "middle")});
   numerator += 1. * current;
   denominator += current;
   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);


   /* Rule 2: IF x.middle AND y.low AND distance.middle THEN high */
   current = std::min({global_fuzzy_yolo->classyfyBy_label(xf,"middle"),
                           global_fuzzy_yolo->classyfyBy_label(yf, "low"),
                           global_fuzzy_yolo->classyfyBy_label(distance, "middle")});

   numerator += 5.75 * current;
   denominator += current;

   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);
   /* Rule 3: IF x.middle AND y.low AND distance.far THEN middle */
   current = std::min({global_fuzzy_yolo->classyfyBy_label(xf,"middle"),
                           global_fuzzy_yolo->classyfyBy_label(yf, "low"),
                           global_fuzzy_yolo->classyfyBy_label(distance, "middle")});
   numerator += 10.5 * current;
   denominator += current;

   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);

   /* Rule 4: IF (x.middle OR y.low) AND distance.close THEN high */
   current = std::min({std::max({global_fuzzy_yolo->classyfyBy_label(xf,"middle"),global_fuzzy_yolo->classyfyBy_label(y,"low")}),
                           global_fuzzy_yolo->classyfyBy_label(distance, "close")});

   numerator += 5.75 * current;
   denominator += current;
   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);
   /* Rule 5: IF (x.middle OR y.low) AND distance.middle THEN middle */
   current = std::min({std::max({global_fuzzy_yolo->classyfyBy_label(xf,"middle"),global_fuzzy_yolo->classyfyBy_label(y,"low")}),
                           global_fuzzy_yolo->classyfyBy_label(distance, "middle")}); 
   numerator += 10.5 * current;
   denominator += current;
   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);


   /* Rule 5: IF (x.middle OR y.low) AND distance.far THEN low */
   current = std::min({std::max({global_fuzzy_yolo->classyfyBy_label(xf,"middle"),global_fuzzy_yolo->classyfyBy_label(y,"low")}),
                           global_fuzzy_yolo->classyfyBy_label(distance, "far")}); 
   numerator += 15.25 * current;
   denominator += current;

   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);
   /* IF (x.left OR x.right OR y.middle OR y.hight) AND distance.close THEN middel */
   current = std::min({std::max({global_fuzzy_yolo->classyfyBy_label(xf,"right"),
                        global_fuzzy_yolo->classyfyBy_label(xf,"left"),
                        global_fuzzy_yolo->classyfyBy_label(yf,"middle"),
                        global_fuzzy_yolo->classyfyBy_label(xf,"high")}),
                       global_fuzzy_yolo->classyfyBy_label(distance,"close")});

   numerator += 10.5 * current;
   denominator += current;
   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);
   /*IF (x.left OR x.right OR y.middle OR y.hight) AND distance.middle THEN low */
   current = std::min({std::max({global_fuzzy_yolo->classyfyBy_label(xf,"right"),
                        global_fuzzy_yolo->classyfyBy_label(xf,"left"),
                        global_fuzzy_yolo->classyfyBy_label(yf,"middle"),
                        global_fuzzy_yolo->classyfyBy_label(xf,"high")}),
                       global_fuzzy_yolo->classyfyBy_label(distance,"middle")});

   numerator += 15.25 * current;
   denominator += current;
   //ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator);
   /*IF (x.left OR xclose.right OR y.middle OR y.hight) AND distance.far THEN very low*/
   current = std::min({std::max({global_fuzzy_yolo->classyfyBy_label(xf,"right"),
                        global_fuzzy_yolo->classyfyBy_label(xf,"left"),
                        global_fuzzy_yolo->classyfyBy_label(yf,"middle"),
                        global_fuzzy_yolo->classyfyBy_label(xf,"high")}),
                       global_fuzzy_yolo->classyfyBy_label(distance,"far")});

   numerator += 20.0 * current;
   denominator += current;
   int ret =(int) numerator / denominator;
   ROS_INFO_STREAM("Current=" << current << " numerator=" << numerator << " denominator=" << denominator << " return=" << ret);
   ROS_INFO("===================================");
   return ret;
}
