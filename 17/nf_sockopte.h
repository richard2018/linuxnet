/* file: nf_sockopte.h
 * example 17-1
 * author: songjingbin <flyingfat@163.com>
 * sockopt extern header file  */
#ifndef __NF_SOCKOPTE_H__
#define __NF_SOCKOPTE_H__

#define SOE_BANDIP    0x6001
#define SOE_BANDPORT  0x6002
#define SOE_BANDPING  0x6003

/* band port sockopt command used struture,
 * copy datum from user space and kernel space*/
typedef struct nf_bandport
{
  /* band protocol, TCP?UDP  */
  unsigned short protocol;
  
  /* band port */
  unsigned short port;
};


typedef struct band_status{
  /* the ip user to band, 0 is not band  */
  unsigned int  band_ip;
  
  /*port to band, {0,0} is not set*/
  nf_bandport band_port;
  
  /* whether the ping is band, 0 NO, 1 YES  */
  unsigned char band_ping;
}band_status;


#endif /* __NF_SOCKOPTE_H__ */
