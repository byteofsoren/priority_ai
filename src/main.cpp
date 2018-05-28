#include <string>
#include <iostream>
#include "sound.hpp"
#include "unistd.h"
#include <fstream>
#include <ctime>
#include <math.h>
#include <iomanip>
#include <vector>
#include <ros/package.h>
#include <algorithm>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "blindy_findy/distances.h"
#include "yolo_depth_fusion/yoloObject.h"
#include "yolo_depth_fusion/yoloObjects.h"
#include "robospeak/sayString.h"

float FINDY_FTOI(float distance){
    float k = -0.05;
    float t = 5;
        return 1.0 + t / (k* distance + 1);
}



Sound global_tones;
ros::NodeHandle *global_rnode;
std::string global_path;

int picture_width = 672;
int picture_height = 376;

ros::ServiceClient* roboPtr;

void blindy_findy_callback(const blindy_findy::distances msg);
void yolo_depth_fusion_callback(const yolo_depth_fusion::yoloObjects::ConstPtr &msg);
int priority_calculation(int x, int y, float distance);
float filterDistance(float distance);

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

   ros::ServiceClient robclient = global_rnode->serviceClient<robospeak::sayString>("say_string");
   roboPtr = &robclient;
   
   
   
   /* Subscribe to blindy_findy module */
   ros::Subscriber sub_blindy = rnode.subscribe("/distances",1,blindy_findy_callback);
   /* Subscirbe to yolo_depth_fusion mudule */
   ros::Subscriber yolo = rnode.subscribe("/yolo_depth_fusion/objects", 1, yolo_depth_fusion_callback);
     ros::spin();
   return 0;
}

float filterDistance(float distance) {
    if (distance < 0)
        return 0.1;
    if (distance > 20)
        return 20.1;
    return distance;
}


void blindy_findy_callback(const blindy_findy::distances msg){
   ROS_INFO("blindy_findy_callback");
   static int counter = 0;
   float right = filterDistance(msg.distR);
   float mid = filterDistance(msg.distM);
   float left = filterDistance(msg.distL);
   int iright = (int)FINDY_FTOI(right);
   int imid = (int)FINDY_FTOI(mid);
   int ileft = (int)FINDY_FTOI(left);
   //ROS_INFO_STREAM("counter=" << counter << "st " << counter%(CHANNEL_DELAY * iright) << "\n");
   //usleep(10);
   if (counter % iright == 0){
      ROS_INFO("Right");
      global_tones.play_sound("tom",50,right);
   }
   if (counter % imid == 0){
      ROS_INFO("Mid");
      global_tones.play_sound("tom",90,mid);
   }
   if (counter % ileft == 0){
      ROS_INFO("Left");
      global_tones.play_sound("tom",180-50,left);
   }
   counter++;
}

struct prioObject{
    int priority;
    const yolo_depth_fusion::yoloObject *data;
};

bool  compStruct(const struct prioObject &a, const struct prioObject &b) {
   return a.priority < b.priority;
}

std::string position(int x) {
    if (x < picture_width / 3)
            return "left  ";
    if (x > picture_width - picture_width / 3)
            return "right  ";
    return "middle  ";
}



size_t threshold = 1;

void yolo_depth_fusion_callback(const yolo_depth_fusion::yoloObjects::ConstPtr &msg){
    /* Connect to the robotspeak client
    * The say_string at the end needs to stay as it is
    * because its used on the other end.
    */
   robospeak::sayString srv;

   ROS_INFO("yolo_depth_fusion_callback");
   size_t counter=msg->list.size();
   //ROS_INFO_STREAM("Got " << counter << "st yoloObjects");
   if(counter > 0){
   
      std::vector<struct prioObject> objectVector;
 
      for (size_t i = 0; i < counter; ++i) {
         ROS_INFO_STREAM("Class " << msg->list[i].classification);
         struct prioObject *currentYoloObject = new struct prioObject;
         currentYoloObject->data = &(msg->list[i]);
         currentYoloObject->priority = priority_calculation(msg->list[i].x,  msg->list[i].y,msg->list[i].distance);
         objectVector.push_back(*currentYoloObject);
         delete currentYoloObject;
      }
      std::sort(objectVector.begin(), objectVector.end(),compStruct);
      for (size_t i = 0; i < threshold; ++i) {     
        srv.request.str = objectVector[i].data->classification + " " + std::to_string((int)round(filterDistance(objectVector[i].data->distance))) + " " + position(objectVector[i].data->x);
        ROS_INFO_STREAM("Sending: " << srv.request.str << " With priority " << objectVector[i].priority << "\n");
        if(roboPtr->call(srv))
            ROS_INFO_STREAM("Return: " << srv.response.str << "\n");
      }
      objectVector.clear();

   }
   usleep(500000);
}


