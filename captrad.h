
/* CapTux
 * Author: darkjoker
 * File  : captrad.h
 * Library for captchatrader
 */

#define	IP_ADDR	"64.85.162.106"

/* Sends an HTTP request to the server and returns the reply */
char *send_HTTP_req (char *req, int *diff) {
	struct sockaddr_in sock;
	char rcv,*buf,*ptr;
	int sd,i;
	sd = socket (AF_INET, SOCK_STREAM, 0);
	sock.sin_family = AF_INET;
	sock.sin_port = htons (80);
	sock.sin_addr.s_addr = inet_addr (IP_ADDR);
	connect (sd, (struct sockaddr*)&sock,sizeof(sock));
	send (sd, req, strlen (req), 0);
	i=0;
	buf=malloc(1);
	while (recv (sd, &rcv, 1, 0)==1) {
		buf = realloc (buf, ++i);
		buf [i-1] = rcv;
	}
	buf = realloc (buf, i+10);
	buf [i-1]=0;
	close (sd);
	ptr=buf;
	buf = strchr (strstr (buf, "\r\n\r\n"), '[')+1;
	buf [strchr(buf,']')-buf]=0;
	i=buf-ptr;
	*diff=i;
	return buf;
}

/* http://captchatrader.com/documentation/enqueue */
char *enqueue (/*char *username, char *password*/ void) {
/*	http://api.captchatrader.com/enqueue/username:[username]/password:[password]/ */
	char *req, *b64,*rep, *ticket;
	int i,d,diff;
	__u8 *dec;
	FILE *fp;
	req = malloc (105+strlen(username)+strlen(password));
	sprintf (req,	"GET /enqueue/username:%s/password:%s/ HTTP/1.1\r\n"
			"Host: api.captchatrader.com\r\n"
			"Connection: Close\r\n\r\n",username,password);
	rep = send_HTTP_req (req,&diff);
	ticket = malloc (strchr(rep,',')-rep+1);
	strncpy (ticket, rep, strchr(rep,',')-rep);
	ticket[strchr(rep,',')-rep]=0;
	if (!strncmp(ticket,"-1",2)) {
		if ((!strcmp (strchr(rep,'"'),"\"INVALID USER\"")) || (!strcmp (strchr(rep,'"'),"\"INVALID PARAMETERS\""))) {
			if (!hidden) printf ("%s\n",strchr(rep,'"'));
			free (rep-diff);
			free (req);
			exit (0);
		}
		if (!hidden) printf ("%s\n",strchr(rep,'"'));
		free (rep-diff);
		free (req);
		return NULL;
	}
	fp = fopen ("captcha.jpeg","wb");
	b64 = strstr (rep,"base64,")+7;
	b64[strlen(b64)-1]=0;
	dec = base64_decode (b64,&d);
	for (i=0;i<d;i++)
		fputc (dec[i],fp);
	fclose (fp);
	free (req);
	free (rep-diff);
	free (dec);
	return ticket;
}

/* http://captchatrader.com/documentation/answer */
void answer (/*char *username, char *password, */char *ticket, char *value) {
/*	http://api.captchatrader.com/answer/ (username, password, ticket, value via POST) */
	char *req,*rep;
	int len,diff;
	len = strlen(username)+strlen(password)+strlen(ticket)+strlen(value);
	req = malloc (189+len);
	sprintf (req,	"POST /answer/ HTTP/1.1\r\n"
			"Host: api.captchatrader.com\r\n"
			"Connection: Close\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: % 2d\r\n\r\n"
			"username=%s&password=%s&ticket=%s&value=%s",34+len,username,password,ticket,value);
	rep = send_HTTP_req (req,&diff);
	if (rep[0]!='0') {
		if ((!strcmp (strchr(rep,'"'),"\"INVALID USER\"")) || (!strcmp (strchr(rep,'"'),"\"INVALID PARAMETERS\""))) {
			if (!hidden) printf ("%s\n",strchr(rep,'"'));
			free (rep-diff);
			free (req);
			exit (0);
		}
		printf ("%s\n",strchr(rep,'"'));
	}
	free (rep-diff);
	free (req);
	unlink ("captcha.jpeg");
	unlink ("captcha.ppm");
}

/* http://captchatrader.com/documentation/dequeue */
void dequeue (/*char *username, char *password*/ void) {
/*	http://api.captchatrader.com/dequeue/ (username, password via POST) */
	char *req, *rep;
	int len,diff;
	len = strlen(username)+strlen(password);
	req = malloc (175+len);
	sprintf (req,	"POST /dequeue/ HTTP/1.1\r\n"
			"Host: api.captchatrader.com\r\n"
			"Connection: Close\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: % 2d\r\n\r\n"
			"username=%s&password=%s",len+19,username,password);
	rep = send_HTTP_req (req,&diff);
	free (rep-diff);
	free (req);
}

/* http://captchatrader.com/documentation/get_wait_time */
int get_wait_time (/*char *username, char *password*/ void) {
/*	http://api.captchatrader.com/get_wait_time/username:[username]/password:[password]/ */
	char *req,*rep;
	int d,i,k,len,diff;
	len = strlen(username)+strlen(password);
	req = malloc (111+len);
	sprintf (req,	"GET /get_wait_time/username:%s/password:%s/ HTTP/1.1\r\n"
			"Host: api.captchatrader.com\r\n"
			"Connection: Close\r\n\r\n",username,password);
	rep = send_HTTP_req (req,&diff);
	sscanf (rep,"%d",&i);
	if (i<0) {
		if (!hidden) printf ("\"INVALID USER\"\n");
		free (rep-diff);
		free (req);
		exit (0);
	}
	else 
		sscanf (rep,"%d,%d,%d",&k,&d,&i);
	free (rep-diff);
	free (req);
	return i;
}
