#ifndef __DHCPC_H__
#define __DHCPC_H__

#define DHCP_OPTIONS_BUFSIZE  308
struct dhcpMessage {
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t ciaddr;
	uint32_t yiaddr;
	uint32_t siaddr;
	uint32_t giaddr;
	uint8_t chaddr[16];
	uint8_t sname[64];
	uint8_t file[128];
	uint32_t cookie;
	uint8_t options[DHCP_OPTIONS_BUFSIZE + CONFIG_UDHCPC_SLACK_FOR_BUGGY_SERVERS];
} PACKED;
struct iphdr {
	__u8	ihl:4,
		version:4;
	__u8	tos;
	__be16	tot_len;
	__be16	id;
	__be16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__sum16	check;
	__be32	saddr;
	__be32	daddr;
	/*The options start here. */
};
struct udphdr {
	__be16	source;
	__be16	dest;
	__be16	len;
	__sum16	check;
};


struct udp_dhcp_packet {
	struct iphdr ip;
	struct udphdr udp;
	struct dhcpMessage data;
} PACKED;

#define IPVERSION 4
#define CLIENT_PORT 68
#define DHCP_MAGIC		0x63825363
#endif /* __DHCPC_H__ */