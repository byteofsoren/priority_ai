#include "messagequeue.hpp"

std::mutex message_queue_lock;

/* The first part of this file is a function that is executed as a thread.
 * The function uses the robottalk module to send text to the voice synthesiser.
 * One problem with that is wee need to pass a struct to the thread containing:
 *    pointer to robosay
 *    vector queue
 * The first one because we are using ros to send data to the handler.
 * And the vector queue handles the quin but remember to mutex loc because
 * std::vector isn't thread safe.
 */

typedef struct _thread_com{
   ros::ServiceClient *roshandle;
   std::vector<message_structPtr> *queue;
   bool running;
} thread_com, *thread_comPtr;

void tread_say_queue(thread_comPtr connection){
   robospeak::sayString srv;
   while (connection->running) {
      /* Loc mutex before accesing data */
      message_queue_lock.lock();
      /* Coppy the first elements text in the queue vector to a string */
      message_struct local  =  (message_struct) connection->queue->pop_back();
      message_queue_lock.unlock();
      srv.request.str = "hello";
      message_queue_lock.unlock();
      if (connection->roshandle->call(srv)) {
         ROS_INFO_STREAM("Response: " << srv.response.str);
      }
      sleep(1);
   }
}

MessageQueue::MessageQueue(ros::NodeHandlePtr handle){
   handle->ok();
}

void MessageQueue::add_message(std::string message, int priority){
   ROS_INFO_STREAM("Adding message " << message << ", with priority " << priority);
   /* Creating the new message struct */
   message_structPtr newmessage = new message_struct;
   newmessage->message=message;
   newmessage->priority=priority;
   /* Adding it to the queue require that the mutex locks */
   message_queue_lock.lock();
   _queue.push_back(newmessage);
   /* Sort the queue */
   std::sort(_queue.begin(), _queue.end());
   /* unloc the mutex now when the queue is sorted */
   message_queue_lock.unlock();
}


