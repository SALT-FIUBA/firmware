#include "network.h"

#include "tcp_priv.h"


void print_network_status(void)
{
    struct netif * netif = netif_default;

    printf("\n=== Network Status ===\n");

    if (!netif) {
        printf("no network interface \n");
        return;
    }

    if (netif != NULL)
    {
        // Print interface status
        printf("Interface Status: %s\n",
               netif_is_up(netif) ? "UP" : "DOWN");
        printf("Link Status: %s\n",
               netif_is_link_up(netif) ? "UP" : "DOWN");

        // Print IP configuration
        printf("IP Address: %d.%d.%d.%d\n",
               ip4_addr1(&netif->ip_addr),
               ip4_addr2(&netif->ip_addr),
               ip4_addr3(&netif->ip_addr),
               ip4_addr4(&netif->ip_addr));

        printf("Netmask: %d.%d.%d.%d\n",
               ip4_addr1(&netif->netmask),
               ip4_addr2(&netif->netmask),
               ip4_addr3(&netif->netmask),
               ip4_addr4(&netif->netmask));

        printf("Gateway: %d.%d.%d.%d\n",
               ip4_addr1(&netif->gw),
               ip4_addr2(&netif->gw),
               ip4_addr3(&netif->gw),
               ip4_addr4(&netif->gw));

        // Print interface details
        printf("Hardware Address (MAC): %02X:%02X:%02X:%02X:%02X:%02X\n",
               netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
               netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

        // Print interface metrics
        printf("MTU: %d\n", netif->mtu);

        // Print interface name and number
        printf("Interface Name: %c%c%d\n",
               netif->name[0], netif->name[1], netif->num);

        // Print interface flags
        printf("Flags: ");
        if (netif->flags & NETIF_FLAG_UP) printf("UP ");
        if (netif->flags & NETIF_FLAG_BROADCAST) printf("BROADCAST ");
        if (netif->flags & NETIF_FLAG_LINK_UP) printf("LINK_UP ");
        if (netif->flags & NETIF_FLAG_ETHARP) printf("ETHARP ");
        if (netif->flags & NETIF_FLAG_ETHERNET) printf("ETHERNET ");
        printf("\n");

        // Print interface statistics if available
#if ETH_PAD_SIZE
        printf("Padding size: %d\n", ETH_PAD_SIZE);
#endif

    }
    else
    {
        printf("No network interface found!\n");
    }

    // Print TCP/IP stack status
    // Print TCP/IP stack status
#if LWIP_STATS
    printf("\nTCP/IP Statistics:\n");

    // Count active TCP connections
    u32_t active_count = 0;
    for (struct tcp_pcb *pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
        active_count++;
    }
    printf("TCP Connections: %lu\n", active_count);

    // Count listening TCP sockets
    u32_t listen_count = 0;
    for (struct tcp_pcb_listen *lpcb = tcp_listen_pcbs.listen_pcbs; lpcb != NULL; lpcb = lpcb->next) {
        listen_count++;
    }
    printf("TCP Listen: %lu\n", listen_count);

    // Count time-wait TCP connections
    u32_t tw_count = 0;
    for (struct tcp_pcb *pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
        tw_count++;
    }
    printf("TCP Time-Wait: %lu\n", tw_count);
#endif

    printf("===================\n\n");
}

err_t check_network_status(void)
{
    struct netif * netif = netif_default;

    /* Check if network interface is up */
    if (!netif_is_up(netif)) {
        return ERR_IF;
    }

    /* Check if link is up */
    if (!netif_is_link_up(netif)) {
        return ERR_IF;
    }

    /* Check if we have valid IP */
    if (ip4_addr_isany_val(*netif_ip4_addr(netif))) {
        return ERR_IF;
    }

    return ERR_OK;
}

// Define network interface check functions
int is_netif_up(void) {
    struct netif *netif = netif_default;

    return netif && netif->flags & NETIF_FLAG_UP;
}

int is_link_up(void) {
    struct netif *netif = netif_default;

    return netif && netif->flags & NETIF_FLAG_LINK_UP;
}

// Wait for the network to be ready
void wait_for_network(void) {

    while (!is_netif_up() || !is_link_up()) {
        sys_check_timeouts();
    }
}















