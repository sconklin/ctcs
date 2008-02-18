/* crashme: Create a string of random bytes and then jump to it.
            crashme [+]<nbytes>[.inc] <srand> <ntrys> [nsub] [verboseness] */

char *crashme_version = "2.4 20-MAY-1994";

/*
 *             COPYRIGHT (c) 1990-1994 BY        *
 *  GEORGE J. CARRETTE, CONCORD, MASSACHUSETTS.  *
 *             ALL RIGHTS RESERVED               *

Permission to use, copy, modify, distribute and sell this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all copies
and that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of the author
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

A signal handler is set up so that in most cases the machine exception
generated by the illegal instructions, bad operands, etc in the procedure
made up of random data are caught; and another round of randomness may
be tried. Eventually a random instruction may corrupt the program or
the machine state in such a way that the program must halt. This is
a test of the robustness of the hardware/software for instruction
fault handling.

Note: Running this program just a few times, using total CPU time of
less than a few seconds SHOULD NOT GIVE YOU ANY CONFIDENCE in system
robustness. Having it run for hours, with tens of thousands of cases
would be a different thing. It would also make sense to run this
stress test at the same time you run other tests, like a multi-user
benchmark.

Comments may be addressed to the author at GJC@MITECH.COM

See the documentation in crashme.1 and READ.ME, or read this code for 
a description of command line arguments to this program. 

Version Date         Description
----------------------------------------------------------------------
 1.0    early 1990   initial hack.
 1.1    19-SEP-1990  added more signals and an alarm to abort looping.
 1.2    25-JUN-1991  added [nsub] to vfork multiple subprocesses of self.
 1.3    14-AUG-1991  +nbytes malloc option, and -nsub option.
 1.4    29-AUG-1991  fix +nbytes (subproc). Add time-driven nprocs. SIGINT.
 1.5     3-SEP-1991  added alarm subprocess monitor to vfork_main.
 1.6     5-SEP-1991  some systems don't have vfork, so use fork by default.
 1.7    25-SEP-1991  verboseness level, exit summary report.
 1.8      -SEP-1991  address page protection issues on badboy.
 1.9     6-AUG-1993  DECC(VMS)/WIN32/NT/Posix, #ifdef some SIGxxx.
 2.0     7-SEP-1993  More extensive WIN32 conditionalization. record_note.
 2.1     6-MAY-1994  Added "dot" syntax to NBYTES. Modularized castaway.
 2.2     9-MAY-1994  __ALPHA && VMS version is now more interesting.
 2.3    11-MAY-1994  Added _IBMRT2 and _POWER code.
 2.4    20-MAY-1994  Added __hpux. Linux from jik@cam.ov.com.

Suggested test: At least let the thing run the length of your lunch break,
in this case 1 hour, 10 minutes, and 30 seconds.

  crashme +2000 666 100 1:10:30 2

Also, it may spend more time trapping and less time computing random bytes
by using these arguments:

  crashme +2000.80 666 100 1:10:30 2

                        CRASH REPORTS

Date,               Machine              Crashme        Reported 
Crashme Ver   Make    Model OS Version   Arguments      by:        
------------------------------------------------------------------------------
10-JUL-90 1.0 SUN     4/110 4.1          1000 20 200    GJC@paradigm.com
10-JUL-90 1.0 SUN     4/280 4.0.3        1000 20 200    GJC@paradigm.com
31-JUL-90 1.0 DIGITAL DECstation 3100    100 10 10000   GAVRON@ARIZONA.EDU
31-JUL-90 1.0 IBM     RT                 100 10 10000   GAVRON@ARIZONA.EDU
 1-AUG-90 1.0 DIGITAL DECstation 5000    10000 230 1000 hudgens@scri.fsu.edu
 3-AUG-90 1.0 Alliant FX/2800                           SJA@SIRIUS.HUT.FI
27-JUN-91 1.2 SUN     4/110 4.1.1        10 1000 10     LPH@PARADIGM.COM   
27-JUN-91 1.2 SUN     4/110 4.1.1        1000 20 200 10 LPH@PARADIGM.COM   
29-JUN-91 1.2 SUN     4/40C 4.1.1        9 29748 5877 4 jon@uk.ac.oxford.robots
29-JUN-91 1.2 SUN     4/60 4.1.1         9 29748 5877 4 jon@uk.ac.oxford.robots
29-JUN-91 1.2 SUN     4/100 4.1.1        9 29748 5877 4 jon@uk.ac.oxford.robots
29-JUN-91 1.2 SUN     4/65 4.1.1         9 29748 5877 4 jon@uk.ac.oxford.robots
18-JUL-91 1.2 SGI     Iris4d Unix 3.3.2  1000 $$ 1000 4 tsacas@ilog.ilog.fr
29-JUL-91 1.1 IBM     RS/6000 AIX 1.3    script         brandis@inf.ethz.ch
 5-SEP-91 1.6 IBM     RS/6000-320 AIX 3.1.5 +2000 666 50 40:00:00 LPH
26-SEP-91 1.8 Nixdorf Targon/35 TOS3.3   script         petri@ibr.cs.tu-bs.de
 9-SEP-93 2.0 Microsoft WNT Build 511 i486 +1000 24131 50 gjc@mitech.com
 3-FEB-94 1.8 HP710/HP-UX 9.00 +2000 666 100 2:00:00 5 UFOP@fpsp.fapesp.br
 5-MAY-94 2.0 HP807/HPUX 9.00  4000 666 100 00:30:00 2 UFOP@fpsp.fapesp.br

Notes: Crashme V1.0 {1000 20 200} used to down the SUN 4/110. V1.2 does *not*
crash SUNOS 4.1.1 on the same arguments. Although using the extra argument
for subprocesses it will crash, with the console reporting:
"Bad Trap, Bad Kernel Read Fault, Bus error. Reboot" 

Script means invoking file with many calls to crashme such as this:
#/bin/csh
crashme 1020 234 500 &
crashme 394 38484 5723 &
crashme 3784 474 474 &
crashme 437 4747 38 &
crashme 47848 4745 123 &
crashme 4747 4747 3463 &
crashme 474 46464 262 &
crashme 37 3644 3723 &
crashme 374 46464 22 &
crashme 3747 464 363 &
crashme 347 4747 44 &
crashme 37374 374 66 &
crashme 3737 474 4444 &

The 4-argument case of crashme could likely do as well as executing
a script.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <excpt.h>
#include <windows.h>
#else
#include <signal.h>
#include <setjmp.h>
#endif
#include <time.h>
#ifdef VMS
#include <processes.h>
#endif

#ifdef pyr
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/vmmac.h>
#define strchr index
#endif

#ifdef linux
#include <unistd.h>
#endif

typedef void (*BADBOY)();

BADBOY badboy;

long nbytes,nseed,ntrys;
long incptr = 0;
long offset = 0;
long next_offset = 0;
long malloc_flag = 0;
unsigned char *the_data;
char *note_buffer;
char *notes;

long verbose_level = 5;

void old_main(),copyright_note(),vfork_main(),badboy_loop();
void record_note();


FILE *logfile = NULL;

void record_note()
{char *logfilename;
 if (!(logfilename = getenv("CRASHLOG"))) return;
 if (!(logfile = fopen(logfilename,
		       (strncmp(note_buffer,"Subprocess",10) == 0)
		       ? "a" : "w")))
   {perror(logfilename);
    return;}
 if (note_buffer[strlen(note_buffer)-1] != '\n')
   strcat(note_buffer,"\n");
 fputs(note_buffer,logfile);
 fclose(logfile);
 logfile = NULL;}

void open_record()
{char *logfilename;
 if (!(logfilename = getenv("CRASHLOG"))) return;
 if (!(logfile = fopen(logfilename,"a")))
   {perror(logfilename);
    return;}}

void close_record()
{if (logfile)
   {fclose(logfile);
    logfile = NULL;}}
 
void note(level)
     long level;
{if (level > verbose_level) return;
 strcat(note_buffer,"\n");
 fputs(note_buffer,stdout);
 if (logfile)
   fputs(note_buffer,logfile);}

#ifndef WIN32
jmp_buf again_buff;
#endif

unsigned char *bad_malloc(n)
     long n;
{unsigned char *data;
 data = (unsigned char *) malloc(n);
#ifdef pyr
 if (mprotect(((int)data/PAGSIZ)*PAGSIZ, (n/PAGSIZ+1)*PAGSIZ,
	      PROT_READ|PROT_WRITE|PROT_EXEC))
   perror("mprotect");
#endif
 return(data);}

#ifndef WIN32

void again_handler(sig)
     int sig;
{char *ss;
 switch(sig)
   {case SIGILL: ss =   " illegal instruction"; break;
#ifdef SIGTRAP
    case SIGTRAP: ss =   " trace trap"; break;
#endif
    case SIGFPE: ss =   " arithmetic exception"; break;
#ifdef SIGBUS
    case SIGBUS: ss =  " bus error"; break;
#endif
    case SIGSEGV: ss =  " segmentation violation"; break;
#ifdef SIGIOT
    case SIGIOT: ss = " IOT instruction"; break;
#endif
#ifdef SIGEMT
    case SIGEMT: ss = " EMT instruction"; break;
#endif
#ifdef SIGALRM
    case SIGALRM: ss = " alarm clock"; break;
#endif
    case SIGINT:  ss = " interrupt"; break;
    default: ss = "";}
 sprintf(notes,"Got signal %d%s",sig,ss);
 note(5);
 longjmp(again_buff,3);}

void my_signal(sig, func)
     int sig;
     void (*func)();
{
#ifndef SA_ONESHOT
 signal(sig, func);
#else
 struct sigaction act;
 act.sa_handler = func;
/* act.sa_mask = SA_NOMASK; */
#ifdef linux
 act.sa_restorer = 0;
