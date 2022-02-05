#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <exception>

using namespace std;

extern "C" {
    #include <shout/shout.h>
}

class Streamer {
    private:
        shout_t* instance;
        thread* worker;

        string hostname;
        unsigned short port;
        string user;
        string password;
        string mount;
        size_t buffer_size = 4096;

        vector<string> file_queue;
        void send_mp3(string filename);

    public:
        Streamer(
            string hostname,
            unsigned short port,
            string user,
            string password,
            string mount
        );

        ~Streamer();

        void open();
        void close();
        bool isPlaying();
};
