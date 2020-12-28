/*
** Η function arenthili δέχεται σαν παράμετρο ένα όνομα
** αρσενικού γένους και επιστρέφει το όνομα σε θηλυκό γένος.
** Το όνομα αποθηκεύεται σε διεύθυνση την οποία περνάμε σαν
** πρώτη παράμετρο και αν αυτή είναι null, τότε χρησιμοποιείται
** το ίδιο το όνομα το οποίο περάσαμε σαν παράμετρο (αλλοιώνεται).
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cht.h>

static struct {
	char *aren;
	unsigned char alen;	/* used as small int */
	char *thili;
	unsigned char tlen;	/* used as small int */
} arth[] = {
	"ΌΠΟΥΛΟΣ",	7,	"ΟΠΟΎΛΟΥ",	7,
	"ΟΎΛΟΣ",	5,	"ΟΎΛΟΥ",	5,
	"ΊΔΗΣ",		4,	"ΊΔΟΥ",		4,
	"ΪΔΗΣ",		4,	"ΪΔΟΥ",		4,
	"ΆΔΗΣ",		4,	"ΆΔΟΥ",		4,
	"ΓΛΟΥ",		4,	"ΓΛΟΥ",		4,
	"ΊΟΥ",		3,	"ΊΟΥ",		3,
	"ΟΣ",		2,	"ΟΥ",		2,
	"ΌΣ",		2,	"ΟΎ",		2,
	"ΗΣ",		2,	"Η",		1,
	"ΉΣ",		2,	"Ή",		1,
	"ΑΣ",		2,	"Α",		1,
	"ΆΣ",		2,	"Ά",		1,
	"ΟΥΛΟΣ",	5,	"ΟΥΛΟΥ",	5,
	"ΙΔΗΣ",		4,	"ΙΔΟΥ",		4,
	"ΑΔΗΣ",		4,	"ΑΔΟΥ",		4,
	"ΙΟΥ",		3,	"ΙΟΥ",		3,
	"ΑΣ",		2,	"Α",		1,
	"όπουλος",	7,	"οπούλου",	7,
	"ούλος",	5,	"ούλου",	5,
	"ίδης",		4,	"ίδου",		4,
	"ΐδης",		4,	"ΐδου",		4,
	"άδης",		4,	"άδου",		4,
	"γλου",		4,	"γλου",		4,
	"ίου",		3,	"ίου",		3,
	"ος",		2,	"ου",		2,
	"ός",		2,	"ού",		2,
	"ης",		2,	"η",		1,
	"ής",		2,	"ή",		1,
	"ας",		2,	"α",		1,
	"άς",		2,	"ά",		1,
};

char *cht_arenthili(char *s, char *name)
{
	unsigned int len;
	int i;
	char *p;

	if (name == (char *)0)
		return (char *)0;

	if ((len = strlen(name)) == 0) {
		if (s == (char *)0)
			return name;
		else {
			*s = 0;
			return s;
		}
	}

	for (i = 0; i < sizeof(arth) / sizeof(*arth); i++) {
		if (len < arth[i].alen)
			continue;
		p = name + (len - arth[i].alen);
		if (strcmp(p, arth[i].aren))
			continue;
		len -= arth[i].alen;
		if (s == (char *)0) {
			strcpy(name + len, arth[i].thili);
			return name;
		}
		memcpy(s, name, len);
		strcpy(s + len, arth[i].thili);
		return s;
	}

	if (s == (char *)0)
		return name;
	else
		return strcpy(s, name);
}

char *cht_thiliaren(char *s, char *name)
{
	unsigned int len;
	int i;
	char *p;

	if (name == (char *)0)
		return (char *)0;

	if ((len = strlen(name)) == 0) {
		if (s == (char *)0)
			return name;
		else {
			*s = 0;
			return s;
		}
	}

	for (i = 0; i < sizeof(arth) / sizeof(*arth); i++) {
		if (len < arth[i].tlen)
			continue;
		p = name + (len - arth[i].tlen);
		if (strcmp(p, arth[i].thili))
			continue;
		len -= arth[i].tlen;
		if (s == (char *)0) {
			strcpy(name + len, arth[i].aren);
			return name;
		}
		memcpy(s, name, len);
		strcpy(s + len, arth[i].aren);
		return s;
	}
	if (s == (char *)0)
		return name;
	else
		return strcpy(s, name);
}