#endif /* linux */
 act.sa_flags = SA_NOMASK;
#ifdef SA_RESTART
 act.sa_flags |= SA_RESTART;
#endif
 sigaction(sig, &act, 0);
#endif /* SA_ONESHOT */
}
 
set_up_signals()
{my_signal(SIGILL,again_handler);
#ifdef SIGTRAP
 my_signal(SIGTRAP,again_handler);
#endif
 my_signal(SIGFPE,again_handler);
#ifdef SIGBUS
 my_signal(SIGBUS,again_handler);
#endif
 my_signal(SIGSEGV,again_handler);
#ifdef SIGIOT
 my_signal(SIGIOT,again_handler);
#endif
#ifdef SIGEMT
 my_signal(SIGEMT,again_handler);
#endif
#ifdef SIGALRM
 my_signal(SIGALRM,again_handler);
#endif
 my_signal(SIGINT,again_handler);}

#endif

compute_badboy_1(n)
     long n;
{long j;
 if (malloc_flag == 1)
   the_data = bad_malloc(n);
 for(j=0;j<n;++j) the_data[j] = (rand() >> 7) & 0xFF;
 if (nbytes < 0)
   {sprintf(notes,"Dump of %ld bytes of data",n);
    note(1);
    for(j=0;j<n;++j)
      {fprintf(stdout,"%3d",the_data[j]);
       if ((j % 20) == 19) putc('\n',stdout); else putc(' ',stdout);}
    putc('\n',stdout);}}

