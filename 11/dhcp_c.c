
#include "dhcp_c.h"
uint16_t udhcp_checksum(void *addr, int count)
{
	/* Compute Internet Checksum for "count" bytes
	 *         beginning at location "addr".
	 */
	int32_t sum = 0;
	uint16_t *source = (uint16_t *) addr;

	while (count > 1)  {
		/*  This is the inner loop */
		sum += *source++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if (count > 0) {
		/* Make sure that the left-over byte is added correctly both
		 * with little and big endian hosts */
		uint16_t tmp = 0;
		*(uint8_t*)&tmp = *(uint8_t*)source;
		sum += tmp;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

/* Construct a ip/udp header for a packet, send packet */
int udhcp_send_raw_packet(struct dhcpMessage *payload,
		uint32_t source_ip, int source_port,
		uint32_t dest_ip, int dest_port, const uint8_t *dest_arp, int ifindex)
{
	struct sockaddr_ll dest;
	struct udp_dhcp_packet packet;
	int fd;
	int result = -1;
	const char *msg;

	enum {
		IP_UPD_DHCP_SIZE = sizeof(struct udp_dhcp_packet) - CONFIG_UDHCPC_SLACK_FOR_BUGGY_SERVERS,
		UPD_DHCP_SIZE    = IP_UPD_DHCP_SIZE - offsetof(struct udp_dhcp_packet, udp),
	};

	fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
	if (fd < 0) {
		msg = "socket(%s)";
		goto ret_msg;
	}

	memset(&dest, 0, sizeof(dest));
	memset(&packet, 0, sizeof(packet));
	packet.data = *payload; /* struct copy */

	dest.sll_family = AF_PACKET;
	dest.sll_protocol = htons(ETH_P_IP);
	dest.sll_ifindex = ifindex;
	dest.sll_halen = 6;
	memcpy(dest.sll_addr, dest_arp, 6);
	if (bind(fd, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
		msg = "bind(%s)";
		goto ret_close;
	}

	packet.ip.protocol = IPPROTO_UDP;
	packet.ip.saddr = source_ip;
	packet.ip.daddr = dest_ip;
	packet.udp.source = htons(source_port);
	packet.udp.dest = htons(dest_port);
	/* size, excluding IP header: */
	packet.udp.len = htons(UPD_DHCP_SIZE);
	/* for UDP checksumming, ip.len is set to UDP packet len */
	packet.ip.tot_len = packet.udp.len;
	packet.udp.check = udhcp_checksum(&packet, IP_UPD_DHCP_SIZE);
	/* but for sending, it is set to IP packet len */
	packet.ip.tot_len = htons(IP_UPD_DHCP_SIZE);
	packet.ip.ihl = sizeof(packet.ip) >> 2;
	packet.ip.version = IPVERSION;
	packet.ip.ttl = IPDEFTTL;
	packet.ip.check = udhcp_checksum(&packet.ip, sizeof(packet.ip));

	/* Currently we send full-sized DHCP packets (zero padded).
	 * If you need to change this: last byte of the packet is
	 * packet.data.options[end_option(packet.data.options)]
	 */
	result = sendto(fd, &packet, IP_UPD_DHCP_SIZE, 0,
				(struct sockaddr *) &dest, sizeof(dest));
	msg = "sendto";
 ret_close:
	close(fd);
	if (result < 0) {
 ret_msg:
		bb_perror_msg(msg, "PACKET");
	}
	return result;
}


/* Returns -1 on errors that are fatal for the socket, -2 for those that aren't */
int udhcp_recv_raw_packet(struct dhcpMessage *payload, int fd)
{
	int bytes;
	struct udp_dhcp_packet packet;
	uint16_t check;

	memset(&packet, 0, sizeof(packet));
	bytes = safe_read(fd, &packet, sizeof(packet));
	if (bytes < 0) {
		DEBUG("Cannot read on raw listening socket - ignoring");
		/* NB: possible down interface, etc. Caller should pause. */
		return bytes; /* returns -1 */
	}

	if (bytes < (int) (sizeof(packet.ip) + sizeof(packet.udp))) {
		DEBUG("Packet is too short, ignoring");
		return -2;
	}

	if (bytes < ntohs(packet.ip.tot_len)) {
		/* packet is bigger than sizeof(packet), we did partial read */
		DEBUG("Oversized packet, ignoring");
		return -2;
	}

	/* ignore any extra garbage bytes */
	bytes = ntohs(packet.ip.tot_len);

	/* make sure its the right packet for us, and that it passes sanity checks */
	if (packet.ip.protocol != IPPROTO_UDP || packet.ip.version != IPVERSION
	 || packet.ip.ihl != (sizeof(packet.ip) >> 2)
	 || packet.udp.dest != htons(CLIENT_PORT)
	/* || bytes > (int) sizeof(packet) - can't happen */
	 || ntohs(packet.udp.len) != (uint16_t)(bytes - sizeof(packet.ip))
	) {
		DEBUG("Unrelated/bogus packet");
		return -2;
	}

	/* verify IP checksum */
	check = packet.ip.check;
	packet.ip.check = 0;
	if (check != udhcp_checksum(&packet.ip, sizeof(packet.ip))) {
		DEBUG("Bad IP header checksum, ignoring");
		return -2;
	}

	/* verify UDP checksum. IP header has to be modified for this */
	memset(&packet.ip, 0, offsetof(struct iphdr, protocol));
	/* ip.xx fields which are not memset: protocol, check, saddr, daddr */
	packet.ip.tot_len = packet.udp.len; /* yes, this is needed */
	check = packet.udp.check;
	packet.udp.check = 0;
	if (check && check != udhcp_checksum(&packet, bytes)) {
		bb_error_msg("packet with bad UDP checksum received, ignoring");
		return -2;
	}

	memcpy(payload, &packet.data, bytes - (sizeof(packet.ip) + sizeof(packet.udp)));

	if (payload->cookie != htonl(DHCP_MAGIC)) {
		bb_error_msg("received bogus message (bad magic), ignoring");
		return -2;
	}
	DEBUG("Got valid DHCP packet");
	return bytes - (sizeof(packet.ip) + sizeof(packet.udp));
}
