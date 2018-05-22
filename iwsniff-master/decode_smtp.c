/*
  decode_smtp.c

  Simple Mail Transfer Protocol.
  
  Copyright (c) 2000 Dug Song <dugsong@monkey.org>
 
  $Id: decode_smtp.c,v 1.1.1.1 2004-07-30 11:36:18 cmn Exp $
*/


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "base64.h"
#include "decode.h"
#include "utils.h"

int
decode_smtp(u_char *buf, int len, u_char *obuf, int olen)
{
	char *p;
	int i, j, login = 0;
	
	obuf[0] = '\0';
	
	for (p = strtok(buf, "\r\n"); p != NULL; p = strtok(NULL, "\r\n")) {
		if (login == 1) {
			strlcat(obuf, p, olen);
			i = base64_pton(p, p, strlen(p));
			p[i] = '\0';
			j = strlen(obuf);
			snprintf(obuf + j, olen - j, " [%s]\n", p);
			login = 0;
		}
		else if (strncmp(p, "AUTH LOGIN ", 11) == 0) {
			strlcat(obuf, p, olen);
			p += 11;
			i = base64_pton(p, p, strlen(p));
			p[i] = '\0';
			j = strlen(obuf);
			snprintf(obuf + j, olen - j, " [%s]\n", p);
			login = 1;
		}
		else if (strncmp(p, "MAIL ", 5) == 0 ||
			 strncmp(p, "RCPT ", 5) == 0 ||
			 strncmp(p, "DATA", 4) == 0) {
			break;
		}
	}
	return (strlen(obuf));
}
