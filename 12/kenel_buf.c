487		case SO_SNDBUF:
488			/* Don't error on this BSD doesn't and if you think
489			   about it this is right. Otherwise apps have to
490			   play 'guess the biggest size' games. RCVBUF/SNDBUF
491			   are treated in BSD as hints */
492	
493			if (val > sysctl_wmem_max)
494				val = sysctl_wmem_max;
495	set_sndbuf:
496			sk->sk_userlocks |= SOCK_SNDBUF_LOCK;
497			if ((val * 2) < SOCK_MIN_SNDBUF)
498				sk->sk_sndbuf = SOCK_MIN_SNDBUF;
499			else
500				sk->sk_sndbuf = val * 2;
501	
502			/*
503			 *	Wake up sending tasks if we
504			 *	upped the value.
505			 */
506			sk->sk_write_space(sk);
507			break;
	

516		case SO_RCVBUF:
517			/* Don't error on this BSD doesn't and if you think
518			   about it this is right. Otherwise apps have to
519			   play 'guess the biggest size' games. RCVBUF/SNDBUF
520			   are treated in BSD as hints */
521	
522			if (val > sysctl_rmem_max)
523				val = sysctl_rmem_max;
524	set_rcvbuf:
525			sk->sk_userlocks |= SOCK_RCVBUF_LOCK;
526			/*
527			 * We double it on the way in to account for
528			 * "struct sk_buff" etc. overhead.   Applications
529			 * assume that the SO_RCVBUF setting they make will
530			 * allow that much actual data to be received on that
531			 * socket.
532			 *
533			 * Applications are unaware that "struct sk_buff" and
534			 * other overheads allocate from the receive buffer
535			 * during socket buffer allocation.
536			 *
537			 * And after considering the possible alternatives,
538			 * returning the value we actually used in getsockopt
539			 * is the most desirable behavior.
540			 */
541			if ((val * 2) < SOCK_MIN_RCVBUF)
542				sk->sk_rcvbuf = SOCK_MIN_RCVBUF;
543			else
544			sk->sk_rcvbuf = val * 2;
545		break;


1102		if (num_physpages <= 4096) {
1103			sysctl_wmem_max = 32767;
1104			sysctl_rmem_max = 32767;
1105			sysctl_wmem_default = 32767;
1106			sysctl_rmem_default = 32767;
1107		} else if (num_physpages >= 131072) {
1108			sysctl_wmem_max = 131071;
1109			sysctl_rmem_max = 131071;
1110		}