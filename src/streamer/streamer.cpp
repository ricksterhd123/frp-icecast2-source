#include "streamer.h"

Streamer::Streamer(
    string hostname,
    unsigned short port,
    string user,
    string password,
    string mount)
{
    this->hostname = hostname;
    this->port = port;
    this->password = password;
    this->mount = mount;

    shout_init();

    this->instance = shout_new();

    if (this->instance == NULL)
    {
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

    if (result != SHOUTERR_SUCCESS)
    {
        shout_free(instance);
        shout_shutdown();
        throw runtime_error("libshout failed to initialize");
    }
}

Streamer::~Streamer()
{
    // Stop detached thread
    // Cleanup shout
    if (this->instance != NULL)
    {
        shout_close(this->instance);
        shout_free(this->instance);
        shout_shutdown();
    }
}

void Streamer::open()
{
    if (this->instance == NULL)
    {
        throw runtime_error("Streamer must be initialized");
    }

    if (shout_open(this->instance) != SHOUTERR_SUCCESS)
    {
        shout_free(this->instance);
        shout_shutdown();
        throw runtime_error("libshout failed to open");
    }
}

void Streamer::close()
{
    // Stop detached thread
    // Cleanup shout
    if (this->instance != NULL)
    {
        shout_close(this->instance);
        shout_free(this->instance);
        shout_shutdown();
    }
}

void Streamer::send_file(string filename)
{
    std::ifstream file(filename);

    while (!file.eof())
    {
        // Read contents into buffer
        unsigned char buffer[this->buffer_size];
        file.read((char *)buffer, sizeof(buffer));

        // Stream buffer to icecast
        if (shout_send(this->instance, buffer, file.gcount()) != SHOUTERR_SUCCESS) {
            // TODO: Check if this is actually needed?
            shout_close(this->instance);
            shout_free(this->instance);
            shout_shutdown();

            throw runtime_error("libshout failed to send data to icecast server");
        }

        // Sync with icecast
        shout_sync(this->instance);
    }
}
