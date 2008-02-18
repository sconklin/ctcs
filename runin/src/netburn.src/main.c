#include	"netburn.h"

struct proto	proto_v4 = { proc, send_packet, NULL, NULL, NULL, 0, IPPROTO_ICMP };


void syntax_error(char* arg0)
{
	fprintf(stderr,"Syntax error - Usage:\n");
	fprintf(stderr,"%s  -n [number of packets] -s [packet size] -t [time out] -x [threshold size] -v   <host address>\n",arg0 );
	fprintf(stderr,"\n  Switches:\n" );
	fprintf(stderr,"\t-n  Specifies number of packets to send  (default: 1000)\n" );
	fprintf(stderr,"\t-s  Size of data in ICMP packets in bytes excluding the ICMP header size  (default: 56)\n" );	
	fprintf(stderr,"\t-t  Time out duration in seconds  (default: 1 second)\n" );
	fprintf(stderr,"\t-x  Threshold of acceptable packet loss  (default: 3)\n" );
	fprintf(stderr,"\t-v  Verbose mode\n" );
	exit(-1);
}
int main(int argc, char **argv)
{
	int		c;
	int		timeout = 1;
	int		num_packets = 1000;
	struct addrinfo	*ai;
	char *h;
	
	sum_rtt = 0.0f;
	datalen = 56;
	nlost = 0;
	threshold = 3;
	
	printf("NetBurn %s\n", VERSION);
	opterr = 0;		/* don't want getopt() writing to stderr */
	while ( (c = getopt(argc, argv, "n:s:t:x:v")) != -1) {
		switch (c) {
		case 'n':	
			if(argc-1<optind)
				syntax_error(argv[0]);
			num_packets = atoi(argv[optind-1]);
			break;
		case 's':	
			if(argc-1<optind)
				syntax_error(argv[0]);
			datalen = atoi(argv[optind-1]);
			break;
		case 't':
			if(argc-1<optind)
				syntax_error(argv[0]);
			timeout = atoi(argv[optind-1]);
			break;
		case 'x':
			if(argc-1<optind)
				syntax_error(argv[0]);
			threshold = atoi(argv[optind-1]);
			break;
		case 'v':
			verbose = 1;
			break;

		case '?':
			syntax_error(argv[0]);
		}
	}
	
	if (optind != argc-1)
		syntax_error(argv[0]);
	host = argv[optind];

	pid = getpid() & 0xffff;	/* ICMP ID field is 16 bits */
	Signal(SIGALRM, sig_alrm);

	ai = Host_serv(host, NULL, 0, 0);

	h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
	printf("Sending ICMP packets to %s (%s): %d data bytes\n\n",
			ai->ai_canonname ? ai->ai_canonname : h,
			h, datalen);

		/* 4initialize according to protocol */
	if (ai->ai_family == AF_INET) {
		pr = &proto_v4;
	} else
		err_quit("unknown address family %d", ai->ai_family);

	pr->sasend = ai->ai_addr;
	pr->sarecv = Calloc(1, ai->ai_addrlen);
	pr->salen = ai->ai_addrlen;
	// default Spped/Duplex
	printf("Default Network Speed/Duplex\n");
	readloop(num_packets,timeout);
//	We do not change the speed, yet
/*	
	int mii_status;
	char* arg_list[] = {
				"mii-diag",
				"-F 100",
				NULL
	};
	
	// default 100Speed/Full Duplex
	arg_list[1] = "-F 100",
	printf("\n\n- Setting 100BaseTX-Full Duplex:\n");
	spawn("mii-diag",arg_list);
	wait(&mii_status);
	if (WIFEXITED (mii_status))
	{
		printf("Exited with the code %d\n\n", WEXITSTATUS(mii_status));
		readloop(num_packets,timeout);
	}
	else
		printf("Error cannot set status speed\n\n");
			
	// default 100Speed/Half Duplex
	arg_list[1] = "-F 80",
	printf("\n\n- Setting 100BaseTX-Half Duplex:\n");
	spawn("mii-diag",arg_list);
	wait(&mii_status);
	if (WIFEXITED (mii_status))
	{
		printf("Exited with the code %d\n\n", WEXITSTATUS(mii_status));
		readloop(num_packets,timeout);
	}
	else
		printf("Error cannot set status speed\n\n");

	// default 10Speed/Full Duplex
	arg_list[1] = "-F 20",
	printf("\n\n- Setting 10BaseTX-Full Duplex:\n");
	spawn("mii-diag",arg_list);
	wait(&mii_status);
	if (WIFEXITED (mii_status))
	{
		printf("Exited with the code %d\n\n", WEXITSTATUS(mii_status));
		readloop(num_packets,timeout);
	}
	else
		printf("Error cannot set status speed\n\n");
	
	// default 10Speed/Half Duplex
	arg_list[1] = "-F 20",
	printf("\n\n- Setting 10BaseTX-Half Duplex:\n");
	spawn("mii-diag",arg_list);
	wait(&mii_status);
	if (WIFEXITED (mii_status))
	{
		printf("Exited with the code %d\n\n", WEXITSTATUS(mii_status));
		readloop(num_packets,timeout);
	}
	else
		printf("Error cannot set status speed\n\n");		

	// Autonegotiate
	arg_list[1] = "-A 1",
	printf("\n\n- Turning auto-negotiation on:\n");
	spawn("mii-diag",arg_list);
	wait(&mii_status);
	if (WIFEXITED (mii_status))
	{
		printf("Exited with the code %d\n\n", WEXITSTATUS(mii_status));
		readloop(num_packets,timeout);
	}
	else
		printf("Error cannot set status speed\n\n");
*/						
	printf("\n\tPackets Sent: %d\n", nsent);
	printf("\tPackets Lost: %d\n", nlost);
	if (!nlost)			// succeed
	{
		printf("\tAverage round trip time: %.1f ms\n", sum_rtt/nsent);
		printf("\tThroughput: %.f KB/s\n", 2*(double)(((datalen+8)*nsent)/1024) / (sum_rtt/1000));
		printf("\n\tNetBurn test has passed.\n\n");
		exit(0);
	}
	else if ( nlost<threshold )	// succeeded conditionally
	{
		printf("\n\tNetBurn test has passed, but some packets were loss.\n\n");
		exit(0);
	}
	else				// failed
	{
		printf("\n\tNetBurn test failed due to packet loss.\n\n");
		exit(1);
	}
	return 0;
}
