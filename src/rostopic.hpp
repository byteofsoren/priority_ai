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

void yolo_callback(std_msgs::String::ConstPtr &msg);
void blindy_callback(std_msgs::String::ConstPtr &msg);


#endif /* ifndef ROSTOPIC_HPP */
