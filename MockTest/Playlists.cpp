#include <iostream>
#include <algorithm>
#include <vector>
#include <mutex>
#include "Playlists.h"

extern std::vector<PlaylistManager> playlists;
extern std::mutex playlists_mutex;

// Create a new playlist
void create_playlist(const std::string& name) {
    std::lock_guard<std::mutex> lck (playlists_mutex);
    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == name; });
    if (it == playlists.end()) {
        playlists.push_back(PlaylistManager{name});
        std::cout << "Playlist '" << name << "' created." << std::endl;
    } else {
        std::cout << "Playlist '" << name << "' already exists." << std::endl;
    }
}

// Add a file to a specific playlist by name
void add_file_to_playlist(const std::string& playlist_name, const std::string& file_path) {
    std::lock_guard<std::mutex> lck (playlists_mutex);
    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == playlist_name; });
    if (it != playlists.end()) {
        it->add_file_asyn(file_path);
        it->waitForCompletion();
    } else {
        std::cout << "Playlist '" << playlist_name << "' not found." << std::endl;
    }
}

// Remove a file from a specific playlist by name
void remove_file_from_playlist(const std::string& playlist_name, const std::string& file_path) {
    std::lock_guard<std::mutex> lck (playlists_mutex);
    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == playlist_name; });
    if (it != playlists.end()) {
        it->remove_file_asyn(file_path);
        it->waitForCompletion();
    } else {
        std::cout << "Playlist '" << playlist_name << "' not found." << std::endl;
    }
}

// Function to show a playlist by name
void show_playlist(const std::string& playlist_name) {
    std::lock_guard<std::mutex> lck (playlists_mutex);
    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == playlist_name; });
    if (it != playlists.end()) {
        it->show_playlist_asyn();
        it->waitForCompletion();
    } else {
        std::cout << "Playlist '" << playlist_name << "' not found." << std::endl;
    }
}

void clear_playlist(const std::string& playlist_name){
    std::lock_guard<std::mutex> lck (playlists_mutex);
    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == playlist_name; });
    if (it != playlists.end()) {
        it->clear_playlist_asyn();
        it->waitForCompletion();
        playlists.erase(it);
    } else {
        std::cout << "Playlist '" << playlist_name << "' not found." << std::endl;
    }
}