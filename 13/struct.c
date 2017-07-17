/*
 * 网际协议结构,IPv4，参见RFC 791
 */
struct ip
{
#if __BYTE_ORDER == __LITTLE_ENDIAN 	/* 如果为小端 */
	unsigned int ip_hl:4;								/* 头部长度 */
	unsigned int ip_v:4;								/* 版本 */
#endif
#if __BYTE_ORDER == __BIG_ENDIAN 			/*如果为大端*/
	unsigned int ip_v:4;								/* 版本 */
	unsigned int ip_hl:4;								/* 头部长度 */
#endif
	u_int8_t ip_tos;										/* TOS，服务类型 */
	u_short ip_len;											/* 总长度 */
	u_short ip_id;											/* 标识值 */
	u_short ip_off;											/* 段偏移值 */
	...
	...
	u_int8_t ip_ttl;										/* TTL，生存时间 */
	u_int8_t ip_p;											/* 协议类型 */
	u_short ip_sum;											/* 校验和 */
	struct in_addr ip_src, ip_dst;			/* 源地址和目的地址 */
};



struct icmp
{
	u_int8_t  icmp_type;	/* 消息类型 */
  u_int8_t  icmp_code;	/* 消息类型的子码 */
  u_int16_t icmp_cksum;	/* 校验和 */
  union
  {
    u_char ih_pptr;		/* ICMP_PARAMPROB */
    struct in_addr ih_gwaddr;	/* 网关地址 */
    struct ih_idseq				/* 显示数据报 */
    {
      u_int16_t icd_id;		/* 数据报ID */
      u_int16_t icd_seq;	/* 数据报的序号 */
    } ih_idseq;
    u_int32_t ih_void;

    /* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
    struct ih_pmtu
    {
      u_int16_t ipm_void;
      u_int16_t ipm_nextmtu;
    } ih_pmtu;

    struct ih_rtradv
    {
      u_int8_t irt_num_addrs;
      u_int8_t irt_wpa;
      u_int16_t irt_lifetime;
    } ih_rtradv;
  } icmp_hun;
#define	icmp_pptr	icmp_hun.ih_pptr
#define	icmp_gwaddr	icmp_hun.ih_gwaddr
#define	icmp_id		icmp_hun.ih_idseq.icd_id
#define	icmp_seq	icmp_hun.ih_idseq.icd_seq
#define	icmp_void	icmp_hun.ih_void
#define	icmp_pmvoid	icmp_hun.ih_pmtu.ipm_void
#define	icmp_nextmtu	icmp_hun.ih_pmtu.ipm_nextmtu
#define	icmp_num_addrs	icmp_hun.ih_rtradv.irt_num_addrs
#define	icmp_wpa	icmp_hun.ih_rtradv.irt_wpa
#define	icmp_lifetime	icmp_hun.ih_rtradv.irt_lifetime
  union
  {
    struct
    {
      u_int32_t its_otime;
      u_int32_t its_rtime;
      u_int32_t its_ttime;
    } id_ts;
    struct
    {
      struct ip idi_ip;
      /* options and then 64 bits of data */
    } id_ip;
    struct icmp_ra_addr id_radv;
    u_int32_t   id_mask;
    u_int8_t    id_data[1];	/* 数据 */
  } icmp_dun;
#define	icmp_otime	icmp_dun.id_ts.its_otime
#define	icmp_rtime	icmp_dun.id_ts.its_rtime
#define	icmp_ttime	icmp_dun.id_ts.its_ttime
#define	icmp_ip		icmp_dun.id_ip.idi_ip
#define	icmp_radv	icmp_dun.id_radv
#define	icmp_mask	icmp_dun.id_mask
#define	icmp_data	icmp_dun.id_data
};

struct icmp
{
	u_int8_t  icmp_type;	/* 消息类型 */
  u_int8_t  icmp_code;	/* 消息类型的子码 */
  u_int16_t icmp_cksum;	/* 校验和 */
  union
  {    
    struct ih_idseq				/* 显示数据报 */
    {
      u_int16_t icd_id;		/* 数据报ID */
      u_int16_t icd_seq;	/* 数据报的序号 */
    } ih_idseq;
  } icmp_hun;

#define	icmp_id		icmp_hun.ih_idseq.icd_id
#define	icmp_seq	icmp_hun.ih_idseq.icd_seq
  union
  {
    u_int8_t    id_data[1];	/* 数据 */
  } icmp_dun;
#define	icmp_data	icmp_dun.id_data
};


#ifdef __FAVOR_BSD			/* 如果喜欢BSD样式 */
struct udphdr
{
  u_int16_t uh_sport;		/* 源地址端口 */
  u_int16_t uh_dport;		/* 目的地址端口 */
  u_int16_t uh_ulen;		/* UDP长度 */
  u_int16_t uh_sum;			/* UDP校验和 */
};
#else										/* Linux样式 */
struct udphdr
{
  u_int16_t source;			/* 源地址端口 */
  u_int16_t dest;				/* 目的地址端口 */
  u_int16_t len;				/* UDP长度 */
  u_int16_t check;			/* UDP校验和 */
};
#endif

struct tcphdr 
{
	__u16	source;		/* 源地址端口 */
	__u16	dest;			/* 目的地址端口 */
	__u32	seq;			/* 序列号 */
	__u32	ack_seq;	/* 确认序列号 */
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u16	res1:4,		/* 保留 */
		doff:4,				/*  */
		fin:1,				/* 关闭连接标志 */
		syn:1,				/* 请求连接标志 */
		rst:1,				/* 重置连接标志 */
		psh:1,				/* 接收方尽快将数据放到应用层标志 */
		ack:1,				/* 确认序号标志 */
		urg:1,				/* 紧急指针标志 */
		ece:1,				/* 拥塞标志位 */
		cwr:1;				/* 拥塞标志位 */
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u16	doff:4,
		res1:4,				/* 保留 */
		cwr:1,				/* 拥塞标志位 */
		ece:1,				/* 拥塞标志位 */
		urg:1,				/* 紧急指针标志 */
		ack:1,				/* 确认序号标志 */
		psh:1,				/* 接收方尽快将数据放到应用层标志 */
		rst:1,				/* 重置连接标志 */
		syn:1,				/* 请求连接标志 */
		fin:1;				/* 关闭连接标志 */
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif	
	__u16	window;			/* 滑动窗口大小 */
	__u16	check;			/* 校验和 */
	__u16	urg_ptr;		/* 紧急字段指针 */
};
