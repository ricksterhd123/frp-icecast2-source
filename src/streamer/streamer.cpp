#include "streamer.h"

Streamer::Streamer(
    string hostname,
    unsigned short port,
    string user,
    string password,
    string mount
) {
    this->hostname = hostname;
    this->port = port;
    this->password = password;
    this->mount = mount;

    shout_init();

    this->instance = shout_new();

    if (this->instance == NULL) {
        shout_shutdown();
        throw runtime_error("libshout failed to allocate memory");
    }

    int result = shout_set_host(instance, hostname.c_str());
    result |= shout_set_protocol(instance, SHOUT_PROTOCOL_HTTP);
    result |= shout_set_port(instance, port);
    result |= shout_set_user(instance, user.c_str());
    result |= shout_set_password(instance, password.c_str());
    result |= shout_set_mount(instance, mount.c_str());
    result |= shout_set_format(instance, SHOUT_FORMAT_MP3);

    if (result != SHOUTERR_SUCCESS) {
        shout_free(instance);
        shout_shutdown();
        throw runtime_error("libshout failed to initialize");
    }
}

Streamer::~Streamer() {
    // Stop detached thread
    // Cleanup shout
    if (this->instance != NULL) {
        shout_close(this->instance);
        shout_free(this->instance);
        shout_shutdown();
    }
}

bool Streamer::open() {
    if (this->instance == NULL) {
        return false;
    }

    if (shout_open(this->instance) != SHOUTERR_SUCCESS) {
        shout_free(this->instance);
        shout_shutdown();
        throw runtime_error("libshout failed to open");
    }

    // Start detached thread that consumes file queue

    return true;
}

void Streamer::close() {
    // Stop detached thread
    // Cleanup shout
    if (this->instance != NULL) {    
        shout_close(this->instance);
        shout_free(this->instance);
        shout_shutdown();
    }
}

// void Streamer::push_file(string filename) {
//     this->file_queue.insert(this->file_queue.begin(), filename);
// }

// string Streamer::pop_file() {
//     // TODO: do these two operations in one step
//     string last = this->file_queue.at(this->file_queue.size() - 1);
//     this->file_queue.pop_back();
//     return last;
// }