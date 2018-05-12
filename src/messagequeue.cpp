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
   while (parent->_running) {
      /* Loc mutex before accesing data */
      /* Coppy the first elements text in the queue vector to a string */
      if(parent->size() > 0){
         parent->message_queue_lock.lock();
         srv.request.str = parent->pop();
         parent->message_queue_lock.unlock();

         if (service->call(srv)) {
            ROS_INFO_STREAM("Response: " << srv.response.str);
         }
      }
      usleep(1000);
   }
}

MessageQueue::MessageQueue(ros::ServiceClient *service){
   this->_running = true;
   this->message_queue_lock.lock();
   //std::thread(this->thread_say_queue,service, this);
   this->thread_handle = new std::thread(this->thread_say_queue,service,this);
   this->message_queue_lock.unlock();
}

void MessageQueue::push(std::string message, int priority){
   ROS_INFO_STREAM("Adding message " << message << ", with priority " << priority);
   /* Creating the new message struct */
   message_struct newmessage;
   newmessage.message=message;
   newmessage.priority=priority;
   /* Adding it to the queue require that the mutex locks */
   _queue.push_back(newmessage);

   //std::sort(_queue.begin(), _queue.end());
   /* unloc the mutex now when the queue is sorted */
}

std::string MessageQueue::pop(){
   std::string retvalue = _queue.at(0).message;
   _queue.erase(_queue.begin());
   return retvalue;
}

void MessageQueue::sort(){
   this->message_queue_lock.lock();
   std::sort(_queue.begin(), _queue.end());
   this->message_queue_lock.unlock();
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




