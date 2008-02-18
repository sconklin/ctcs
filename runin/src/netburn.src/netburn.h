#include	<netinet/in_systm.h>
#include	<netinet/ip.h>
#include	<netinet/ip_icmp.h>
#include	<stdarg.h>		/* ANSI C header file */
#include	<syslog.h>		/* for syslog() */

#define	BUFSIZE		2500
#define	MAXLINE		100
#define 	VERSION		"0.11"
//////////////////////////////////////
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#if TIME_WITH_SYS_TIME
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#else
#if HAVE_SYS_TIME_H
#include	<sys/time.h>	/* includes <time.h> unsafely */
#else
#include	<time.h>		/* old system? */
#endif
#endif
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */



/* OSF/1 actually disables recv() and send() in <sys/socket.h> */
#ifdef	__osf__
#undef	recv
#undef	send
#define	recv(a,b,c,d)	recvfrom(a,b,c,d,0,0)
#define	send(a,b,c,d)	sendto(a,b,c,d,0,0)
#endif

#ifndef	INADDR_NONE
/* $$.Ic INADDR_NONE$$ */
#define	INADDR_NONE	0xffffffff	/* should have been in <netinet/in.h> */
#endif

#ifndef	SHUT_RD				/* these three POSIX names are new */
#define	SHUT_RD		0	/* shutdown for reading */
#define	SHUT_WR		1	/* shutdown for writing */
#define	SHUT_RDWR	2	/* shutdown for reading and writing */
/* $$.Ic SHUT_RD$$ */
/* $$.Ic SHUT_WR$$ */
/* $$.Ic SHUT_RDWR$$ */
#endif



void	*Calloc(size_t, size_t);
int	Socket(int family, int type, int protocol);
void	Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen);
char	*Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
int	spawn(const char* program,char* const arg_list[]);	   
void	err_sys(const char *fmt, ...);
void	err_quit(const char *fmt, ...);
static void	err_doit(int errnoflag, int level, const char *fmt, va_list ap);

typedef	void	Sigfunc(int);	/* for signal handlers */
Sigfunc	*signal_intr(int, Sigfunc *);
Sigfunc	*Signal(int signo, Sigfunc *func);

struct addrinfo	*Host_serv(const char *, const char *, int, int);
char	*sock_ntop_host(const struct sockaddr *A, socklen_t);

void Gettimeofday(struct timeval *tv, void *foo);
//////////////////////////////////////
			/* globals */
char		sendbuf[BUFSIZE];
int		datalen;			/* # bytes of data following ICMP header */
char		*host;
int		nsent;				/* add 1 for each sendto() */
int		nlost;
pid_t		pid;				/* our PID */
int		sockfd;
int		verbose;
int		threshold;
double		sum_rtt;

/* function prototypes */
void		proc(char *, ssize_t, struct msghdr *, struct timeval *);
void		send_packet(void);
void		readloop(int num_packets, int timeout);
void		sig_alrm(int);
void		tv_sub(struct timeval *, struct timeval *);
uint16_t	in_cksum(uint16_t *, int);

struct proto {
  void	 (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
  void	 (*fsend)(void);
  void	 (*finit)(void);
  struct sockaddr  *sasend;	/* sockaddr{} for send, from getaddrinfo */
  struct sockaddr  *sarecv;	/* sockaddr{} for receiving */
  socklen_t	    salen;		/* length of sockaddr{}s */
  int	   	    icmpproto;	/* IPPROTO_xxx value for ICMP */
} *pr;