void proto_badboy()
{printf("Hello world.\n");}

#if defined(__ALPHA) && defined(VMS) && !defined(NOCASTAWAY)
#include <pdscdef.h>
#endif

BADBOY castaway(dat)
     char *dat;
{
#if defined(VAX) && !defined(NOCASTAWAY)
  /* register save mask avoids bashing our callers locals */
  ((unsigned short *)dat)[0] = 0x0FFC;
#endif
#if defined(__ALPHA) && defined(VMS) && !defined(NOCASTAWAY)
  struct pdscdef *p,*b;
  p = (struct pdscdef *) proto_badboy;
  b = (struct pdscdef *) dat;
  memcpy(b,p,sizeof(struct pdscdef));
  b->pdsc$q_entry[1] = 0;
  b->pdsc$q_entry[0] = (int)&dat[sizeof(struct pdscdef)];
#endif
#if (defined(_IBMR2) || defined(_POWER)) && !defined(NOCASTAWAY)
  struct fdesc {void *fcn_addr; void *toc; void *linkage;} *p,*b;
  p = (struct fdesc *) proto_badboy;
  b = (struct fdesc *) dat;
  memcpy(b,p,sizeof(struct fdesc));
  b->fcn_addr = (void *) &dat[sizeof(struct fdesc)];
#endif
#if defined(__hpux) && defined(__hppa) && !defined(NOCASTAWAY)
  struct fdesc {void *fcn_addr; void *toc;} *p,*b;
  p = (struct fdesc *) proto_badboy;
  b = (struct fdesc *) dat;
  memcpy(b,p,sizeof(struct fdesc));
  b->fcn_addr = (void *) &dat[sizeof(struct fdesc)];
#endif
  return((BADBOY)dat);}

