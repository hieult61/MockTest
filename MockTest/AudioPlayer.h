#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <thread>

class AudioPlayer {
private:
    void playAudio();
    std::thread audioThread;

    std::vector<std::string> currentPlaylist;
    int currentTrackIndex;
    Mix_Music* currentMusic;
    int volume;

    // For displaying
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font *font;
    SDL_Color textColor;
    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    SDL_Rect textRect;
    
public:
    AudioPlayer();
    ~AudioPlayer();
    void stop();

    // Delete copy operations
    AudioPlayer(const AudioPlayer &obj) = delete;
    AudioPlayer& operator= (const AudioPlayer &obj) = delete;

    // Move operations as default
    AudioPlayer(AudioPlayer &&obj) = default;
    AudioPlayer& operator= (AudioPlayer &&obj) = default;

    void playPlaylist(const std::vector<std::string>& playlist);
    void playMusic(const std::string &filePath);

    void nextTrack();
    void previousTrack();

    // Display helper functions
    void renderText(const std::string &text, int x, int y);
};

#endif
