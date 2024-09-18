/*
    2 - Playlists:
		+ List of playlist
		+ View a playlist
		+ Create/Update/Delete a playlist
*/
#include <iostream>
#include <algorithm>
#include "PlaylistManager.h"

PlaylistManager::PlaylistManager(const std::string &name)
    : name{name}{}

PlaylistManager::~PlaylistManager() {
    waitForCompletion();
}

void PlaylistManager::waitForCompletion() {
    if (playlistThread.joinable())
        playlistThread.join();
}

void PlaylistManager::add_file_asyn(const std::string& file_name){
    waitForCompletion();
    playlistThread = std::thread(&PlaylistManager::add_file, this, file_name);
}
    
void PlaylistManager::remove_file_asyn(const std::string& file_name){
    waitForCompletion();
    playlistThread = std::thread(&PlaylistManager::remove_file, this, file_name);
}

void PlaylistManager::show_playlist_asyn(){
    waitForCompletion();
    playlistThread = std::thread(&PlaylistManager::show_playlist, this);
}

void PlaylistManager::clear_playlist_asyn(){
    waitForCompletion();
    playlistThread = std::thread(&PlaylistManager::clear_playlist, this);
}


void PlaylistManager::add_file(const std::string& file_name){
    // Check if the file is already in the playlist
    if (std::find(files.begin(), files.end(), file_name) == files.end()) {
        files.push_back(file_name);
        std::cout << "Added: " << file_name << std::endl;
    } 
    else {
        std::cout << "File already in playlist: " << file_name << std::endl;
    }
}

void PlaylistManager::remove_file(const std::string& file_name){
    // Check if the file is not in the playlist
    auto it = std::find(files.begin(), files.end(), file_name);
    if (std::find(files.begin(), files.end(), file_name) != files.end()) {
        files.erase(it);
        std::cout << "Removed: " << file_name << std::endl;
    } else {
        std::cout << "File not found in playlist: " << file_name << std::endl;
    }
}

void PlaylistManager::show_playlist() const{
    if (files.empty()) {
        std::cout << "Playlist is empty." << std::endl;
    } else {
        std::cout << "Current Playlist:" << std::endl;
        for (const auto& file : files) {
            std::cout << file << std::endl;
        }
    }
}

void PlaylistManager::clear_playlist(){
    files.clear();
    std::cout << "Playlist cleared." << std::endl;
}

std::string PlaylistManager::get_name() const{
    return name;
}

std::vector<std::string> PlaylistManager::get_files() const{
    return files;
}