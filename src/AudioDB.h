#ifndef AUDIODB_H
#define AUDIODB_H

#include <unordered_map>
#include <string>
#include "../SDL2_mixer/SDL_mixer.h"

class AudioDB {
public:
    AudioDB();

    void LoadAudioFromDirectory(const std::string& directoryPath);
    static Mix_Chunk* GetAudio(const std::string& name);
    static void RenderAudio(const int channel, const std::string& name, const bool loop);
    static void Halt(const int channel);
    static void SetVolume(const int channel, const int volume);

private:
    static inline std::unordered_map<std::string, Mix_Chunk*> audio;
};

#endif // AUDIODB_H
