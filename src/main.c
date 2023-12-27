#include <concord/discord.h>
#include <concord/log.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // Include for file handling
#include <time.h>

// Function to load environment variables from a file
void load_env_file() {
    FILE *env_file = fopen(".env", "r");
    if (env_file == NULL) {
        log_error(".env file not found.");
        exit(1);
    }

    char line[100]; // Adjust the buffer size as needed

    while (fgets(line, sizeof(line), env_file)) {
        char *name = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (name && value) {
            if (setenv(name, value, 1) != 0) {
                log_error("Failed to set environment variable.");
                exit(1);
            }
        }
    }

    fclose(env_file);
}

void on_ready(struct discord *client, const struct discord_ready *event) {
    log_info("Logged in as %s!", event->user->username);
}

void on_message(struct discord *client, const struct discord_message *event) {
    const char *command_prefix = getenv("COMMAND_PREFIX");
    if (command_prefix == NULL) {
        log_error("COMMAND_PREFIX environment variable is not set.");
        return;
    }

    if (strncmp(event->content, command_prefix, strlen(command_prefix)) == 0 &&
        strcmp(event->content + strlen(command_prefix), "ping") == 0) {
        
        // Record the time before sending the message
        clock_t start_time = clock();

        // Send a preliminary message to calculate round-trip time
        struct discord_create_message params = { .content = "Calculating ping..." };
        discord_create_message(client, event->channel_id, &params, NULL);

        // Record the time after the message is sent (acknowledged by Discord)
        clock_t end_time = clock();

        // Calculate the difference in milliseconds
        double latency_ms = ((double) (end_time - start_time)) * 1000.0 / CLOCKS_PER_SEC;

        // Prepare an embedded message with the latency
        char reply[256];
        snprintf(reply, sizeof(reply), "Pong! Latency: %.2f ms", latency_ms);
        params.content = reply;
        discord_create_message(client, event->channel_id, &params, NULL);
    }
}

int main(void) {
    // Load environment variables from .env file
    load_env_file();

    // Read the BOT_TOKEN from the environment variable
    const char *bot_token = getenv("BOT_TOKEN");
    if (bot_token == NULL) {
        log_error("BOT_TOKEN environment variable is not set.");
        return 1;
    }

    struct discord *client = discord_init(bot_token);
    if (client == NULL) {
        log_error("Failed to initialize Discord client.");
        return 1;
    }

    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);

    // Start the Discord bot
    if (discord_run(client) != 0) {
        log_error("Failed to start Discord bot.");
        discord_cleanup(client);
        return 1;
    }

    // Clean up resources
    discord_cleanup(client);

    return 0;
}
