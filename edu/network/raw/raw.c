// send ARP request using raw socket

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
    if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
	perror("%s: not an ethernet interface");
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

    /* enable promiscuous reception */
    memset(&mreq, 0, sizeof mreq);
    mreq.mr_ifindex = ifindex;
    mreq.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof mreq) < 0) {
      perror("setsockopt");
      goto fail;
    }

    return sock;

fail:
    close(sock);
    return -1;
}


void usage()
{
    printf("Usage:\n raw iface ip\n");
    exit(1);
}



int getmac(const char *ifn,uint8_t *mac,uint8_t *ip)
{
struct ifreq s;
int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strcpy(s.ifr_name, ifn);
    if (ioctl(fd, SIOCGIFHWADDR, &s)) {
	return -1;
    }
    memcpy(mac, s.ifr_addr.sa_data, 6);
    memcpy(ip, &((struct sockaddr_in *)&s.ifr_addr)->sin_addr, 4);
    return 0;
}


void build_arp(uint8_t *arp,uint8_t *ip,uint8_t *mac,uint8_t *trg_ip)
{
    memset(arp,0xff,6);
    memcpy(arp+6,mac,6);
    arp[12] = 8;	arp[13] = 6;
    arp[14] = 0;	arp[15] = 1;

    arp[16] = 8;	arp[17] = 0;
    arp[18] = 6;	arp[19] = 4;
    arp[20] = 0;	arp[21] = 1;

    memcpy(arp+22,mac,6);
    memcpy(arp+28,ip,4);
    memset(arp+32,0,6);
    memcpy(arp+38,trg_ip,4);
}


int main(int argc,char **argv)
{
int sock;
int i;
char buffer[1024];
uint8_t ip[4];
uint8_t mac[6];
uint8_t arp[64];
uint8_t trg_ip[4];
in_addr_t tmp;

    if(argc != 3) usage();

    sock = open_ethif(argv[1]);
    if(sock < 0) {
	perror("Open interface");
	exit(1);
    }

    if(getmac(argv[1],ip,mac) != 0) {
	perror("Get ip/mac");
	exit(1);
    }

    printf("my IP: %d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
    printf("my MAC: %02x.%02x.%02x.%02x.%02x.%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    tmp = inet_addr(argv[2]);
    memcpy(trg_ip,&tmp,4);
    printf("trg IP: %d.%d.%d.%d\n",trg_ip[0],trg_ip[1],trg_ip[2],trg_ip[3]);

    build_arp(arp,ip,mac,trg_ip);

    int nwrite = send(sock,arp,42,0);
    if(nwrite < 0) {
	perror("Writing to interface");
	exit(1);
    }

    int nread = recv(sock,buffer,sizeof(buffer),0);
    if(nread < 0) {
        perror("Reading from interface");
        exit(1);
    }

    printf("Read %d bytes from device %s \n", nread, argv[2]);
    for(i=0; i < nread; i++) {
	printf("%02x ",buffer[i] & 0xff);
	if((i & 0x0f) == 0x0f) printf("\n");
    }
    printf("\n");

    printf("trg MAC: %02x.%02x.%02x.%02x.%02x.%02x\n",buffer[6] & 0xff,buffer[7] & 0xff,buffer[8] & 0xff,buffer[9] & 0xff,buffer[10] & 0xff,buffer[11] & 0xff);

    close(sock);
    return 0;
}


