/*
    3 - Metadata of a file (Taglib): [MAIN FEATURE]
		+ Available from either media list or a playlist
		+ Show metadata of a media file (pretty much anything you can think of as metadata): 
			Audio: Track name, Album, Artist, Duration, Genre, Publisher, Publish Year,...
			Video: Name, Size, Duration, Bitrate, Codec, 
		+ Edit metadata: change value, add new keys,...
*/

#include "MetaData.h"

MetaData::MetaData(){}

MetaData::~MetaData(){
    waitForCompletion();
}

void MetaData::waitForCompletion() {
    if (metadataThread.joinable()) {
        metadataThread.join();
    }
}

// Edit metadata of a specific file
void MetaData::edit_metadata(const std::string& file_path, const std::string& new_title, const std::string& new_artist, const std::string& new_album) {
    TagLib::FileRef file(file_path.c_str());
    if (!file.isNull()) {
        TagLib::Tag* tag = file.tag();
        if (new_title != "") 
            tag->setTitle(new_title);
        if (new_artist != "")
            tag->setArtist(new_artist);
        if (new_album != "")
            tag->setAlbum(new_album);
        file.save();  // Save changes to the file
        std::cout << "Metadata updated for file: " << file_path << std::endl;
    } else {
        std::cout << "Failed to edit metadata for file: " << file_path << std::endl;
    }
}

// Add or update a custom metadata field
void MetaData::add_custom_metadata(const std::string& file_path, const std::string& key, std::string& value) {
    TagLib::FileRef file(file_path.c_str());
    if (!file.isNull()) {
        TagLib::PropertyMap properties = file.file()->properties();
        properties.replace(key, (const TagLib::String) value);  // Add new or update the key-value pair
        file.file()->setProperties(properties);
        file.save();  // Save changes to the file
        std::cout << "Custom metadata added/updated for file: " << file_path << std::endl;
    } else {
        std::cout << "Failed to add custom metadata for file: " << file_path << std::endl;
    }
}

void MetaData::show_metadata_async(const std::vector<std::string> &playlist) {
    // Wait for any previous thread to finish
    waitForCompletion();
    metadataFiles = playlist;
    metadataThread = std::thread(&MetaData::show_metadata, this);
}

void MetaData::show_metadata() const {
    for (const auto& file : metadataFiles) {
        read_metadata_from_file(file);
    }
}

void MetaData::read_metadata_from_file(const std::string& file_path) const {
    TagLib::FileRef file(file_path.c_str());
    if (!file.isNull()) {
        TagLib::Tag* tag = file.tag();
        std::cout << "File: "   << file_path     << std::endl;
        std::cout << "Title: "  << tag->title()  << std::endl;
        std::cout << "Artist: " << tag->artist() << std::endl;
        std::cout << "Album: "  << tag->album()  << std::endl;
        std::cout << "Year: "   << tag->year()   << std::endl;
        std::cout << "Genre: "  << tag->genre()  << std::endl;
        std::cout << "Track: "  << tag->track()  << std::endl;
        std::cout << "---------------------------------------" << std::endl;
    } else {
        std::cout << "Failed to read metadata from: " << file_path << std::endl;
    }
}