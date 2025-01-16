#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

// Function to get the interface index from the IP address
int get_interface_index_from_ip(const char *ip_address) {
    struct ifaddrs *ifaddr, *ifa;
    int family, s, interface_index = -1;
    char host[NI_MAXHOST];

    // Get the list of network interfaces
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    // Loop through the list of network interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        // Check for IPv4 or IPv6 family
        if (family == AF_INET || family == AF_INET6) {
            // Convert the address to a readable form
            s = getnameinfo(ifa->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST,
                            NULL, 0, NI_NUMERICHOST);

            if (s != 0) {
                fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }

            // Compare the provided IP address with the current interface address
            if (strcmp(host, ip_address) == 0) {
                // Get the interface index
                interface_index = if_nametoindex(ifa->ifa_name);
                break;
            }
        }
    }

    // Free the interface list
    freeifaddrs(ifaddr);

    return interface_index;
}

int main() {
    const char *ip_address = "172.18.0.1"; // Replace with your IP address
    int interface_index = get_interface_index_from_ip(ip_address);

    if (interface_index != -1) {
        printf("Interface index for IP address %s: %d\n", ip_address, interface_index);
    } else {
        printf("No interface found for IP address %s\n", ip_address);
    }

    return 0;
}

