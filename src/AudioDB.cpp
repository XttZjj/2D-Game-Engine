#include "AudioDB.h"
#include "AudioHelper.h"
#include <iostream>
#include <filesystem>

AudioDB::AudioDB() {}


void AudioDB::LoadAudioFromDirectory(const std::string& directoryPath) {
    if (std::filesystem::exists(directoryPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".wav" || entry.path().extension() == ".ogg")) {
                Mix_Chunk* chunk = AudioHelper::Mix_LoadWAV498(entry.path().string().c_str());
                std::string fileName = entry.path().filename().stem().string();
                audio[fileName] = chunk;
            }
        }
    }
}

Mix_Chunk* AudioDB::GetAudio(const std::string& name) {
    auto it = audio.find(name);
    if (it != audio.end()) {
        return it->second;
    }
    // Handle error if audio not found
    std::cout << "error: failed to play audio clip " << name;
    exit(0);
    return nullptr;
}

void AudioDB::RenderAudio(const int channel, const std::string& name, const bool loop) {
    Mix_Chunk* chunk = GetAudio(name);
    if (loop)
        AudioHelper::Mix_PlayChannel498(channel, chunk, -1);
    else
        AudioHelper::Mix_PlayChannel498(channel, chunk, 0);
}

void AudioDB::Halt(const int channel) {
    AudioHelper::Mix_HaltChannel498(channel);
}

void AudioDB::SetVolume(const int channel, const int volume) {
    AudioHelper::Mix_Volume498(channel, volume);
}
