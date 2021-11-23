#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <shout/shout.h>
/**
 * @brief Setup libshout to connect to our icecast2 server
 * 
 * @param instance shout_t pointer returned from shout_new()
 * @param hostname hostname of the icecast2 server
 * @param port port of the icecast2 server
 * @param user source user
 * @param password source password
 * @param mount source mount point
 * @return int success?
 */
int setup_shout(shout_t *instance, const char *hostname, unsigned short port, const char *user, const char *password, const char *mount)
{
	int result = shout_set_host(instance, hostname) |
				 shout_set_protocol(instance, SHOUT_PROTOCOL_HTTP) |
				 shout_set_port(instance, port) |
				 shout_set_user(instance, user) |
				 shout_set_password(instance, password) |
				 shout_set_mount(instance, mount) |
				 shout_set_format(instance, SHOUT_FORMAT_MP3);

	return result == SHOUTERR_SUCCESS;
}

/**
 * @brief Clean up our mess before shutting down
 * @param instance shout_t pointer returned from shout_new()
 */
void shutdown(shout_t *instance)
{
	if (instance)
	{
		shout_free(instance);
	}

	shout_shutdown();
}

/**
 * @brief Reads the contents of a file into buffer, expects the user to free buffer 
 * when no longer being used.
 * @param filePath path to file to read into buffer
 * @param buffer pointer to buffer
 * @return int 
 */
int read_file(const char *filePath, char **buffer)
{
	FILE *pFile;
	long lSize;
	size_t result;

	pFile = fopen(filePath, "r");
	if (pFile == NULL)
	{
		fprintf(stderr, "File error, could not open %s\n", filePath);
		return 0;
	}

	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);
	printf("ftell gave us %ld\n", lSize);

	*buffer = (char *) malloc(sizeof(char) * lSize);
	if (*buffer == NULL)
	{
		fprintf(stderr, "Memory error\n");
		// TODO: panic
		return 0;
	}


	result = fread(*buffer, 1, lSize, pFile);

	if (result != lSize)
	{
		fprintf(stderr, "Reading error, size of buffer and size of file are not the same\n");
	}

	printf("here the size is %lu\n", strlen(*buffer));
	fclose(pFile);

	return 1;
}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		printf("You MUST provide a path to file!\n");
		printf("Usage: %s [FILE]\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *filePath = argv[1];

	shout_init();

	const char *version = shout_version(NULL, NULL, NULL);

	printf("Initialized libshout %s\n", version);

	shout_t *instance = shout_new();

	// Setup libshout to connect to our icecast server
	if (!setup_shout(instance, "127.0.0.1", 8000, "source", "hackme", "/stream"))
	{
		fprintf(stderr, "Failed to setup shout\n");
		shutdown(instance);
		return EXIT_FAILURE;
	}

	// Connect to server
	if (shout_open(instance) == SHOUTERR_SUCCESS)
	{
		printf("Connected to server...\n");
		// TODO: read a file into the buffer
		// TODO: send buffer contents into icecast via shout_send
		// TODO: clean up then finish

		printf("Reading file %s into buffer\n", filePath);

		char *buffer;
		if (!read_file(filePath, &buffer))
		{
			fprintf(stderr, "Could not read file into buffer\n");
			// shout_close(instance);
			// shutdown(instance);
			return EXIT_FAILURE;
		}

		printf("Buffer has %lu bytes\n", sizeof(buffer));

		printf("Syncing\n");
		shout_sync(instance);

		printf("Sending\n");
		int ret = shout_send(instance, buffer, sizeof(buffer));
		printf("Got %d\n", ret);

		if (ret != SHOUTERR_SUCCESS)
		{
			fprintf(stderr, "Send error: %s", shout_get_error(instance));
			free(buffer);
			shout_close(instance);
			shutdown(instance);
			return EXIT_FAILURE;
		}

		printf("Finished, no issues\n");
	}
	else
	{
		fprintf(stderr, "Failed to connect to server\n");
		shutdown(instance);
		return EXIT_FAILURE;
	}

	shutdown(instance);

	return EXIT_SUCCESS;
}