compute_badboy()
{long n;
 n = (nbytes < 0) ? - nbytes : nbytes;
 if (incptr == 0)
   {compute_badboy_1(n);
    badboy = castaway(the_data);}
 else if ((next_offset == 0) || (next_offset > ((n * 90) / 100)))
   {compute_badboy_1(n);
    offset = 0;
    next_offset = offset + incptr;
    badboy = castaway(the_data);}
 else
   {offset = next_offset;
    next_offset = offset + incptr;
    badboy = castaway(&the_data[offset]);}}

/* maybe add this code before invoking badboy. But it didn't seem
   to be needed when using +1000.
   FlushInstructionCache(GetCurrentProcess(),
                         the_data,(nbytes < 0) ? - nbytes : nbytes);
*/

try_one_crash()
{if (nbytes > 0)
   (*badboy)();
 else if (nbytes == 0)
   while(1);}

char *subprocess_ind = "subprocess";
 
main(argc,argv)
     int argc; char **argv;
{long nsubs,hrs,mns,scs,tflag,j,m;
 note_buffer = (char *) malloc(512);
 notes = note_buffer;
 if ((argc == 7) &&
     (strcmp(argv[6],subprocess_ind) == 0))
   {sprintf(note_buffer,"Subprocess %s: ",argv[4]);
    notes = note_buffer + strlen(note_buffer);
    verbose_level = atol(argv[5]);
    sprintf(notes,"starting");
    note(3);
    old_main(4,argv);}
 else if (argc == 4)
   old_main(4,argv);
 else if ((argc == 6) && ((strlen(argv[4]) == 0) ||
			  (strcmp(argv[4],".") == 0)))
   {verbose_level = atol(argv[5]);
    old_main(4,argv);}
 else if ((argc == 5) || (argc == 6))
   {if (argc == 6)
      verbose_level = atol(argv[5]);
    copyright_note(1);
    if (argc < 7)
      m = argc;
    else
      m = 6;
    strcpy(notes,"crashme");
    for(j=1;j<m;++j)
      {strcat(notes," ");
       strcat(notes,argv[j]);}
    note(1);
    record_note();
    if (strchr(argv[4],':'))
      {sscanf(argv[4],"%d:%d:%d",&hrs,&mns,&scs);
       tflag = 1;
       nsubs = (((hrs * 60) + mns) * 60) + scs;
       sprintf(notes,"Subprocess run for %d seconds (%d %02d:%02d:%02d)",
	       nsubs, hrs / 24, hrs % 24,mns,scs);}
    else
      {tflag = 0;
       nsubs = atol(argv[4]);
       sprintf(notes,"Creating %d crashme subprocesses",nsubs);}
    note(1);
    vfork_main(tflag,nsubs,argv[0],argv[1],atol(argv[2]),argv[3]);}
 else
   {sprintf(notes,
	    "crashme [+]<nbytes>[.inc] <srand> <ntrys> [nsub] [verbose]");
    note(0);}}

