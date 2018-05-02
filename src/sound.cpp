#include <sstream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <SFML/Audio.hpp>
#include "sound.hpp"
#include <unistd.h>
#include <future>
#include <stdio.h>
#include <math.h>

void thread_sound_play(sf::SoundBuffer *bf, int x, int y, int z)
{
   sf::Sound s;
   s.setBuffer(*bf);
   s.setPosition(x,y,z);
   usleep(1);
   s.play();
   while (s.getStatus() == sf::Sound::Playing){
      usleep(100);
   }
}

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
}
Sound::~Sound(){
   // Wait untill all threads are finiched before exiting
   //for (size_t i = 0; i < playing_sounds.size(); ++i) {
      //playing_sounds[i]->join();
   //}
   //sleep(4);
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
   // setPosition isnt done yet.
   std::cout << "playing sound_nr="<<sound_nr<<" at angle=" << angle << " distance=" << distance << std::endl;
   //playing_sounds.push_back(thread_sound_play(sound_nr,angle,distance))
   //playing_sounds.push_back(thread_sound_play(buffers[sound_nr]));;
   //vThreads.push_back(std::thread(&Crob::findPath, *i));
   //playing_sounds.push_back(std::async(thread_sound_play, 0,0,0));
   float xf = _multippler*distance*cosd(angle);
   float yf = _multippler*distance*sind(angle);
   std::cout << " play_sound x=" << xf << " y=" << yf << std::endl;
   playing_sounds.push_back(std::async(thread_sound_play, buffers[sound_nr], xf,yf,0));
   return 0;
}

void Sound::list_sounds(){
   for (std::size_t i = 0; i < files.size(); ++i) {
      std::cout << "Path[" << i << "]=" << files[i] << std::endl;
      //std::cout << "Loopend " << sound_buffers[i].getLoop() << std::endl;

   }
}
void Sound::setMultippler(float value)
{
   _multippler = value;
}
