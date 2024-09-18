#ifndef _PLAYLISTS_H
#define _PLAYLISTS_H

#include <string>
#include "PlaylistManager.h"

void create_playlist(const std::string& name);
void add_file_to_playlist(const std::string& playlist_name, const std::string& file_path);
void remove_file_from_playlist(const std::string& playlist_name, const std::string& file_path);
void show_playlist(const std::string& playlist_name);
void clear_playlist(const std::string& playlist_name);

#endif