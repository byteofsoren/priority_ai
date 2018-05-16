#include "messagequeue.hpp"
#include "globals.h"



/* The first part of this file is a function that is executed as a thread.
 * The function uses the robottalk module to send text to the voice synthesiser.
 * One problem with that is wee need to pass a struct to the thread containing:
 *    pointer to robosay
 *    vector queue
 * The first one because we are using ros to send data to the handler.
 * And the vector queue handles the quin but remember to mutex loc because
 * std::vector isn't thread safe.
 */


void MessageQueue::thread_say_queue(ros::ServiceClient *service, MessageQueue* parent){
   robospeak::sayString srv;
   ROS_INFO("Init thread_say_queue");
//   while (parent->_running){
//      if (service->isPersistent())
//         break;
//      usleep(100);
//   }
   int counter = 0;
   while (parent->_running) {
      /* Loc mutex before accesing data */
      /* Coppy the first elements text in the queue vector to a string */
      ROS_INFO("MessageQueue::thread_say_queue Loc mutex in thread");
      parent->message_queue_lock.lock();
      usleep(10);
      if(parent->size() > 0){
         ROS_INFO("MessageQueue::thread_say_queue srv.request.str=parrent->pop();");
         srv.request.str = parent->pop(counter);
         ROS_INFO("MessageQueue::thread_say_queue srv.request.str=parrent->pop(); [done]");
         /* Send the string over to the robottalker */
         if (service->call(srv)) {
            ROS_INFO_STREAM("Response: " << srv.response.str);
         } else {
            ROS_INFO("Did not recive the cal");
         }
         /* Remove uninportant pairs of data based on _threashold */
         if (counter < parent->_threshold) {
            counter++;
         } else {
            counter = 0;
            ROS_INFO("MessageQueue::thread_say_queue parrent->clear_queue()");
            parent->clear_queue();
            ROS_INFO("MessageQueue::thread_say_queue parrent->clear_queue() [done]");
         }
      }
      ROS_INFO("MessageQueue::thread_say_queue Unlock mutex in thread");
      parent->message_queue_lock.unlock();
      usleep(100);
   }
}

MessageQueue::MessageQueue(ros::ServiceClient *service){
   this->_running = true;
   ROS_INFO("MessageQueue::MessageQueue mutex lock");
   this->message_queue_lock.lock();
   //std::thread(this->thread_say_queue,service, this);
   this->thread_handle = new std::thread(this->thread_say_queue,service,this);
   ROS_INFO("MessageQueue::MessageQueue mutex unlock");
   this->message_queue_lock.unlock();
   this->set_threshold(3);
}

void MessageQueue::set_threshold(int th) {
   this->_threshold=th;
}

void MessageQueue::push(std::string message, int priority){
   ROS_INFO_STREAM("Adding message " << message << ", with priority " << priority);
   /* Creating the new message struct */
   message_struct newmessage;
   //newmessage.message=message;
   newmessage.message= new std::string(message);
   newmessage.priority=priority;
   /* Adding it to the queue require that the mutex locks */
   ROS_INFO("MessageQueue::push push_back");
   _queue.push_back(newmessage);
   ROS_INFO("MessageQueue::push push_back done");

   //std::sort(_queue.begin(), _queue.end());
   /* unloc the mutex now when the queue is sorted */
}

std::string MessageQueue::pop(int place){
   ROS_INFO("MessageQueue::pop()");
   std::string retvalue = *_queue.at(place).message;
   ROS_INFO("MessageQueue::pop() _queue.erase");
   //_queue.erase(_queue.begin());
   ROS_INFO("MessageQueue::pop() _queue.erase done");
   return retvalue;
}

void MessageQueue::clear_queue() {
   this->_queue.clear();
}

void MessageQueue::sort(){
   ROS_INFO("MessageQueue::sort()");
   std::sort(_queue.begin(), _queue.end());
   ROS_INFO("MessageQueue::sort() end");
}

size_t MessageQueue::size(){
   return _queue.size();
}

MessageQueue::~MessageQueue(){
   this->_running = false;
   ROS_INFO("MessageQueue destruct stet _running=false");
   //this->thread_handle->join();
   ROS_INFO("MessageQueue godbye");
}




