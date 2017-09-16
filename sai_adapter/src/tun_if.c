#include <fcntl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int tun_alloc(char *dev, int make_presistent) { //, int flags) {

  struct ifreq ifr;
  int fd, err;
  char *clonedev = "/dev/net/tun";

  /* Arguments taken by the function:
   *
   * char *dev: the name of an interface (or '\0'). MUST have enough
   *   space to hold the interface name if '\0' is passed
   * int flags: interface flags (eg, IFF_TUN etc.)
   */

  /* open the clone device */
  if ((fd = open(clonedev, O_RDWR)) < 0) {
    return fd;
  }

  /* preparation of the struct ifr, of type "struct ifreq" */
  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags =
      IFF_TUN |
      IFF_NO_PI; // flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

  if (*dev) {
    /* if a device name was specified, put it in the structure; otherwise,
     * the kernel will try to allocate the "next" device of the
     * specified type */
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  /* try to create the device */
  if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
    close(fd);
    return err;
  }

  /* if the operation was successful, write back the name of the
   * interface to the variable "dev", so the caller can know
   * it. Note that the caller MUST reserve space in *dev (see calling
   * code below) */
  strcpy(dev, ifr.ifr_name);
  if (make_presistent == 1) {
    if (ioctl(fd, TUNSETPERSIST, 1) < 0) {
      printf("error enabling TUNSETPERSIST");
      exit(1);
    }
  }
  /* this is the special file descriptor that the caller will use to talk
   * with the virtual interface */
  return fd;
}

int tun_delete(int tun_fd) {
  if(ioctl(tun_fd, TUNSETPERSIST, 0) < 0){
      perror("disabling TUNSETPERSIST");
      exit(1);
  }
  close(tun_fd);
  return 0;
}