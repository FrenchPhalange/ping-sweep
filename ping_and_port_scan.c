#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netdb.h>

// Function to calculate the ICMP checksum
unsigned short calculate_checksum(void *b, int len) {    
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Function to perform the ping sweep
void ping_sweep(const char *network, int cidr) {
    int sockfd, icmp_seq = 0;
    char buffer[1024] = {0};
    struct ip *ip_hdr = (struct ip *)buffer;
    struct icmp *icmp_hdr = (struct icmp *)(buffer + sizeof(struct ip));
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    char target_ip[20];

    // Creating raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setting up destination address
    addr.sin_family = AF_INET;

    // Generating IP addresses for ping
    for (int host = 1; host < (1 << (32 - cidr)); host++) {
        snprintf(target_ip, sizeof(target_ip), "%s.%d", network, host);
        addr.sin_addr.s_addr = inet_addr(target_ip);

        memset(icmp_hdr, 0, sizeof(struct icmp));
        icmp_hdr->icmp_type = ICMP_ECHO;
        icmp_hdr->icmp_code = 0;
        icmp_hdr->icmp_id = getpid();
        icmp_hdr->icmp_seq = icmp_seq++;
        icmp_hdr->icmp_cksum = 0;
        icmp_hdr->icmp_cksum = calculate_checksum(icmp_hdr, sizeof(struct icmp));

        if (sendto(sockfd, buffer, sizeof(struct icmp) + sizeof(struct ip), 0, (struct sockaddr *)&addr, addr_len) <= 0) {
            perror("Sendto failed");
            continue;
        }

        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len) > 0) {
            struct ip *ip_hdr_r = (struct ip *)buffer;
            struct icmp *icmp_hdr_r = (struct icmp *)(buffer + sizeof(struct ip));
            if (icmp_hdr_r->icmp_type == ICMP_ECHOREPLY)
                printf("Host %s is online.\n", inet_ntoa(addr.sin_addr));
        }
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <network base IP> <CIDR>\n", argv[0]);
        return 1;
    }
    
    ping_sweep(argv[1], atoi(argv[2]));
    return 0;
}