void copyright_note(n)
     long n;
{sprintf(notes,"Crashme: (c) Copyright 1990-1994 George J. Carrette");
 note(n);
 sprintf(notes,"Version: %s",crashme_version);
 note(n);}

void old_main(argc,argv)
     int argc;
     char **argv;
{char *ptr;
 copyright_note(3);
 nbytes = atol(argv[1]);
 if (ptr = strchr(argv[1],'.'))
   incptr = atol(&ptr[1]);
 if (argv[1][0] == '+') malloc_flag = 1;
 nseed = atol(argv[2]);
 ntrys = atol(argv[3]);
 sprintf(notes,"crashme %s%ld.%d %ld %ld",
	 (malloc_flag == 0) ? "" : "+",nbytes,incptr,nseed,ntrys);
 note(3);
 record_note();
 if (malloc_flag == 0)
   {the_data = bad_malloc((nbytes < 0) ? -nbytes : nbytes);
    badboy = castaway(the_data);
    sprintf(notes,"Badboy at %d. 0x%X",badboy,badboy);
    note(3);}
 srand(nseed);
#ifdef WIN32
 SetErrorMode(SEM_FAILCRITICALERRORS |
	      SEM_NOGPFAULTERRORBOX |
	      SEM_NOOPENFILEERRORBOX);
#endif
 badboy_loop();}

#ifdef WIN32
DWORD exception_filter(DWORD value)
{int sev,cus,res,fac,cod;
 sev = 3 & (value >> 30);
 cus = 1 & (value >> 29);
 res = 1 & (value >> 28);
 fac = 07777 & (value >> 16);
 cod = 0xFFFF & value;
 sprintf(notes,"sev(%d)cus(%d)res(%d)fac(%d)code(%d)",
	 sev,cus,res,fac,cod);
 note(5);
 return(EXCEPTION_EXECUTE_HANDLER);}
#endif

void badboy_loop()
{int i;
 for(i=0;i<ntrys;++i)
   {compute_badboy();
    if (offset)
      sprintf(notes,"try %d, offset %d",i,offset);
    else if (malloc_flag == 1)
      sprintf(notes,"try %d, Badboy at %d. 0x%X",i,badboy,badboy);
    else
      sprintf(notes,"try %d",i);
    note(5);

#ifdef WIN32
    try {try_one_crash();
	 sprintf(notes,"didn't barf!");
	 note(5);}
    except(exception_filter(GetExceptionCode()))
      {}
#else
    if (setjmp(again_buff) == 3)
      {sprintf(notes,"Barfed");
       note(5);}
    else
      {set_up_signals();
       alarm(10);
       try_one_crash();
       sprintf(notes,"didn't barf!");
       note(5);}
#endif
  }}

struct status_list
{long status;
 long count;
 struct status_list *next;};

struct status_list *slist = NULL;

record_status(n)
     long n;
{struct status_list *l;
 for(l=slist;l != NULL; l = l->next)
   if (n == l->status)
     return(++l->count);
 l = (struct status_list *) malloc(sizeof(struct status_list));
 l->count = 1;
 l->status = n;
 l->next = slist;
 slist = l;
 return(1);}

summarize_status()
{struct status_list *l;
 sprintf(notes,"exit status ... number of cases");
 note(2);
 for(l=slist;l != NULL; l = l->next)
   {sprintf(notes,"exit status ... number of cases");
    sprintf(notes,"%11d ... %5d",l->status,l->count);
    note(2);}}

