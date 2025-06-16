
#include <stdio.h>
#include <string.h>

#include <server.h>
#include <dns_config.h>

#define CONFIG_PATH "./config/config.json"
// compile with -ljansson


int main(int argc, char** argv) {

    // parse config file
    const char *config_path = CONFIG_PATH;
    if (argc > 1) {
        config_path = argv[1];
    }

    Config config;
    memset(&config, 0, sizeof(config)); // set all fields with zeros
    load_config_json(config_path, &config);

    // print configs
    printf("listen %s:%d\n", config.listen_ip, config.listen_port);
    printf("upstream server %s:%d\n", config.upstream_dns_ip, config.upstream_dns_port);
    printf("blacklist size: %d\n", config.blacklist_count);
    for (size_t i = 0; i < config.blacklist_count; ++i) {
        printf("%zu. %s\n", i, config.blacklist[i]);
        if (i > 5) break; 
    }


    int server_fd;

    return 0;
}