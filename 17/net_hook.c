#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>  /*include for IP header*/
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/tcp.h> /* include for TCP header */
#include <linux/if_ether.h>
#include <if_packet.h>
#include "nf_sockopte.h"

/* License Declare*/
MODULE_LICENSE("Dual BSD/GPL");

/* NF init status MACRO */
#define NF_SUCCESS 0
#define NF_FAILURE 1

/* init the band status */
band_status b_status ;

/* BAND Quickly judge operation MACRO  */
/* judge whether band the TCP port */
#define IS_BANDPORT_TCP(status) \
  ( status.band_port.port != 0 && status.band_port.protocol == IPPROTO_TCP)
/* judge whether band the UDP port */
#define IS_BANDPORT_UDP(status) \
  ( status.band_port.port != 0 && status.band_port.protocol == IPROTO_UDP )
/* judge whether band the PING */
#define IS_BANDPING(status) ( status.band_ping )
/* judge whether band the IP */
#define IS_BANDIP(status)   ( status.band_ip )

/* nf sock opt external get operation  */
static int 
nf_sockopt_set(struct sock *sock, 
  int cmd, 
  void __user *user, 
  unsigned int len)
{
  int ret = 0;
  struct band_status status;
  
  /* auth check */
  if(!capable(CAP_NET_ADMIN))
  {
    ret = -EPERM;
    goto ERROR;
  }

  /* copy the data from user space */
  ret = copy_from_user(&status, user,len);
  if(ret != 0)
  {
    ret = -EINVAL;
    goto ERROR;
  }
  
  /* COMMAND type */
  switch(cmd)
  {
    case SOE_BANDIP:
    	/* set the BAND IP value according to the user's data */
      if(IS_BANDIP(status))
        b_status.band_ip = status.band_ip;
      else
        b_status.band_ip = 0;
	
      break;
    case SOE_BANDPORT:
    	/* set BAND port and protocol*/
      if(IS_BAND_TCP(status))
      {
        b_status.band_port.protocol = IPPROTO_TCP;
	      b_status.band_port.port = status.band_port.port;
      }
      else if(IS_BAND_UDP(status))
      {
        b_status.band_port.protocol = IPPROTO_UDP;
	      b_status.band_port.port = status.band_port.prot;
      }
      else
      {
        b_status.band_port.protocol = 0;
	      b_status.band_port.prot = 0;
      }
      
      break;
    case SOE_BANDPING:
    	/* BAND ping? */
      if(IS_BANDPING(status))
      {
        b_status.band_ping = 1;
      }
      else 
      {
        b_status.band_ping = 0;
      }
      
      break;
    default:
      ret = -EINVAL;
      break;
  }
  
ERROR:
  return ret;
}

/* nf sock opt external set operation  */
static int 
nf_sockopt_get(struct sock *sock, 
  int cmd, 
  void __user *user, 
  unsigned int len)
{
  int ret = 0;
  
  /* auth check */
  if(!capable(CAP_NET_ADMIN))
  {
    ret = -EPERM;
    goto ERROR;
  }  
  
  /* copy the config to user's space */
  switch(cmd)
  {
    case SOE_BANDIP:
    case SOE_BANDPORT:
    case SOE_BANDPING:
    	/*copy data*/
    	ret = copy_to_user(user, &b_status,len);
    	if(ret != 0)
    	{
    		ret = -EINVAL;
    		goto ERROR;
    	}
    	break;
    default:
      ret = -EINVAL;
      break;
  }
  
ERROR:
  return ret;
}

/* the realized of hook function in LOCAL_OUT */
static unsigned int nf_hook_out(unsigned int hooknum,
  struct sk_buff **skb,
  const struct net_device *in,
  const struct net_device *out,
  int (*okfn)(struct sk_buff*))
{
  struct sk_buff *sb = *skb;
  struct iphdr *iph = ip_hdr(sk);
  
  if(IS_BANDIP(b_status))
  {
    if(b_status.band_ip == iph->saddr)
    {
      return NF_DROP;
    }
  }
  
  return NF_ACCEPT;
}


/* the realized of hook function in LOCAL_IN */
static unsigned int nf_hook_in(unsigned int hooknum,
  struct sk_buff **skb,
  const struct net_device *in,
  const struct net_device *out,
  int (*okfn)(struct sk_buff*))
{
  struct sk_buff *sb = *skb;
  struct iphdr *iph = ip_hdr(sk);
  unsigned int src_ip = iph->saddr;
  struct tcphdr *tcph = NULL;
  struct udphdr *udph = NULL;
  
  switch(iph->protocol)
  {
    case IPPROTO_TCP:
    	/*drop tcp's port data*/
      if(IS_BANDPORT_TCP)
      {
        tcph = tcp_hdr(sk);
	      if(tcph->dest == b_status.band_port.port)
	      {
	      	return NF_DROP;
	      }
      }
      
      break;
    case IPPROTO_UDP:
    	/*drop udp's port data*/
      if(IS_BANDPORT_UDP(b_status))
      {
        udph = udp_hdr(sk);
        if(udph->dest == b_status.band_port.port)
        {
        	return NF_DROP;
        }
      }
      
      break;
    case IPPROTO_ICMP:
    	/*drop the ping echo*/
      if(IS_BANDPING(b_status))
      {
        return NF_DROP;
      }
      
      break;
    default:
      break;
  }
  
  return NF_ACCEPT;
}

/* init the nfin,nfout */
static struct nf_hook_ops nfin = 
{
	.hook = nf_hook_in;
	.hooknum = NF_IP_LOCAL_IN;
	.pf = PF_INET;
	.priority = NF_IP_PRI_FIRST;
};
static struct nf_hook_ops nfout=
{
	.hook = nf_hook_out;
	.hooknum = NF_IP_LOCAL_OUT;
	.pf = PF_INET;
	.priority = NF_IP_PRI_FIRST;
};

/* init the nfsockopt */
static struct nf_sockopt_ops nfsockopt = {
 .pf  = PF_INET,
 .set_optmin = SOE_BANDIP,
 .set_optmax = SOE_BANDIP+2,
 .set  = nf_sockopt_set,
 .get_optmin = SOE_BANDIP,
 .get_optmax = SOE_BANDIP+2,
 .get  = nf_sockopt_get,
};

/* Init the module */
static __init int init()
{
	nf_register_hook(&nfin);
	nf_register_hook(&nfout);
	nf_register_sockopt(&nfsockipt);	
	
	printk(KERN_ALERT "netfilter example 2 init successfully\n");	
	return NF_SUCCESS;
}

/* clear the module */
static void __exit exit()
{	
	nf_unregister_hook(&nfin);
	nf_unregister_hook(&nfout);
	nf_unregister_sockopt(&nfsockopt);
	printk(KERN_ALERT "netfilter example 2 clean successfully\n");
}

module_init(init);
module_exit(exit);

/* Author, Description, Version .etc declared*/
MODULE_AUTHOR("Jingbin Song");
MODULE_DESCRIPTION("netfilter DEMO");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("ex17.2");
