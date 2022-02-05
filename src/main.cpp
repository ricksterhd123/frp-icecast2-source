#include <string>
#include <iostream>
#include <thread>
#include <streamer.h>
#include <sw/redis++/redis++.h>

using namespace sw::redis;
using namespace std;

// Name of channel that we subscribe to
const char* channel_name = "channel1";
// URI of redis server
const char* redis_uri = "tcp://127.0.0.1:6379";

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "You must provide a path to file!\n"
             << endl;
        cout << "Usage: " << argv[0] << " [FILE]\n"
             << endl;
        exit(EXIT_FAILURE);
    }

    try
    {
        // /**
        //  * @brief Subscribe to channel in redis that provides us with commands
        //  */

        // cout << "Connecting to redis" << endl;
        // auto redis = Redis(redis_uri);

        // // Create a Subscriber.
        // auto sub = redis.subscriber();

        // // Set callback functions.
        // sub.on_message(
        //     [](std::string channel, std::string msg){
        //         cout << channel << " : " << msg << endl;
        //     }
        // );

        // // Subscribe to channels and patterns.
        // sub.subscribe(channel_name);

        // // Consume messages in a loop.
        // while (true)
        // {
        //     try
        //     {
        //         sub.consume();
        //     }
        //     catch (const Error &err)
        //     {
        //         // Handle exceptions.
        //         cout << err.what() << endl;
        //     }
        // }

        Streamer streamer("127.0.0.1", 8000, "source", "At3tQH9K4pKoDt", "/stream");
        streamer.open();
        cout << "Opened connection to icecast" << endl;

        thread t([&](Streamer* streamer, const char* filename) {
            streamer->send_mp3(filename);
        }, &streamer, argv[1]);
        t.detach();

        streamer.close();
        cout << "Closed connection" << endl;
    }
    catch (const runtime_error &e)
    {
        cout << e.what() << endl;
        cout << "ending" << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
