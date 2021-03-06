#include <string>
#include <iostream>
#include <thread>
#include <sw/redis++/redis++.h>
#include "streamer.h"

using namespace sw::redis;
using namespace std;

// Name of channel that we subscribe to
const char *channel_name = "channel1";
// URI of redis server
const char *redis_uri = "tcp://127.0.0.1:6379";

int main()
{
    try
    {
        cout << "Connecting to icecast2" << endl;
        Streamer streamer("127.0.0.1", 8000, "source", "At3tQH9K4pKoDt", "/stream");

        streamer.open();

        cout << "Connecting to redis" << endl;
        auto redis = Redis(redis_uri);

        // Create a Subscriber.
        auto sub = redis.subscriber();

        // Set callback functions.
        sub.on_message(
            [&streamer](string channel, string msg)
            {
                if (channel == channel_name)
                {
                    cout << streamer.getStatus() << endl;
                    streamer.addFile(msg);
                }
            }
        );

        // Subscribe to channel
        sub.subscribe(channel_name);

        // Consume messages in a loop.
        while (true)
        {
            try
            {
                sub.consume();
            }
            catch (const Error &err)
            {
                // Handle exceptions.
                cout << err.what() << endl;
                break;
            }
        }

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
