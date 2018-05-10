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
   bool operator<(_message_struct const &r) const { return priority > r.priority; }
} message_struct, *message_structPtr;

class MessageQueue
{
private:
   std::vector<message_structPtr> _queue;
   bool _compaire_queue(message_struct left, message_struct right);

public:
   MessageQueue(ros::NodeHandlePtr handle);
   void add_message(std::string message, int priority);
   int queue_size();
   virtual ~MessageQueue();
};

#endif /* MESSAGE_QUEUE_H */
