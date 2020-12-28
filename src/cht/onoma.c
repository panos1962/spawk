#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cht.h>

typedef struct {
	char *onomastiki;
	unsigned char olen;	/* used as small int */
	char *geniki;
	unsigned char glen;	/* used as small int */
	char *etiatiki;
	unsigned char elen;	/* used as small int */
} ONOMA;

static ONOMA aren[] = {
"ΌΠΟΥΛΟΣ",	7,	"ΌΠΟΥΛΟΥ",	7,	"ΌΠΟΥΛΟ",	6,
"ΟΎΛΟΣ",	5,	"ΟΎΛΟΥ",	5,	"ΟΎΛΟ",		4,
"ΊΔΗΣ",		4,	"ΊΔΗ",		3,	"ΊΔΗ",		3,
"ΪΔΗΣ",		4,	"ΪΔΗ",		3,	"ΪΔΗ",		3,
"ΆΔΗΣ",		4,	"ΆΔΗ",		4,	"ΆΔΗ",		3,
"ΓΛΟΥ",		4,	"ΓΛΟΥ",		4,	"ΓΛΟΥ",		4,
"ΊΟΥ",		3,	"ΊΟΥ",		3,	"ΊΟΥ",		3,
"ΟΣ",		2,	"ΟΥ",		2,	"Ο",		1,
"ΌΣ",		2,	"ΟΎ",		2,	"Ό",		1,
"ΗΣ",		2,	"Η",		1,	"Η",		1,
"ΉΣ",		2,	"Ή",		1,	"Ή",		1,
"ΕΎΣ",		3,	"ΈΑ",		2,	"ΈΑ",		2,
"ΕΥΣ",		3,	"ΕΑ",		2,	"ΕΑ",		2,
"ΑΣ",		2,	"Α",		1,	"Α",		1,
"ΆΣ",		2,	"Ά",		1,	"Ά",		1,
"ΈΣ",		2,	"Έ",		1,	"Έ",		1,
"ΕΣ",		2,	"Ε",		1,	"Ε",		1,
"ΟΥΛΟΣ",	5,	"ΟΥΛΟΥ",	5,	"ΟΥΛΟ",		4,
"ΙΔΗΣ",		4,	"ΙΔΗ",		3,	"ΙΔΗ",		3,
"ΑΔΗΣ",		4,	"ΑΔΗ",		3,	"ΑΔΗ",		3,
"ΙΟΥ",		3,	"ΙΟΥ",		3,	"ΙΟΥ",		3,
"ΣΥΜΕΏΝ",	6,	"ΣΥΜΕΏΝ",	6,	"ΣΥΜΕΏΝ",	6,
"ΣΥΜΕΩΝ",	6,	"ΣΥΜΕΩΝ",	6,	"ΣΥΜΕΩΝ",	6,
"ΣΑΜΨΏΝ",	6,	"ΣΑΜΨΏΝ",	6,	"ΣΑΜΨΏΝ",	6,
"ΣΑΜΨΩΝ",	6,	"ΣΑΜΨΩΝ",	6,	"ΣΑΜΨΩΝ",	6,
"ΞΕΝΟΦΩΝ",	7,	"ΞΕΝΟΦΩΝΤΑ",	9,	"ΞΕΝΟΦΩΝΤΑ",	9,
"ΣΟΛΟΜΩΝ",	7,	"ΣΟΛΟΜΩΝΤΑ",	9,	"ΣΟΛΟΜΩΝΤΑ",	9,
"ΩΝ",		2,	"ΩΝΑ",		3,	"ΩΝΑ",		3,
"ΏΝ",		2,	"ΏΝΤΑ",		4,	"ΏΝΤΑ",		4,
"όπουλος",	7,	"όπουλου",	7,	"όπουλο",	6,
"ούλος",	5,	"ούλου",	5,	"ούλο",		4,
"ίδης",		4,	"ίδη",		3,	"ίδη",		3,
"ΐδης",		4,	"ΐδη",		3,	"ΐδη",		3,
"άδης",		4,	"άδη",		3,	"άδη",		3,
"γλου",		4,	"γλου",		4,	"γλου",		4,
"ίου",		3,	"ίου",		3,	"ίου",		3,
"ος",		2,	"ου",		2,	"ο",		1,
"ός",		2,	"ού",		2,	"ό",		1,
"ης",		2,	"η",		1,	"η",		1,
"ής",		2,	"ή",		1,	"ή",		1,
"ας",		2,	"α",		1,	"α",		1,
"άς",		2,	"ά",		1,	"ά",		1,
"Συμεών",	6,	"Συμεών",	6,	"Συμεών",	6,
"Σαμψών",	6,	"Σαμψών",	6,	"Σαμψών",	6,
"ων",		2,	"ωνα",		3,	"ωνα",		3,
"ών",		2,	"ώντα",		4,	"ώντα",		4,
};