#ifndef WIN32

long monitor_pid = 0;
long monitor_period = 5;
long monitor_limit =  6; /* 30 second limit on a subprocess */
long monitor_count = 0;
long monitor_active = 0;

void monitor_fcn(sig)
     int sig;
{long status;
 my_signal(SIGALRM,monitor_fcn);
 alarm(monitor_period);
 if (monitor_active)
   {++monitor_count;
    if (monitor_count >= monitor_limit)
      {sprintf(notes,"time limit reached on pid %d 0x%X. using kill.",
	       monitor_pid,monitor_pid);
       note(3);
       status = kill(monitor_pid,SIGKILL);
       if (status < 0)
	 {sprintf(notes,"failed to kill process");
	  note(3);}
       monitor_active = 0;}}}

void vfork_main(tflag,nsubs,cmd,nb,sr,nt)
     long tflag,nsubs,sr;
     char *cmd,*nb,*nt;
{long j,pid,n,seq,total_time,dys,hrs,mns,scs;
 int status;
 char arg2[20],arg4[20],arg5[20];
 time_t before_time,after_time;
 if (tflag == 1)
   {seq = 1;
    n = 100000000;}
 else if (nsubs < 0)
   {n = -nsubs;
    seq = 1;}
 else
   {n = nsubs;
    seq = 0;}
 if (seq == 1)
   {my_signal(SIGALRM,monitor_fcn);
    alarm(monitor_period);}
 time(&before_time);
 sprintf(arg5,"%d",verbose_level);
 for(j=0;j<n;++j)
   {sprintf(arg2,"%d",sr+j);
    sprintf(arg4,"%d",j+1);
#ifdef VMS
    status = vfork();
#else
    status = fork();
#endif
    if (status == 0)
      {status = execl(cmd,cmd,nb,arg2,nt,arg4,arg5,subprocess_ind,0);
       if (status == -1)
	 {perror(cmd);
	  exit(1);}}
    else if (status < 0)
      perror(cmd);
    else
      {sprintf(notes,"pid = %d 0x%X (subprocess %d)",status,status,j+1);
       note(3);
       if (seq == 1)
	 {monitor_pid = status;
	  monitor_count = 0;
	  monitor_active = 1;
	  while((pid = wait(&status)) > 0)
	    {monitor_active = 0;
	     sprintf(notes,"pid %d 0x%X exited with status %d",pid,pid,status);
	     note(3);
	     record_status(status);}}
       if (tflag == 1)
	 {time(&after_time);
	  total_time = after_time - before_time;
	  if (total_time >= nsubs)
	    {sprintf(notes,"Time limit reached after run %d",j+1);
	     note(2);
	     break;}}}}
 if (seq == 0)
   while((pid = wait(&status)) > 0)
     {sprintf(notes,"pid %d 0x%X exited with status %d",pid,pid,status);
      note(3);
      record_status(status);}
 time(&after_time);
 total_time = after_time - before_time;
 scs = total_time;
 mns = scs / 60;
 hrs = mns / 60;
 dys = hrs / 24;
 scs = scs % 60;
 mns = mns % 60;
 hrs = hrs % 24;
 open_record();
 sprintf(notes,
	 "Test complete, total real time: %d seconds (%d %02d:%02d:%02d)",
	 total_time,dys,hrs,mns,scs);
 note(1);
 summarize_status();
 close_record();}

#else

void chk_CloseHandle(HANDLE h)
{DWORD err;
 if (CloseHandle(h) == FALSE)
   {err = GetLastError();
    sprintf(notes,"err %d trying to close handle.",err);
    note(3);}}

int maxticks = 100; /* tenths of a second before forced termination
		       of the subprocess */

