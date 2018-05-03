/* Ros callback functions and normalisations before going in to fuzzy
 * classifications.
 *
 * Ros -> callback_normalice -> fuzzy
 *
 */

#ifndef ROSTOPIC_HPP
#define ROSTOPIC_HPP

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "fuzzy_logic.hpp"
#include <iostream>
#include <vector>
#include <string>


class rostopic
{
public:
   rostopic (std::string topic);
   void add_subTopic(std::string subTopic);
   void print_topics();
   void start_subscribing();
   void connect_fuzzy(fuzzyLogic &fu);
   virtual ~rostopic ();

private:
   template<typename Message>
   void _calback(const boost::shared_ptr<Message const>&);
   std::string _mainTopic;
   std::vector<std::string> _subTopic;


};


#endif /* ifndef ROSTOPIC_HPP */
