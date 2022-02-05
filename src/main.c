#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <shout/shout.h>
#include <time.h>
#include <sqlite3.h>

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
	if (!setup_shout(instance, "127.0.0.1", 8000, "source", "At3tQH9K4pKoDt", "/stream"))
	{
		fprintf(stderr, "Failed to setup shout\n");
		shutdown(instance);
		return EXIT_FAILURE;
	}

	printf("Connected to server...\n");

	// Connect to server
	if (shout_open(instance) == SHOUTERR_SUCCESS)
	{
		unsigned char buff[4096];


		printf("Reading file %s\n", filePath);
		FILE *pFile = fopen(filePath, "r");

		if (pFile == NULL)
		{
			fprintf(stderr, "File error, could not open %s\n", filePath);
			return 0;
		}

		printf("Sending to icecast2\n");
		while (1)
		{
			size_t result = fread(buff, 1, sizeof(buff), pFile);

			if (result > 0) 
			{
				//printf("Sending %lu\n", result);
				int ret = shout_send(instance, buff, result);
				//printf("Response %d\n", ret);

				if (ret != SHOUTERR_SUCCESS)
				{
					fprintf(stderr, "Send error: %s", shout_get_error(instance));
					shout_close(instance);
					shutdown(instance);
					return EXIT_FAILURE;
				}
			} 
			else 
			{
				printf("Finished sending\n");
				continue;
			}

			shout_sync(instance);
		}

		printf("Closing connection\n");

		shout_close(instance);
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
