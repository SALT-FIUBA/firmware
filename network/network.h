
#ifndef NUCLEO_F429ZI_NETWORK_H
#define NUCLEO_F429ZI_NETWORK_H

#include "netif.h"
#include "timeouts.h"

void print_network_status(void);
err_t check_network_status(void);
int is_netif_up(void);
int is_link_up(void);
void wait_for_network(void);



#endif //NUCLEO_F429ZI_NETWORK_H
