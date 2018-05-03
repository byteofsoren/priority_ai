//#include "easylogging++.h"
#include <string>
#include <iostream>
#include "sound.hpp"
#include "unistd.h"
//#include "fuzzy_logic.hpp"
//#include "test.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include "rostopic.hpp"
#include "priority_ai/distances.h"



//INITIALIZE_EASYLOGGINGPP

void chatterCallback(const priority_ai::distances msg)
{
  ROS_INFO("I heard: [%f]", msg.distL);
}


int main(int argc, char *argv[])
{
   /*el::Configurations conf("easylogging++.conf");
   el::Loggers::reconfigureLogger("default",conf);
   el::Loggers::reconfigureAllLoggers(conf);
   LOG(INFO) << "The priority AI initialization";*/
   for (int i = 0; i < argc; ++i) {
      std::cout << "argument nr=" << i << " argument=" << argv[i] << std::endl;
   }

   ros::init(argc, argv, "priority_ai");
   ros::NodeHandle rnode;
   ros::Subscriber sub = rnode.subscribe("/distances", 1000, chatterCallback);
   //rnode.setParam("name","priority_ai");
   //rnode.param<float>("volume",100);
   

   ros::spin();




   //test_ros_sub(n);
   //test_audio();
   //test_fuzzy_Class();
   //std::ostream strm = std::ostream('DEBUG.DAT');
   //test_fuzzyLogic();
//   static std::time_t time_now = std::time(nullptr);
//   for (int i = 0; i < 10; ++i) {
//      std::fstream fs;
//      fs.open ("DEBUG.DAT", std::fstream::in | std::fstream::out | std::fstream::app);
//      fs << std::put_time(std::localtime(&time_now), "%y-%m-%d %OH:%OM:%OS") << " [ERROR] " << __FILE__ << "(" << __FUNCTION__ << ":" << __LINE__ << ") >> ";
//      fs.close();
//   }
   return 0;

}
