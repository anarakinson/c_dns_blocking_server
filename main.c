
#include <dns_packet.h>
#include <dns_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


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
        if (i >= 5) break; 
        printf("%zu. %s\n", i, config.blacklist[i]);
    }

    // create a socket file descriptor (use datagram/DGRAM for udp)
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd <= 0) {
        perror("[!] ERROR: socket creation failed");
        exit(EXIT_FAILURE);
    }

    // client and server addresses
    struct sockaddr_in serv_addr, cli_addr;
    // fill with zeros
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&serv_addr, 0, sizeof(serv_addr)); 
    memset(&cli_addr, 0, sizeof(cli_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(config.listen_ip);
    serv_addr.sin_port = htons(config.listen_port);

    // bind socket with address
    if (bind(socket_fd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[!] Error: bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[+] Server is ready\n");

    // buffers for messages
    unsigned char buffer[BUFFER_SIZE];
    unsigned char response[BUFFER_SIZE];
    // buffer for extracting domain
    unsigned char domain[256];

    while (1) {
        ssize_t valread = recvfrom(
            socket_fd, 
            buffer, BUFFER_SIZE, 0,
            (struct sockaddr*)&cli_addr, &(cli_addr_len)
        );
        if (valread < 0) {
            perror("[!] Error: receiving message failed");
            // skip iteration
            continue;
        }

        if (!extract_domain(buffer, domain)) {
            printf("[!] Error: failed to extract domain from query\n");
            continue;
        }
        
        printf("[+] Received query for: %s\n", domain);
        
        if (is_blocked(&config, domain)) {
            printf("[!] Domain in blacklist: %s\n", domain);
        }
        else {
            printf("[+] Address is allowed: %s\n", domain);
        }

        if (sendto(
            socket_fd, 
            response, sizeof(response), 0, 
            (struct sockaddr*)&cli_addr, cli_addr_len)
        ) {
            perror("[!] Error: failed to send response");
        }

    }

    close(socket_fd);

    return 0;
}