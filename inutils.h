
/* CapTux
 * Author: darkjoker
 * File  : inutils.h
 * Just some functions
 */

/* Make sure 'convert' is available! */
void convert () {
	system ("convert -compress None captcha.jpeg captcha.ppm");
}

void usage (char *exe) {
	printf (	"CapTux ~ darkjoker\n"
			"Usage: %s <username> <password|passkey> [--hidden]\n", exe);
	exit (0);
}

void quit_sig (int signum) {
	if (signum==SIGQUIT) {
		dequeue ();
		if (!hidden) printf ("Bye bye :)\n");
		exit (0);
	}
}

