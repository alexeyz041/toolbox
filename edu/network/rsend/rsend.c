// send send data to raw socket

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <linux/if_tun.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>



int open_ethif(const char *ifname)
{
struct ifreq ifr;
struct sockaddr_ll sll;
struct packet_mreq mreq;
int flags;
int sock;
int ifindex;
    if((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
    	perror("socket");
    	goto fail;
    }

    memset(&ifr, 0, sizeof ifr);
    snprintf(ifr.ifr_name, sizeof ifr.ifr_name, "%s", ifname);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
    	perror("ioctl(SIOCGIFINDEX)");
    	goto fail;
    }
    ifindex = ifr.ifr_ifindex;

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
    	perror("ioctl(SIOCGIFHWADDR)");
    	goto fail;
    }
    if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER &&
    		ifr.ifr_hwaddr.sa_family != ARPHRD_LOOPBACK) {
    	printf("%s: not an ethernet interface", ifname);
    	goto fail;
    }
    if (IFHWADDRLEN != 6) {
    	printf("%s: bad hardware address length", ifname);
    	goto fail;
    }

    if (ioctl(sock, SIOCGIFMTU, &ifr) < 0)  {
    	perror("ioctl(SIOCGIFMTU)");
    	goto fail;
    }

    memset(&sll, 0, sizeof sll);
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifindex;
    if (bind(sock, (void *)&sll, sizeof sll) < 0)  {
    	perror("bind");
    	goto fail;
    }
/*
    // enable promiscuous reception
    memset(&mreq, 0, sizeof mreq);
    mreq.mr_ifindex = ifindex;
    mreq.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof mreq) < 0) {
    	perror("setsockopt");
    	goto fail;
    }
*/
    return sock;

fail:
    close(sock);
    return -1;
}


void usage()
{
    printf("Usage:\n");
    printf(" rsend iface file\n");
    exit(1);
}


int is_hd(uint8_t x)
{
    if(x >= '0' && x <= '9') return 1;
    if(x >= 'A' && x <= 'F') return 1;
    if(x >= 'a' && x <= 'f') return 1;
    return 0;
}


int xd(uint8_t x)
{
    if(x >= '0' && x <= '9') return x-'0';
    if(x >= 'A' && x <= 'F') return x-'A'+10;
    if(x >= 'a' && x <= 'f') return x-'a'+10;
    return 0;
}

int to_n(uint8_t *p)
{
    return (xd(p[0]) << 4) + xd(p[1]);
}



int load(char *fnm,char *buf,int size)
{
char buffer[4096] = {0};
char buffer2[4096] = {0};
int len = 0;
int pos = 0;

    FILE *f = fopen(fnm,"r");
    if(!f) {
	printf("can\'t load %s\n",fnm);
	exit(1);
    }
    len = fread(buffer,1,sizeof(buffer),f);
    fclose(f);

    for(int i=0; i < len; i++) {
	if(is_hd(buffer[i])) {
	    buffer2[pos++] = buffer[i];
	}
    }

    if(pos % 2 != 0) {
	printf("odd number of chars in input file\n");
	exit(1);
    }

    for(int i=0; i < pos/2; i++) {
	buf[i] = to_n(&buffer2[2*i]);
    }

    return pos/2;
}


int main(int argc,char **argv)
{
int sock;
int len;
char buffer[1024];

    if(argc != 3) usage();

    sock = open_ethif(argv[1]);
    if(sock < 0) {
	perror("Open interface");
	exit(1);
    }

    len = load(argv[2],buffer,sizeof(buffer));

    printf("Sending %d bytes to device %s \n", len, argv[2]);
    for(int i=0; i < len; i++) {
	printf("%02x ",buffer[i] & 0xff);
	if((i & 0x0f) == 0x0f) printf("\n");
    }
    printf("\n");

    int nwrite = send(sock,buffer,len,0);
    if(nwrite < 0) {
	perror("Writing to interface");
	exit(1);
    }

    close(sock);
    return 0;
}


