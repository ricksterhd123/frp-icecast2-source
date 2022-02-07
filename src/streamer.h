#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <exception>
#include <thread>
#include <algorithm>

using namespace std;

extern "C" {
    #include <shout/shout.h>
}

/**
 * An enum that represents the possible states Streamer could be in
 */
enum STATE {
    UNINIT,
    WAITING,
    RUNNING,
    STOPPED,
    ERROR
};

class Streamer {
    private:
        // Streaming worker thread
        thread worker;

        // libshout instance
        shout_t* instance;
        
        // Icecast2 hostname
        string hostname;

        // Icecast2 port number
        unsigned short port;

        // Icecast2 source username
        string user;
        
        // Icecast2 source password
        string password;

        // Icecast2 source mount path
        string mount;

        // How many bytes should we send in each pulse?
        size_t bufferSize = 4096;

        // State of streamer object
        STATE status = UNINIT;

        // Did something go wrong?
        string errorMessage;

        // File queue
        vector<string> fileQueue;

        // Method for sending mp3
        void sendMP3(string filename);

    public:
        // Initialize
        Streamer(
            string hostname,
            unsigned short port,
            string user,
            string password,
            string mount
        );

        // Clean up, free memory
        ~Streamer();
        
        // Opens connection to icecast2 and starts streaming
        void open();

        void work();

        // Stop streaming and close connection to icecast2
        void close();

        // Get the state of the streamer
        STATE getStatus() { return this->status; }
        
        // How many files are on the queue?
        size_t getQueueSize() { return this->fileQueue.size(); }

        // Get file from top of queue
        string getFile() { return this->fileQueue.at(0); }

        // Push audio file to queue
        void addFile(string filename);
        
        // Remove audio file from queue
        void removeFile(string filename);

        // Pop audio file from queue
        void removeFile(int index = 0);
};
