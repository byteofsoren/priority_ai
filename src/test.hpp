#ifndef TEST_LIBARY
#define TEST_LIBARY

#include "sound.hpp"
#include "fuzzy_logic.hpp"
#include "ros/ros.h"
#include "std_msgs/String.h"

void test_ros_sub(ros::NodeHandlePtr &n);
void test_fuzzy_Class();
void test_sound();
void test_fuzzyLogic();

#endif /* ifndef TEST_LIBARY */
