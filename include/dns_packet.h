#ifndef _DNS_PACKET
#define _DNS_PACKET

#include <dns_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


// dns-udp standard
#define BUFFER_SIZE 512 



int extract_domain(const unsigned char *packet, unsigned char *domain) {
    int pos = 12; // skip dns header
    int domain_pos = 0;
    int len;

    while (1) {
        // get domain segment lenght
        len = packet[pos];
        // if lenght == 0 - all domain parts copied
        if (len == 0) break;

        // if packet real size > excepted size 
        if ((pos + len + 1) > BUFFER_SIZE) return 0;

        // copy domain segment from packet to domain variable
        memcpy(domain + domain_pos, packet + pos + 1, len);
        // update pointers
        domain_pos += len + 1;
        pos += len + 1;
        // add '.' to segment (e.g. "www" + '.')
        domain[domain_pos - 1] = '.';
    }
    // last dot replaced with null-term
    domain[domain_pos - 1] = '\0';
    return 1;

}


int is_blocked(const Config *config, const unsigned char *domain) {

    for (int i = 0; i < config->blacklist_count; ++i) {
        if (strcasecmp(domain, config->blacklist[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


#endif