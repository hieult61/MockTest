/*
    4 - Play music from your application using SDL2 library on other thread.
		+ Play/Pause
		+ Next/Previous
		+ automatically move to the next song when current song finish
		+ Display current time and duration time
*/
/*
    5 - Can change volume of device
*/

#include "AudioPlayer.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <mutex>

#define MY_FONT "FreeSans.ttf"

namespace fs = std::filesystem;
extern std::mutex playlists_mutex;
bool quitMusicThread = false;

AudioPlayer::AudioPlayer() 
    : currentTrackIndex{0}, currentMusic{nullptr}, volume{64},
    window{nullptr}, renderer{nullptr}
{
    // Initialize SDL2 mixer and TTF
    if (Mix_Init(MIX_INIT_MP3) == 0 || TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_mixer or SDL_ttf: " << Mix_GetError() << std::endl;
        return;
    }

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_Mixer couldn't initialize. Error: " << Mix_GetError() << std::endl;
        return;
    }
    
    textColor = { 255, 255, 255, 255 }; // White color for text
}

AudioPlayer::~AudioPlayer() {
    stop();
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

void AudioPlayer::stop() {
    quitMusicThread = true;
    Mix_HaltMusic();
    if (audioThread.joinable()) {
        audioThread.join();
    }
    if (currentMusic != nullptr) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // Thread can be restarted later
    quitMusicThread = false;
}

// Render text on SDL window
void AudioPlayer::renderText(const std::string &text, int x, int y) {
    textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect dstrect = { x, y, textSurface->w, textSurface->h };
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &dstrect);
    SDL_DestroyTexture(textTexture);
}

// Function to load and play a music file
void AudioPlayer::playMusic(const std::string &filePath) {
    if (currentMusic != nullptr) {
        Mix_FreeMusic(currentMusic); // A song is being played. Stop it
    }

    currentMusic = Mix_LoadMUS(filePath.c_str());
    if (currentMusic == nullptr) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        return;
    }

    if (Mix_PlayMusic(currentMusic, 1) == -1) {  // Play once
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_VolumeMusic(volume);
    std::cout << "\nNow playing: " << filePath << std::endl;
}

void AudioPlayer::playPlaylist(const std::vector<std::string>& playlist) {

    // If the thread is already running, stop it before starting a new one
    stop();

    currentPlaylist = playlist;
    currentTrackIndex = 0;
    audioThread = std::thread(&AudioPlayer::playAudio, this); 
}

void AudioPlayer::nextTrack() {
    currentTrackIndex = (currentTrackIndex + 1) % currentPlaylist.size();
    playMusic(currentPlaylist[currentTrackIndex]);
}

void AudioPlayer::previousTrack() {
    currentTrackIndex = (currentTrackIndex - 1 + currentPlaylist.size()) % currentPlaylist.size();
    playMusic(currentPlaylist[currentTrackIndex]);
}

void AudioPlayer::playAudio() {
    // Create SDL window
    window = SDL_CreateWindow("Music Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 300, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Load a font for rendering text
    font = TTF_OpenFont(MY_FONT, 24); // Specify a valid font path

    // Start playing the first song
    playMusic(currentPlaylist[currentTrackIndex]);

    Uint32 startTicks = SDL_GetTicks(); // Track the start time of the current song
    SDL_Event e;
    Uint32 pauseTicks = 0;
    bool isPause = false;

    while (!quitMusicThread) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quitMusicThread = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:  // Increase volume
                        if (volume < 128) {
                            volume += 8;
                            Mix_VolumeMusic(volume);
                            std::cout << "Volume Up: " << volume << std::endl;
                        }
                        break;
                    case SDLK_DOWN:  // Decrease volume
                        if (volume > 0) {
                            volume -= 8;
                            Mix_VolumeMusic(volume);
                            std::cout << "Volume Down: " << volume << std::endl;
                        }
                        break;
                    case SDLK_RIGHT:  // Skip to next song
                        nextTrack();
                        startTicks = SDL_GetTicks(); // Reset song start time
                        break;
                    case SDLK_LEFT:  // Back to previous song
                        previousTrack();
                        startTicks = SDL_GetTicks();
                        break;
                    case SDLK_SPACE:  // Pause or unpause
                        if (isPause){
                            Mix_ResumeMusic();
                            startTicks = SDL_GetTicks() - pauseTicks;
                            isPause = false;
                        }
                        else{
                            Mix_PauseMusic();
                            pauseTicks = SDL_GetTicks() - startTicks;
                            isPause = true;
                        }
                        break;
                    case SDLK_q:  // Quit the music thread
                        quitMusicThread = true;
                        break;
                }
            }
        }
        
        if (quitMusicThread){     // Clean up
            Mix_HaltMusic();
            Mix_FreeMusic(currentMusic);
            currentMusic = nullptr;
            SDL_DestroyWindow(window);
            window = nullptr;
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        else{
            // Check if the song has finished
            if (!Mix_PlayingMusic() && !Mix_PausedMusic()) {
                nextTrack(); // Automatically play the next song
                startTicks = SDL_GetTicks(); 
            }

            // Calculate song duration and current time
            Uint32 currentTicks = (isPause ? pauseTicks : SDL_GetTicks() - startTicks);
            int secondsPlayed = currentTicks / 1000;
            int minutesPlayed = secondsPlayed / 60;
            secondsPlayed %= 60;
            
            //int durationSeconds = Mix_MusicDuration(currentMusic);
            //int durationMinutes = durationSeconds / 60;
            //durationSeconds %= 60;

            // Render text information
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
            SDL_RenderClear(renderer);   // Clear screen

            std::string songName = fs::path(currentPlaylist[currentTrackIndex]).filename().string();
            renderText("Now Playing: " + songName, 20, 20);

            // Configure current time played
            std::string currentTime = "Time: " + std::to_string(minutesPlayed) + ":" + (secondsPlayed < 10 ? "0" : "") + std::to_string(secondsPlayed);
            //std::string totalTime = " / " + std::to_string(durationMinutes) + ":" + (durationSeconds < 10 ? "0" : "") + std::to_string(durationSeconds);
            renderText(currentTime, 20, 60);

            SDL_RenderPresent(renderer); // Update the screen
        }
    }
}