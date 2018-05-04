#include "test.hpp"
#include "fuzzy_logic.hpp"
#include "sound.hpp"
#include "rostopic.hpp"
#include "ros/ros.h"
#include "std_msgs/String.h"

/*

void test_ros_sub(ros::NodeHandlePtr &n){
   ros::Subscriber rossub = n->subscribe("YOLO", 100, yolo_callback);
   ////ros::spin();
}
*/

void test_sound()
{
   Sound tones;
   int tom = tones.add_sound("resources/tom.wav");
   int test= tones.add_sound("resources/test.wav");
   //int test = tones.add_sound("resources/test.wav");
   std::cout << "tom=" << tom << std::endl;
   tones.play_sound(tom,0 ,0 );
   tones.play_sound(test,0 ,0 );
   tones.list_sounds();
   ROS_INFO("=== LOADED SOUNDS ===");

   //tones.play_sound(test, 0, 0);
   float angle = -180;
   bool runnig = true;
   while (runnig){
      //sleep(2);
      std::cout << "play_sound loop\n";
      tones.play_sound(tom, angle,1.6);
      //sleep(1);
      usleep(500000);
      angle += 10;
      if (angle > 360) {
         runnig=false;
      }
   }
   tones.~Sound();
}

void test_fuzzy_Class(){
   fuzzyClass test1(0,0);
   test1.add_point(0.3,0.0);
   test1.add_point(0.5,1.0);
   test1.add_point(0.7,0.0);
   test1.show_points();
   float x = 0.6;
   printf("In test1 where x=%0.2f the fuzzy classification was %0.2f\n",x, test1.getLevel(x));
   x = 0.3;
   printf("In test1 where x=%0.2f the fuzzy classification was %0.2f\n",x, test1.getLevel(x));
   x = 0.4;
   printf("In test1 where x=%0.2f the fuzzy classification was %0.2f\n",x, test1.getLevel(x));
   x = 0.5;
   printf("In test1 where x=%0.2f the fuzzy classification was %0.2f\n",x, test1.getLevel(x));
   x =1.00;
   printf("In test1 where x=%0.2f the fuzzy classification was %0.2f\n",x, test1.getLevel(x));
}


void test_fuzzyLogic()
{
   fuzzyClass cold(1.0,0.0);
   cold.add_point(0.5, 0.0);

   fuzzyClass warm(0.0,0.0);
   warm.add_point(0.1,0.0);
   warm.add_point(0.5,1.0);
   warm.add_point(0.9,0.0);

   fuzzyClass hot(0.0,1.0);
   hot.add_point(0.5,0);
   fuzzyLogic weather;
   weather.add_classifier("cold",&cold);
   weather.add_classifier("warm",&warm);
   weather.add_classifier("hot",&hot);
   float temp = 0.0;
   while (temp <= 1.0) {
      std::vector<float> results;
      std::vector<std::string> label;
      size_t count = weather.classyfy(temp , &results);
      weather.classyfy_label(&label);
      std::cout << "temp " << temp << "\t";
      for (size_t i = 0; i < count; ++i) {
         std::cout << label[i] << " is " << results[i] << "\t";
      }
      std::cout << "\n";
      temp = temp + 0.1;
   }
}
