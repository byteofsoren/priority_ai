#ifndef SOUND_H
#define SOUND_H

#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>



class Sound
{
private:
   std::vector<sf::SoundBuffer*> buffers;
   std::vector<sf::Sound*> _sounds;
   std::vector<std::string> files;
   //std::vector<std::future<void>> playing_sounds;
   std::vector<std::string> _labels;
   std::thread *_cleaner_handle;
   float _multippler = 60;

public:
   Sound();
   virtual ~Sound();
   int add_sound(std::string path);    // Returs a sound number.
   void add_sound(std::string path, std::string label);
   int play_sound(unsigned long sound_nr, float angle, float distance);
   void play_sound(std::string label, float angle, float distance);
   void list_sounds();
   void setMultippler(float value);
};

#endif /* SOUND_H */
