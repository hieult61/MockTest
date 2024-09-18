/*
    1 - View list of media files in that folder and all sub-folder 
        + Pagination, 25 per page
*/ 

#include "FileBrowser.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>

extern std::mutex media_files_mutex;
extern std::vector<std::string> media_files;
namespace fs = std::filesystem;
const int page_size = 25;

FileBrowser::FileBrowser(){}

FileBrowser::~FileBrowser() {
    waitForCompletion();
}

void FileBrowser::listFilesAsync(const std::string& directory) {
    waitForCompletion();
    fileBrowserThread = std::thread(&FileBrowser::listFiles, this, directory);
}

void FileBrowser::waitForCompletion() {
    if (fileBrowserThread.joinable())
        fileBrowserThread.join();
}

// Function to check if a file is a media file (audio or video)
bool is_media_file(const std::string& path) {
    const std::vector<std::string> extensions = { ".mp3", ".wav", ".flac", ".mp4", ".avi", ".mkv", ".mov" };
    std::string ext = fs::path(path).extension().string();
    return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}

// Function to get all media files in the folder and sub-folders
std::vector<std::string> get_media_files(const std::string& directory) {
    std::vector<std::string> media_files;
    for (const auto& entry: fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && is_media_file(entry.path().string())) {
            media_files.push_back(entry.path().string());
        }
    }
    return media_files;
}

// Function to display files with pagination
void show_page(const std::vector<std::string>& media_files, int page_number, int page_size) {
    int start_index = (page_number - 1) * page_size;

    if (start_index >= media_files.size()) {
        std::cout << "No more files to display." << std::endl;
        return;
    }

    int end_index = std::min(start_index + page_size, (int)media_files.size());
    for (int i = start_index; i < end_index; ++i) {
        std::cout << i + 1 << ". " << media_files[i] << std::endl;
    }
}

void FileBrowser::listFiles(const std::string& directory) {
    char choice;
    int current_page = 1;
    std::vector<std::string> list_media_files;
    
    {
        std::lock_guard<std::mutex> lck(media_files_mutex);
        media_files = get_media_files(directory);
        list_media_files = media_files; // Copy shared resource
    }
    
    do {
        std::cout << "\nDisplaying page " << current_page << "/" << (list_media_files.size() + page_size - 1) / page_size << std::endl;
        show_page(list_media_files, current_page, page_size);
        
        std::cout << "\nEnter 'n' for next page, 'p' for previous page, 'q' to quit: ";
        std::cin >> choice;

        if (choice == 'n' && current_page * page_size < list_media_files.size()) {
            ++current_page;
        } else if (choice == 'p' && current_page > 1) {
            --current_page;
        }
    } while (choice != 'q');
}
