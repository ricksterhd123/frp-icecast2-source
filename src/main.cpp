#include <string>
#include <iostream>
#include <thread>
#include <streamer.h>

using namespace std;

int main(int argc, char **argv) {
    if (argc <= 1) {
        cout << "You must provide a path to file!\n" << endl;
        cout << "Usage: " << argv[0] << " [FILE]\n" << endl;
        exit(EXIT_FAILURE); 
    }

    try {
        Streamer streamer("127.0.0.1", 8000, "source", "At3tQH9K4pKoDt", "/stream");
        streamer.open();
        cout << "Opened connection to icecast" << endl;

        thread t([&](Streamer* streamer, const char* filename) {
            streamer->send_file(filename);
        }, &streamer, argv[1]);
        t.detach();

        while (!t.joinable()) {
            cout << "Hello world" << endl;
        }

        t.join();

        streamer.close();
        cout << "Closed connection" << endl;
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
        cout << "ending" << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
