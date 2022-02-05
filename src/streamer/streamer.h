#include <vector>
#include <iostream>
#include <exception>

using namespace std;

extern "C" {
    #include <shout/shout.h>
}

class Streamer {
    private:
        shout_t* instance;
        string hostname;
        unsigned short port;
        string user;
        string password;
        string mount;

        vector<string> file_queue;

        //string pop_file();
    public:
        Streamer(
            string hostname,
            unsigned short port,
            string user,
            string password,
            string mount
        );

        ~Streamer();

        bool open();
        void close();

        //void push_file(string filename);
};