/***********************************
 * Fuzzy
 ***********************************/


float close(float input) {
    if (input < 0) {
        return 1.0;
    } else if (input > 0.5) {
        return 0.0;
    } else {
        return -2 * input + 1;
    }

}

float middle(float input) {
    if (input < 0) {
        return 0.0;
    } else if (input > 1) {
        return 0.0;
    } else if (input < 0.5) {
        return 2 * input;
    } else {
        return -2 * input + 2;
    }
}

float far(float input) {
    if (input < 0.5) {
        return 0.0;
    } else if (input > 1) {
        return 1.0;
    } else {
        return 2 * input - 1;
    }
}

#define low(input) far(input)

#define high(input) close(input)

#define left(input) close(input)

#define right(input) far(input)


int priority_calculation(int x, int y, float distance){
   /* priority_calulation assains a value dependent on a set of rules.
    */

   /* Normalize distance */
   if (distance < 0) {
      distance = 0.1;
   } else if (distance > 20){
      distance = 20.0;
   }
   distance = distance/20;

   /*Normalize the x and y cordinates to float from 0.0 to 1.0 */
   float xf = x/ (float) picture_width;
   float yf = y/ (float) picture_height;
   
   
   /* Setup variables for later de-fuzzification of rules. */
   float numerator=0;
   float denominator = 0;
   float current;


   /* Fuzzy rules */
   /* Rule 1: IF x.middle AND y.low AND distance.close THEN very high */
   current = std::min({middle(xf),
                       low(yf),
                       close(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 1. * current;
   denominator += current;


   /* Rule 2: IF x.middle AND y.low AND distance.middle THEN high */
   current = std::min({middle(xf),
                       low(yf),
                       middle(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 5.75 * current;
   denominator += current;


   /* Rule 3: IF x.middle AND y.low AND distance.far THEN middle */
   current = std::min({middle(xf),
                       low(yf),
                       far(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 10.5 * current;
   denominator += current;


   /* Rule 4: IF (x.middle OR y.low) AND distance.close THEN high */
   current = std::min({std::max({middle(xf),low(yf)}),
                       close(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 5.75 * current;
   denominator += current;


   /* Rule 5: IF (x.middle OR y.low) AND distance.middle THEN middle */
   current = std::min({std::max({middle(xf),low(yf)}),
                       middle(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 10.5 * current;
   denominator += current;


   /* Rule 6: IF (x.middle OR y.low) AND distance.far THEN low */
   current = std::min({std::max({middle(xf),low(yf)}),
                           far(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 15.25 * current;
   denominator += current;


   /* Rule 7: IF (x.left OR x.middle OR x.right OR y.middle OR y.high OR y.low) AND distance.close THEN middel */
   current = std::min({std::max({left(xf),
                                 middle(xf),
                                 right(xf),
                                 low(yf),
                                 middle(yf),
                                 high(yf)}),
                       close(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 10.5 * current;
   denominator += current;


   /* Rule 8: IF (x.left OR x.middle OR x.right OR y.middle OR y.high OR y.low) AND distance.middle THEN low */
   current = std::min({std::max({left(xf),
                                 middle(xf),
                                 right(xf),
                                 low(yf),
                                 middle(yf),
                                 high(yf)}),
                       middle(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 15.25 * current;
   denominator += current;


   /* Rule 9: IF (x.left OR x.middle OR x.right OR y.middle OR y.high OR y.low) AND distance.far THEN very low*/
   current = std::min({std::max({left(xf),
                                 middle(xf),
                                 right(xf),
                                 low(yf),
                                 middle(yf),
                                 high(yf)}),
                       far(distance)});
   assert(current >= 0 && current <= 1);
   numerator += 20.0 * current;
   denominator += current;



   int ret =round(numerator / denominator);
   return ret;
}
