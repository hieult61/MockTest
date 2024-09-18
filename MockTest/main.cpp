#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <mutex>
#include <algorithm>
#include "FileBrowser.h"
#include "PlaylistManager.h"
#include "Playlists.h"
#include "AudioPlayer.h"
#include "MetaData.h"

namespace fs = std::filesystem;

// Global shared data
std::vector<std::string> media_files;     // List of all media files
std::vector<PlaylistManager> playlists;   // List of all playlists

std::mutex media_files_mutex;
std::mutex playlists_mutex;

void showMenu() {
    std::cout << "\nCommands:\n";
    std::cout << "1 - Change Directory\n";
    std::cout << "2 - View Current Directory\n";
    std::cout << "3 - Edit Playlists\n";
    std::cout << "4 - Edit Metadata\n";
    std::cout << "5 - Play Musics\n";
    std::cout << "q - Quit\n";
    std::cout << "Enter your choice: ";
}

void showEditPlaylistMenu() {
    std::cout << "\nEdit Playlist Menu:\n";
    std::cout << "1 - Create Playlist\n";
    std::cout << "2 - Add File to Playlist\n";
    std::cout << "3 - Remove File from Playlist\n";
    std::cout << "4 - Show Playlist\n";
    std::cout << "5 - Remove Playlist\n";
    std::cout << "q - Quit\n";
    std::cout << "Enter your choice: ";
}

void showMetaDataMenu() {
    std::cout << "\nEdit Metadata Menu:\n";
    std::cout << "1 - Show Metadata of playlist\n";
    std::cout << "2 - Edit Metadata of file\n";
    std::cout << "3 - Add Custom Metadata to file\n";
    std::cout << "q - Quit\n";
    std::cout << "Enter your choice: ";
}

int main(int argc, char *argv[]) {
    FileBrowser fileBrowser;
    AudioPlayer audioPlayer;
    MetaData metaData;

    // Get current path
    fs::path currentPath = fs::current_path();
    std::string currentDirectory = currentPath.string();

    bool running = true;

    // Initial file listing
    // fileBrowser.listFilesAsync(currentDirectory);
    // fileBrowser.waitForCompletion();

    while (running) {
        showMenu();
        char choice;
        std::cin >> choice;

        switch (choice) {
            case '1': {
                std::string newDirectory;
                std::cout << "Enter new directory path: ";
                std::cin >> newDirectory;
                currentDirectory = newDirectory;
            }
            case '2': {
                fileBrowser.listFilesAsync(currentDirectory);
                fileBrowser.waitForCompletion();
                break;
            }
            case '3': {
                bool editing = true;
                while (editing){
                    showEditPlaylistMenu();
                    char editChoice;
                    std::cin >> editChoice;

                    switch (editChoice) {
                        case '1': {
                            std::string playlistName;
                            std::cout << "Enter playlist name to create: ";
                            std::cin >> playlistName;
                            create_playlist(playlistName);
                            break;
                        }
                        case '2': {
                            if (playlists.size() == 0)
                                std::cout << "Collection empty\n";
                            else{
                                std::string playlistName, filePath;
                                std::cout << "Enter playlist name: ";
                                std::cin >> playlistName;
                                std::cout << "Enter file path to add: ";
                                std::cin >> filePath;
                                add_file_to_playlist(playlistName, filePath);
                            }
                            break;
                        }
                        case '3': {
                            if (playlists.size() == 0)
                                std::cout << "Collection empty\n";
                            else{
                                std::string playlistName, filePath;
                                std::cout << "Enter playlist name: ";
                                std::cin >> playlistName;
                                std::cout << "Enter file path to remove: ";
                                std::cin >> filePath;
                                remove_file_from_playlist(playlistName, filePath);
                            }
                            break;
                        }
                        case '4': {
                            if (playlists.size() == 0)
                                std::cout << "Collection empty\n";
                            else{
                                std::string playlistName;
                                std::cout << "Enter playlist name to show: ";
                                std::cin >> playlistName;
                                show_playlist(playlistName);
                            }
                            break;
                        }
                        case '5': {
                            if (playlists.size() == 0)
                                std::cout << "Collection empty\n";
                            else{
                                std::string playlistName;
                                std::cout << "Enter playlist name to remove: ";
                                std::cin >> playlistName;
                                clear_playlist(playlistName);
                            }
                            break;
                        }
                        case 'q': {
                            editing = false;
                            break;
                        }
                        default: {
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
                            std::cout << "Invalid choice, please try again.\n";
                            break;
                        }
                    }
                }
                break;
            }
            case '4': {
                bool choice = true;
                while (choice){
                    showMetaDataMenu();
                    char editChoice;
                    std::cin >> editChoice;

                    switch (editChoice) {
                        case '1': {
                            if (playlists.size() == 0)
                                std::cout << "Collection empty\n";
                            else{
                                std::string playlistName;
                                std::cout << "Enter playlist name: ";
                                std::cin >> playlistName;

                                // Look for playlist in the collection
                                {
                                    std::lock_guard<std::mutex> lck (playlists_mutex);
                                    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == playlistName; });
                                    if (it != playlists.end())
                                        metaData.show_metadata_async(it->get_files());
                                    else 
                                        std::cout << "No playlist found in collection.\n";
                                }
                                metaData.waitForCompletion();
                            }
                            break;
                        }
                        case '2': {
                            std::cout << "Editing title, artist and album. Press Enter to keep as original\n";
                            std::string file_path, new_title, new_artist, new_album;
                            std::cout << "Enter file path to edit: ";
                            std::cin >> file_path;
                            std::cin.ignore();
                            std::cout << "Enter new title: ";
                            std::getline(std::cin, new_title);  // Allows empty string
                            std::cout << "Enter new artist: ";
                            std::getline(std::cin, new_artist);
                            std::cout << "Enter new album: ";
                            std::getline(std::cin, new_album);
                            metaData.edit_metadata(file_path, new_title, new_artist, new_album);
                            break;
                        }
                        case '3': {
                            std::cout << "The only available metadata keys are TITLE, ARTIST, ALBUM, YEAR, GENRE, TRACKNUMBER and PUBLISHER\n";
                            std::string file_path, key, value;
                            std::cout << "Enter file path to to add custom metadata: ";
                            std::cin >> file_path;
                            std::cout << "Enter metadata key: ";
                            std::cin >> key;
                            std::cin.ignore();
                            std::cout << "Enter value for " << key << ": ";
                            std::getline(std::cin, value);
                            metaData.add_custom_metadata(file_path, key, value);
                            break;
                        }                     
                        case 'q': {
                            choice = false;
                            break;
                        }
                        default: {
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear input buffer
                            std::cout << "Invalid choice, please try again.\n";
                            break;
                        }
                    }
                }
                break;
            }
            case '5': {
                if (playlists.size() == 0)
                    std::cout << "Collection empty\n";
                else{
                    std::string playlistName;
                    std::cout << "Enter playlist name: ";
                    std::cin >> playlistName;

                    // Look for playlist in the collection
                    std::lock_guard<std::mutex> lck (playlists_mutex);
                    auto it = std::find_if(playlists.begin(), playlists.end(), [&](const PlaylistManager& p) { return p.get_name() == playlistName; });
                    if (it != playlists.end())
                        audioPlayer.playPlaylist(it->get_files());
                    else 
                        std::cout << "No playlist found in collection.\n";
                }
                break;
            }
            case 'q':
                running = false;
                fileBrowser.waitForCompletion(); 
                metaData.waitForCompletion();
                audioPlayer.stop(); 
                break;
            default:
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cout << "Invalid choice, please try again.\n";
                break;
        }
    }
    return 0;
}
