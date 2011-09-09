
/* CapTux
 * Author: darkjoker
 * File  : main.c
 * The core
 */

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <SDL/SDL_ttf.h>

char username[128],password[128];
int hidden;

#include "GUI.h"
#include "base64.h"
#include "captrad.h"
#include "inutils.h"

int main (int argc, char *argv[]) {
	char *value,*ticket;
	int i;
	signal(SIGQUIT, quit_sig);
	/* username and/or password missing */
	if (argc<3)
		usage (argv[0]);
	/* hidden mode */
	if (argc==4 && !strcmp (argv[3],"--hidden"))
		hidden=1;
	else
		hidden=0;
	strncpy (username,argv[1],128);
	strncpy (password,argv[2],128);
	if (hidden) 
		if (fork())
			return 0;
	printf ("Use 'kill -QUIT %d' to exit.\n",getpid());
	while (1) {
		ticket = enqueue ();
		if (ticket!=NULL) {
			convert ();
			value = open_window ();
			answer (ticket,value);
			free (value);
		}
		else
			dequeue ();
	}
	return 0;
}
