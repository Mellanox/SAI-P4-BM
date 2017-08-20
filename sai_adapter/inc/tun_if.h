#ifndef TUN_IF_H
#define TUN_IF_H
int tun_alloc(char *dev, int make_presistent);
int tun_delete(int tun_fd);
#endif