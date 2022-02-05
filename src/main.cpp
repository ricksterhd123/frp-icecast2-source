#include <string>
#include <iostream>
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
        cout << "opened connection to icecast" << endl;
        streamer.close();
        cout << "close connection" << endl;
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
        cout << "ending" << endl;
    }
    return 0;
}
