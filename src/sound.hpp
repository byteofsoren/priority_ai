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
   //sf::SoundBuffer buffer;
   //std::vector<sf::Sound*> sound_ptr;
   std::vector<sf::SoundBuffer*> buffers;
   std::vector<std::string> files;
   //std::thread playing_sounds;
   std::vector<std::future<void>> playing_sounds;
   float _multippler = 60;
   //void thread_sound_play(size_t sound_nr, float angle, float distance);

public:
   Sound();
   virtual ~Sound();
   int add_sound(std::string path);    // Returs a sound number.
   int play_sound(unsigned long sound_nr, float angle, float distance);
   //int play_sound(tplay *tsound);
   void list_sounds();
   void setMultippler(float value);
};

#endif /* SOUND_H */
