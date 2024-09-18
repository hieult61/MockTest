#ifndef _METADATA_H
#define _METADATA_H

/*
    3 - Metadata of a file (Taglib): [MAIN FEATURE]
		+ Available from either media list or a playlist
		+ Show metadata of a media file (pretty much anything you can think of as metadata): 
			Audio: Track name, Album, Artist, Duration, Genre, Publisher, Publish Year,...
			Video: Name, Size, Duration, Bitrate, Codec, 
		+ Edit metadata: change value, add new keys,...
*/

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>

class MetaData {
private:
    std::thread metadataThread;  
    std::vector<std::string> metadataFiles;
    void read_metadata_from_file(const std::string& file_path) const;
    void show_metadata() const;

public:
    MetaData();
    ~MetaData();
    void waitForCompletion();

    // Delete copy operations
    MetaData(const MetaData &obj) = delete;
    MetaData& operator= (const MetaData &obj) = delete;

    // Move operations as default
    MetaData(MetaData &&obj) = default;
    MetaData& operator= (MetaData &&obj) = default;

    // Show metadata of a playlist
    void show_metadata_async(const std::vector<std::string> &playlist);

    // Edit metadata of a specific file
    void edit_metadata(const std::string& file_path, const std::string& new_title, const std::string& new_artist, const std::string& new_album);

    // Add or update a custom metadata field
    void add_custom_metadata(const std::string& file_path, const std::string& key, std::string& value);
};

#endif