void vfork_main(tflag,nsubs,cmd,nb,sr,nt)
     long tflag,nsubs,sr;
     char *cmd,*nb,*nt;
{long j,pid,n,seq,total_time,dys,hrs,mns,scs;
 char arg2[20],arg4[20],arg5[20];
 time_t before_time,after_time;
 char cmdbuf[250];
 int nticks;
 PROCESS_INFORMATION pinfo;
 STARTUPINFO sinfo;
 DWORD exit_code,err;
 if (tflag == 1)
   {seq = 1;
    n = 100000000;}
 else if (nsubs < 0)
   {n = -nsubs;
    seq = 1;}
 else
   {n = nsubs;
    seq = 0;}
 /* tflag says this is a timed run. So nsub is the time in seconds.
    seq says to run in sequence, not created nsub processes all
    at once. */
 if (seq == 0)
   {printf("Not implemented. Use [-nsub] or [HH:MM:SS] instead.\n");
    return;}
 time(&before_time);
 sprintf(arg5,"%d",verbose_level);
 for(j=0;j<n;++j)
   {sprintf(arg2,"%d",sr+j);
    sprintf(arg4,"%d",j+1);
    sprintf(cmdbuf,"%s %s %s %s %s %s %s",
	    cmd,nb,arg2,nt,arg4,arg5,subprocess_ind);
    /* kind of dumb that this sinfo structure is a required argument */
    memset(&sinfo,0,sizeof(STARTUPINFO));
    sinfo.cb = sizeof (STARTUPINFO);
    if (CreateProcess(NULL,cmdbuf,NULL,NULL,TRUE,0,NULL,NULL,&sinfo,&pinfo)
	== FALSE)
      {err = GetLastError();
       sprintf(notes,"err %d trying to create process",err);
       continue;}
    sprintf(notes,"pid = %d 0x%X (subprocess %d)",
	    pinfo.dwProcessId,pinfo.dwProcessId,j+1);
    note(3);
    nticks = 0;
    while(1)
      {if (GetExitCodeProcess(pinfo.hProcess,&exit_code) == TRUE)
	 {if (exit_code == STILL_ACTIVE)
	    {if (nticks == maxticks)
	       {sprintf(notes,"time limit reached on pid %d 0x%X. using kill.",
			pinfo.dwProcessId,pinfo.dwProcessId);
		note(3);
		if (TerminateProcess(pinfo.hProcess,
				     APPLICATION_ERROR_MASK |
				     ERROR_SEVERITY_ERROR |
				     1) == FALSE)
		  {err = GetLastError();
		   sprintf(notes,"err %d trying to terminate process.",err);
		   note(3);
		   chk_CloseHandle(pinfo.hProcess);
		   chk_CloseHandle(pinfo.hThread);
		   break;}
		++nticks;}
	    else
	      {++nticks;
	       Sleep(100);}}
	 else
	   {sprintf(notes,"pid %d 0x%X exited with status %d",
		    pinfo.dwProcessId,pinfo.dwProcessId,exit_code);
	    note(3);
	    record_status(exit_code);
	    chk_CloseHandle(pinfo.hProcess);
	    chk_CloseHandle(pinfo.hThread);
	    break;}}
      else
	{err = GetLastError();
	 sprintf(notes,"err %d on GetExitCodeProcess.");
	 note(3);
	 chk_CloseHandle(pinfo.hProcess);
	 chk_CloseHandle(pinfo.hThread);
	 break;}}
    if (tflag == 1)
      {time(&after_time);
       total_time = after_time - before_time;
       if (total_time >= nsubs)
	 {sprintf(notes,"Time limit reached after run %d",j+1);
	  note(2);
	  break;}}}
 time(&after_time);
 total_time = after_time - before_time;
 scs = total_time;
 mns = scs / 60;
 hrs = mns / 60;
 dys = hrs / 24;
 scs = scs % 60;
 mns = mns % 60;
 hrs = hrs % 24;
 open_record();
 sprintf(notes,
	 "Test complete, total real time: %d seconds (%d %02d:%02d:%02d)",
	 total_time,dys,hrs,mns,scs);
 note(1);
 summarize_status();
 open_record();}

#endif

