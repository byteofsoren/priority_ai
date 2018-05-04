#include <sstream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <SFML/Audio.hpp>
#include "sound.hpp"
#include <unistd.h>
#include <future>
#include <stdio.h>
#include <math.h>


void _cleaner(std::vector<sf::Sound*> *sounds);

double sind(double angle)
{
    double angleradians = angle * M_PI / 180.0f;
    return sin(angleradians) * M_PI / 180.0f;
}

double cosd(double angle)
{
    double angleradians = angle * M_PI / 180.0f;
    return cos(angleradians) * M_PI / 180.0f;
}


Sound::Sound(){
   _cleaner_handle = new std::thread(_cleaner,&_sounds);
}
Sound::~Sound(){
   _cleaner_handle->join();
}


int Sound::add_sound(std::string path){
   /* This function adds a path to the sound buffer vector thus
    * the sound can be played by play_sound() method.
    * Input:
    *    Path to the file.
    * Returns:
    *    the nuber of the added file for future referese in the palay_sound()
    *    method
    */
   std::cout << "Adding path=" << path <<std::endl;
   //sound_ptr.push_back(new sf::Sound);
   buffers.push_back(new sf::SoundBuffer);
   int element = buffers.size() - 1;
   if (buffers[element]->loadFromFile(path)) {
      files.push_back(path);
   }
   //int nr_of_buffers = sound_ptr.size()-1;
   // Loading the filepath to the buffer.
   return element;
}

int Sound::play_sound(unsigned long sound_nr, float angle, float distance){
   float xf = _multippler*distance*cosd(angle);
   float yf = _multippler*distance*sind(angle);
   sf::Sound *s = new sf::Sound;
   s->setBuffer(*buffers[sound_nr]);
   s->setPosition(xf,yf,0);
   s->play();
   _sounds.push_back(s);
   return 0;
}

void _cleaner(std::vector<sf::Sound*> *sounds){
   while (true){
      if (sounds->size() > 0) {
         sf::Sound *s = (sounds->at(0));
         ROS_INFO("_Cleaner");
         if (s->getStatus() != sf::Sound::Playing){
               ROS_INFO("remove sounds");
               s->resetBuffer();
               s->~Sound();
               sounds->erase(sounds->begin());
         }
      }
      usleep(1000);
   }
}

void Sound::list_sounds(){
   for (std::size_t i = 0; i < files.size(); ++i) {
      std::cout << "Path[" << i << "]=" << files[i] << std::endl;
   }
}
void Sound::setMultippler(float value)
{
   _multippler = value;
}
