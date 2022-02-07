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

    // Now Streamer is waiting for open() to be called
    this->status = WAITING;
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

    // Now Streamer is running
    this->status = RUNNING;

    // Once this thread has stopped, we need to signal back
    // to the main thread
    this->worker = thread(
        [](Streamer *streamer)
        {
            try
            {
                streamer->work();
            }
            catch (const runtime_error &e)
            {
                cout << e.what() << endl;
                terminate();
            }
        },
        this);

    this->worker.detach();
}

void Streamer::work()
{
    while (this->getStatus() == RUNNING)
    {
        if (this->getQueueSize() > 0)
        {
            string file = this->getFile();
            cout << file << endl;
            this->sendMP3(file);
            this->removeFile();
        }
        else
        {
            // do nothing until queue size has increased
            shout_sync(this->instance);
        }
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

// TODO: Do we know that filename actually exists?
void Streamer::sendMP3(string filename)
{
    std::ifstream file(filename);

    while (!file.eof())
    {
        // Read contents into buffer
        unsigned char buffer[this->bufferSize];
        file.read((char *)buffer, sizeof(buffer));

        // Stream buffer to icecast
        if (shout_send(this->instance, buffer, file.gcount()) != SHOUTERR_SUCCESS)
        {
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

void Streamer::addFile(string filename)
{
    if (find(this->fileQueue.begin(), this->fileQueue.end(), filename) == this->fileQueue.end())
    {
        this->fileQueue.push_back(filename);
    }
}

void Streamer::removeFile(string filename)
{
    vector<string>::iterator iter = find(this->fileQueue.begin(), this->fileQueue.end(), filename);
    if (iter != this->fileQueue.end())
    {
        this->fileQueue.erase(iter);
    }
}

void Streamer::removeFile(int index)
{
    this->fileQueue.erase(this->fileQueue.begin() + index);
}
