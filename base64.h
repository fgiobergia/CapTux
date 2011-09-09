
/* CapTux
 * Author: darkjoker
 * File  : base64.h
 * base64_decode () only (base64_encode () is too mainstream)
 */

__u8 val (char ch) {
	switch (ch) {
		case '+':
			return 62;
			break;
		case '/':
			return 63;
			break;
		default:
			if (ch>='a')
				return ch-'a'+26;
			else if (ch>='A')
				return ch-'A';
			else
				return ch-'0'+52;
			break;
	}
}

__u8 *base64_decode (char *base64, int *len) {
	__u8 *decoded, x;
	decoded = malloc (strlen(base64)*6/8+10);
	int i,d,a;
	i=0;
	for (i=0,d=0;base64[i]!=0;) {
		decoded [d] = (val(base64[i++]) << 2) & 0xFC;  // 0b11111100
		if(base64[i]==0||base64[i]=='=') break;
		decoded [d++] |= (val(base64[i]) >> 4) & 0x03; // 0b00000011
		decoded [d] = (val(base64[i++])<<4) & 0xF0;    // 0b11110000
		if(base64[i]==0||base64[i]=='=') break;
		decoded [d++] |= (val(base64[i])>>2) & 0x0F;   // 0b00001111
		decoded [d] = (val(base64[i++])<<6) & 0xC0;    // 0b11000000
		if(base64[i]==0||base64[i]=='=') break;
		decoded [d++] |= val(base64[i++]);
	}
	*len = d;
	return decoded;
}
