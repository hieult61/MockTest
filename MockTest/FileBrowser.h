#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <string>
#include <thread>

class FileBrowser {
private:
    // Member thread entry point
    void listFiles(const std::string& directory);
    std::thread fileBrowserThread;
public:
    FileBrowser();
    ~FileBrowser();

    void listFilesAsync(const std::string& directory);
    void waitForCompletion();

    // Delete copy operations
    FileBrowser(const FileBrowser &obj) = delete;
    FileBrowser& operator= (const FileBrowser &obj) = delete;

    // Move operations as default
    FileBrowser(FileBrowser &&obj) = default;
    FileBrowser& operator= (FileBrowser &&obj) = default;
};

#endif