ONOMA thili[] = {
"Α",		1,	"ΑΣ",		2,	"Α",		1,
"Η",		1,	"ΗΣ",		2,	"Η",		1,
"Ά",		1,	"ΆΣ",		2,	"Ά",		1,
"Ή",		1,	"ΉΣ",		2,	"Ή",		1,
"Ω",		1,	"ΩΣ",		2,	"Ω",		1,
"Ώ",		1,	"ΏΣ",		2,	"Ώ",		1,
"α",		1,	"ας",		2,	"α",		1,
"η",		1,	"ης",		2,	"η",		1,
"ά",		1,	"άς",		2,	"ά",		1,
"ή",		1,	"ής",		2,	"ή",		1,
"ω",		1,	"ως",		2,	"ω",		1,
"ώ",		1,	"ώς",		2,	"ώ",		1,
};

static char *onoma(char *name, int len, ONOMA *at,
	size_t at_n, int ptosi)
{
	int i;
	char *p;
	
	for (i = 0; i < at_n; i++) {
		if (len < at[i].olen)
			continue;

		p = name + (len - at[i].olen);
		if (strcmp(p, at[i].onomastiki))
			continue;

		switch (ptosi) {
		case CHT_PTOSI_GENIKI:
			strcpy(p, at[i].geniki);
			break;
		case CHT_PTOSI_ETIATIKI:
			strcpy(p, at[i].etiatiki);
			break;
		}
		break;
	}
	return name;
}

#define MAX_ONOMA 1024

char *cht_onoma(char *s, char *name, const char filo, const int ptosi)
{
	unsigned int len;
	char buf[MAX_ONOMA];
	char buf_new[MAX_ONOMA];
	register char *p;
	register char *q;
	char *pb;
	int space;
	ONOMA *at;
	size_t at_n;

	if (name == (char *)0)
		return (char *)0;

	switch (ptosi) {
	case CHT_PTOSI_GENIKI:
	case CHT_PTOSI_ETIATIKI:
		break;
	default:
		return (s == ((char *)NULL) ? name : strcpy(s, name));
	}

	switch (filo) {
	case 'Α':
	case 'α':
	case 'Ά':
	case 'ά':
		at = aren;
		at_n = sizeof(aren) / sizeof(*aren);
		break;
	default:
		at = thili;
		at_n = sizeof(thili) / sizeof(*thili);
		break;
	}
		
	/* eat leading spaces/dashes */

	for (p = name; *p; p++) {
		switch (*p) {
		case ' ':
		case '-':
			continue;
		}
		break;
	}

	pb = buf_new;
	for (space = 0, q = buf; *p; p++) {
		switch (*p) {
		case '-':
		case ' ':
			if (space)
				continue;

			*q = 0;
			pb = cht_strcpy(pb, onoma(buf, q - buf,
				at, at_n, ptosi));
			q = buf;
			space = 1;
			continue;
		}

		if (space) {
			*pb++ = '-';
			space = 0;
		}
		*q++ = *p;
	}

	if (q > buf) {
		*q = 0;
		pb = cht_strcpy(pb, onoma(buf, q - buf,
			at, at_n, ptosi));
	}
	*pb = 0;

	if (s == (char *)0)
		return strcpy(name, buf_new);
	return strcpy(s, buf_new);
}
