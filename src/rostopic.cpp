#include "rostopic.hpp"
#include "easylogging++.h"
#include <string>

rostopic::rostopic (std::string topic){
   LOG(INFO) << "Added main ros topic info " << topic << "\n";
   _mainTopic = topic;
}
void rostopic::add_subTopic(std::string subTopic) {
   LOG(INFO) << "Added the subTopic " << subTopic << " to the mainTopic " << _mainTopic << std::endl;
   _subTopic.push_back(subTopic);
}

void rostopic::print_topics() {
   ROS_INFO("Topics subsctibed on");
   for(std::string top : _subTopic){
      ROS_INFO_STREAM("Subs\t" << _mainTopic << "/" << top << "\n");
   }
}


