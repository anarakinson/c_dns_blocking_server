#ifndef _DNS_CONFIG
#define _DNS_CONFIG

#include <stdio.h>
#include <stdlib.h>

#include <jansson.h>


#define MAX_DOMAINS 128


// struct for containing JSON data
typedef struct{
    char listen_ip[16]; // IP on which the server will run
    int listen_port; // port on which the server will run

    char upstream_dns_ip[16]; // IP and port of server where redirect to
    int upstream_dns_port; 
 
    char block_response[20]; // type of response for blocked domains (NXDOMAIN or REFUSED)
    char block_ip[16]; // IP for redirect blocked IP-s
    char blacklist[MAX_DOMAINS][256]; // blacklist of blocked IP-s
    int blacklist_count; // current size of blacklist

} Config;



// parse json file into Config structure
void load_config_json(const char *filename, Config *config);

#endif