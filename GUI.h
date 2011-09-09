
/* CapTux
 * Author: darkjoker
 * File  : GUI.h
 * All the GUI-related stuff
 */

#define	MAX_SECS	18

void clear_input (SDL_Surface *screen,int width, int height) {
	int i;
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	memset (screen->pixels+screen->pitch*height,0x00,width);
	for (i=0;i<14;i++) {
		memset (screen->pixels+screen->pitch*(height+1+i),0x00,1);
		memset (screen->pixels+screen->pitch*(height+1+i)+1,0xFF,width-2);
		memset (screen->pixels+screen->pitch*(height+1+i)+width-1,0x00,1);
	}
	memset (screen->pixels+screen->pitch*(height+15),0x00,width);
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_Flip (screen);
}

int print_string (SDL_Surface *screen,char *string, TTF_Font *font, int y, int x, int width) {
	SDL_Color color;
	SDL_Rect dst;
	SDL_Surface *txt;
	int ret;
	if (strlen(string)) {
		color.r=0;
		color.g=0;
		color.b=0;
		txt = TTF_RenderText_Solid (font,string,color);
		dst.x = x;
		dst.y = y;
		dst.w = txt->w;
		dst.h = txt->h;
		if (txt->w+2<=width-2) {
			SDL_BlitSurface(txt,NULL,screen,&dst);
			ret = 1;
		}
		else
			ret = 0;
	}
	SDL_Flip(screen);
	return ret;
}
char *open_window () {
	SDL_Event a;
	SDL_Surface *screen;
	TTF_Font *cour;
	FILE *fp;
	int width,height,i,d,l;
	char *value,line [1024];
	time_t t1,t2;
	__u8 p,*m;
	
	t1 = time (0);
	
	value = malloc (128);

	value[0] = 0;
	
	/* The PPM image is read */
	fp = fopen ("captcha.ppm", "r");
	fgets (line,sizeof(line),fp);
	while (1) {
		fgets (line,sizeof(line),fp);
		if (line[0]>='0' && line[0]<='9')
			break;
	}
	sscanf (line, "%d %d",&width, &height);
	while (1) {
		fgets (line,sizeof(line),fp);
		if (!strcmp (line,"255\n"))
			break;
	}
	m=malloc(width*height);
	memset (m,0,width*height);

	// 0bRRRGGGBB
	/* 8 bit */
	for (d=0;d<width*height;d++) {
		fscanf (fp,"%u",(unsigned int*)&p);
		m[d] |= (7*p/255) << 5;
		fscanf (fp,"%u",(unsigned int*)&p);
		m[d] |= (7*p/255) << 2;
		fscanf (fp,"%u",(unsigned int*)&p);
		m[d] |= (3*p/255);
	}
	SDL_Init(SDL_INIT_VIDEO);
	SDL_putenv("SDL_VIDEO_CENTERED=center");
	SDL_WM_SetCaption("CapTux ~ darkjoker", NULL);
	screen = SDL_SetVideoMode(width,height+16,8,SDL_HWSURFACE|SDL_DOUBLEBUF);
	SDL_EnableUNICODE(1);
	TTF_Init();
	cour = TTF_OpenFont("cour.ttf", 12); 
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	/* The captcha is loaded */
	for (i=0;i<height;i++)
		for (d=0;d<width;d++)
			memset (screen->pixels+i*screen->pitch+d,m[i*width+d],1);
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_Flip(screen);
	clear_input (screen,width,height);
	for (l=1,i=0;l==1;) {
		t2 = time (0);
		if (t2-t1>MAX_SECS)
			l=0;
		while (SDL_PollEvent(&a)) {
			switch (a.type) {
				case SDL_KEYDOWN:
					/* ' ' => '~' characters admitted */
					if (a.key.keysym.unicode>0x19 && a.key.keysym.unicode<0x80) {
						value[i++]=a.key.keysym.unicode;
						value[i]=0;
						clear_input (screen,width,height);						
						if (!print_string (screen,value,cour,height+1,1,width)) {
							value[--i]=0;
							print_string (screen,value,cour,height+1,1,width);
						}
					}
					/* Backspace */
					if (a.key.keysym.unicode==0x08) {
						if (i-1>=0) {
							value[--i]=0;
						}
						clear_input (screen,width,height);
						print_string (screen,value,cour,height+1,1,width);
					}
					/* Enter */
					else if (a.key.keysym.unicode==0x0d) 
						l=0;
					break;
				case SDL_QUIT:
					l=0;
					break;
				default:
					break;
			}
		}
	}
	SDL_Quit ();
	TTF_Quit ();
	return value;
}
