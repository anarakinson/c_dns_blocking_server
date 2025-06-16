// dns_config

#include <dns_config.h>


void load_config_json(const char *filename, Config *config) {
    // open file
    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    // handle parsing error
    if (!root) {
        fprintf(
            stderr, "[!] Error parsing JSON: %s \nline %d, column %d\n", 
            error.text, error.line, error.column
        );
        exit(EXIT_FAILURE);
    }

    // parse json params
    json_t *listen_ip = json_object_get(root, "listen_ip");
    json_t *listen_port = json_object_get(root, "listen_port");
    json_t *upstream_dns_ip = json_object_get(root, "upstream_dns_ip");
    json_t *upstream_dns_port = json_object_get(root, "upstream_dns_port");
    json_t *block_response = json_object_get(root, "block_response");
    json_t *block_ip = json_object_get(root, "block_ip");
    json_t *blacklist = json_object_get(root, "blacklist");
    
    // setting up Config fields with type-checking and default values
    snprintf(
        config->listen_ip, sizeof(config->listen_ip), "%s",
        json_is_string(listen_ip) ? json_string_value(listen_ip) : "0.0.0.0"
    );
    config->listen_port = json_is_integer(listen_port) ? json_integer_value(listen_port) : 53;

    snprintf(
        config->upstream_dns_ip, sizeof(config->upstream_dns_ip), "%s",
        json_is_string(upstream_dns_ip) ? json_string_value(upstream_dns_ip) : "8.8.8.8"
    );
    config->upstream_dns_port = json_is_integer(upstream_dns_port) ? json_integer_value(upstream_dns_port) : 53;

    snprintf(
        config->block_response, sizeof(config->block_response), "%s",
        json_is_string(block_response) ? json_string_value(block_response) : "not found"
    );

    snprintf(
        config->block_ip, sizeof(config->block_ip), "%s",
        json_is_string(block_ip) ? json_string_value(block_ip) : "0.0.0.0"
    );

    // parse blacklist IP-s
    config->blacklist_count = 0;
    if (json_is_array(blacklist)) {
        size_t index;
        json_t *value;

        json_array_foreach(blacklist, index, value) {
            if (index > MAX_DOMAINS) {
                fprintf(stderr, "[!] Warning: blacklist exceeds maximum size: %d", MAX_DOMAINS);
                break;
            }

            if (json_is_string(value)) {
                snprintf(
                    config->blacklist[config->blacklist_count],
                    sizeof(config->blacklist[0]),
                    "%s",
                    json_string_value(value)
                );
                config->blacklist_count++;
            }
        }
    }

    // close file
    json_decref(root);
}
