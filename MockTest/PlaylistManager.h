#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

#include <string>
#include <thread>
#include <vector>

class PlaylistManager {
private:
    std::string name;
    std::vector<std::string> files;

    void add_file(const std::string& file_name);
    void remove_file(const std::string& file_name);
    void show_playlist() const;
    void clear_playlist();
    
    std::thread playlistThread;
public:
    PlaylistManager(const std::string&);
    ~PlaylistManager();
    void waitForCompletion();

    // Delete copy operations
    PlaylistManager(const PlaylistManager &obj) = delete;
    PlaylistManager& operator= (const PlaylistManager &obj) = delete;

    // Move operations as default
    PlaylistManager(PlaylistManager &&obj) = default;
    PlaylistManager& operator= (PlaylistManager &&obj) = default;

    // Helper functions
    void add_file_asyn(const std::string& file_name);
    void remove_file_asyn(const std::string& file_name);
    void show_playlist_asyn();
    void clear_playlist_asyn();

    std::string get_name() const;
    std::vector<std::string> get_files() const;
};

#endif
