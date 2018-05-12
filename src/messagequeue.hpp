#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H
#include <iostream>
#include <vector>
#include <ros/ros.h>
#include <mutex>
#include <algorithm>
#include <thread>
#include <functional>
#include "robospeak/sayString.h"


typedef struct _message_struct{
   int priority;
   std::string message;
   bool operator<(_message_struct const &r) const {
      ROS_INFO("From sort operator");
      return priority > r.priority; }
} message_struct, *message_structPtr;

class MessageQueue
{
private:
   std::vector<message_struct> _queue;
   static void thread_say_queue(ros::ServiceClient *service, MessageQueue* parent);
   bool _running;
   std::thread *thread_handle;

public:
   MessageQueue(ros::ServiceClient *service);
   void push(std::string message, int priority);
   std::string pop();
   void sort();
   size_t size();
   std::mutex message_queue_lock;
   virtual ~MessageQueue();
};

#endif /* MESSAGE_QUEUE_H */
