#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cht.h>

/*
** Η function cht_lexposo δέχεται σαν παραμέτρους ενα character
** pointer και ένα long και επιστρέφει ολογράφως την τιμή του long.
*/

static int lex100(char *, int);
static int olex100(char *, int);

#define add_space(s, p) if (s > p) { *s++ = ' '; }

char *cht_lexposo(char *s, long n)
{
	int dis;
	int ekatom;
	int xiliades;
	int ekatontades;
	char *buf = s;

	if (n == 0L)
		return strcpy(s, "ΜΗΔΕΝ");

	if (n < 0L) {
		strcpy(s, "ΜΕΙΟΝ");
		s += 5;
		n = -n;
	}

	dis = n / 1000000000L;
	n %= 1000000000L;

	ekatom = n / 1000000L;
	n %= 1000000L;

	xiliades = n / 1000L;
	ekatontades = n % 1000L;

	switch (dis) {
	case 0:
		break;
	case 1:
		add_space(s, buf);
		strcpy(s, "ΕΝΑ ΔΙΣΕΚΑΤΟΜΜΥΡΙΟ");
		s += 18;
		break;
	default:
		add_space(s, buf);
		s += olex100(s, dis);
		strcpy(s, " ΔΙΣΕΚΑΤΟΜΜΥΡΙΑ");
		s += 15;
		break;
	}

	switch (ekatom) {
	case 0:
		break;
	case 1:
		add_space(s, buf);
		strcpy(s, "ΕΝΑ ΕΚΑΤΟΜΜΥΡΙΟ");
		s += 15;
		break;
	default:
		add_space(s, buf);
		s += olex100(s, ekatom);
		strcpy(s, " ΕΚΑΤΟΜΜΥΡΙΑ");
		s += 12;
		break;
	}

	switch (xiliades) {
	case 0:
		break;
	case 1:
		add_space(s, buf);
		strcpy(s, "ΧΙΛΙΕΣ");
		s += 6;
		break;
	default:
		add_space(s, buf);
		s += lex100(s, xiliades);
		strcpy(s, " ΧΙΛΙΑΔΕΣ");
		s += 9;
		break;
	}

	add_space(s, buf);
	lex100(s, ekatontades);

	return buf;
}

static int lex100(char *s, int n)
{
	register char *p = s;
	int ekatontades = n / 100;
	int more_100 = n % 100;
	int dekades = more_100 / 10;
	int monades = n % 10;

	switch (ekatontades) {
	case 0:
		*p = 0;
		break;
	case 1:
		if (more_100 > 0) {
			strcpy(p, "ΕΚΑΤΟΝ");
			p += 6;
		}
		else {
			strcpy(p, "ΕΚΑΤΟ");
			p += 5;
		}
		break;
	case 2:
		strcpy(p, "ΔΙΑΚΟΣΙΕΣ");
		p += 9;
		break;
	case 3:
		strcpy(p, "ΤΡΙΑΚΟΣΙΕΣ");
		p += 10;
		break;
	case 4:
		strcpy(p, "ΤΕΤΡΑΚΟΣΙΕΣ");
		p += 11;
		break;
	case 5:
		strcpy(p, "ΠΕΝΤΑΚΟΣΙΕΣ");
		p += 11;
		break;
	case 6:
		strcpy(p, "ΕΞΑΚΟΣΙΕΣ");
		p += 9;
		break;
	case 7:
		strcpy(p, "ΕΠΤΑΚΟΣΙΕΣ");
		p += 10;
		break;
	case 8:
		strcpy(p, "ΟΚΤΑΚΟΣΙΕΣ");
		p+= 10;
		break;
	case 9:
		strcpy(p, "ΕΝΝΙΑΚΟΣΙΕΣ");
		p += 11;
		break;
	}

	if (more_100)
		*p++ = ' ';

	switch (more_100) {
	case 11:
		strcpy(p, "ΕΝΤΕΚΑ");
		p += 6;
		break;
	case 12:
		strcpy(p, "ΔΩΔΕΚΑ");
		p += 6;
		break;
	default:
		switch (dekades) {
		case 1:
			strcpy(p, "ΔΕΚΑ");
			p += 4;
			break;
		case 2:
			strcpy(p, "ΕΙΚΟΣΙ");
			p += 6;
			break;
		case 3:
			strcpy(p, "ΤΡΙΑΝΤΑ");
			p += 7;
			break;
		case 4:
			strcpy(p, "ΣΑΡΑΝΤΑ");
			p += 7;
			break;
		case 5:
			strcpy(p, "ΠΕΝΗΝΤΑ");
			p += 7;
			break;
		case 6:
			strcpy(p, "ΕΞΗΝΤΑ");
			p += 6;
			break;
		case 7:
			strcpy(p, "ΕΒΔΟΜΗΝΤΑ");
			p += 9;
			break;
		case 8:
			strcpy(p, "ΟΓΔΟΝΤΑ");
			p += 7;
			break;
		case 9:
			strcpy(p, "ΕΝΕΝΗΝΤΑ");
			p += 8;
			break;
		}

		if (monades)
			*p++ = ' ';

		switch (monades) {
		case 1:
			strcpy(p, "ΜΙΑ");
			p += 3;
			break;
		case 2:
			strcpy(p, "ΔΥΟ");
			p += 3;
			break;
		case 3:
			strcpy(p, "ΤΡΕΙΣ");
			p += 5;
			break;
		case 4:
			strcpy(p, "ΤΕΣΣΕΡΙΣ");
			p += 8;
			break;
		case 5:
			strcpy(p, "ΠΕΝΤΕ");
			p += 5;
			break;
		case 6:
			strcpy(p, "ΕΞΗ");
			p += 3;
			break;
		case 7:
			strcpy(p, "ΕΠΤΑ");
			p += 4;
			break;
		case 8:
			strcpy(p, "ΟΚΤΩ");
			p += 4;
			break;
		case 9:
			strcpy(p, "ΕΝΝΕΑ");
			p += 5;
			break;
		}
	}

	return p - s;
}

static int olex100(char *s, int n)
{
	register char *p = s;
	int ekatontades = n / 100;
	int more_100 = n % 100;
	int dekades = more_100 / 10;
	int monades = n % 10;

	switch (ekatontades) {
	case 0:
		*p = 0;
		break;
	case 1:
		if (more_100 > 0) {
			strcpy(p, "ΕΚΑΤΟΝ");
			p += 6;
		}
		else {
			strcpy(p, "ΕΚΑΤΟ");
			p += 5;
		}
		break;
	case 2:
		strcpy(p, "ΔΙΑΚΟΣΙΑ");
		p += 8;
		break;
	case 3:
		strcpy(p, "ΤΡΙΑΚΟΣΙΑ");
		p += 9;
		break;
	case 4:
		strcpy(p, "ΤΕΤΡΑΚΟΣΙΑ");
		p += 10;
		break;
	case 5:
		strcpy(p, "ΠΕΝΤΑΚΟΣΙΑ");
		p += 10;
		break;
	case 6:
		strcpy(p, "ΕΞΑΚΟΣΙΑ");
		p += 8;
		break;
	case 7:
		strcpy(p, "ΕΠΤΑΚΟΣΙΑ");
		p += 9;
		break;
	case 8:
		strcpy(p, "ΟΚΤΑΚΟΣΙΑ");
		p+= 9;
		break;
	case 9:
		strcpy(p, "ΕΝΝΙΑΚΟΣΙΑ");
		p += 10;
		break;
	}
	if (more_100) *p++ = ' ';
	switch (more_100) {
	case 11:
		strcpy(p, "ΕΝΤΕΚΑ");
		p += 6;
		break;
	case 12:
		strcpy(p, "ΔΩΔΕΚΑ");
		p += 6;
		break;
	default:
		switch (dekades) {
		case 1:
			strcpy(p, "ΔΕΚΑ");
			p += 4;
			break;
		case 2:
			strcpy(p, "ΕΙΚΟΣΙ");
			p += 6;
			break;
		case 3:
			strcpy(p, "ΤΡΙΑΝΤΑ");
			p += 7;
			break;
		case 4:
			strcpy(p, "ΣΑΡΑΝΤΑ");
			p += 7;
			break;
		case 5:
			strcpy(p, "ΠΕΝΗΝΤΑ");
			p += 7;
			break;
		case 6:
			strcpy(p, "ΕΞΗΝΤΑ");
			p += 6;
			break;
		case 7:
			strcpy(p, "ΕΒΔΟΜΗΝΤΑ");
			p += 9;
			break;
		case 8:
			strcpy(p, "ΟΓΔΟΝΤΑ");
			p += 7;
			break;
		case 9:
			strcpy(p, "ENENHNTA");
			p += 8;
			break;
		}
		if (monades) *p++ = ' ';
		switch (monades) {
		case 1:
			strcpy(p, "ΕΝΑ");
			p += 3;
			break;
		case 2:
			strcpy(p, "ΔΥΟ");
			p += 3;
			break;
		case 3:
			strcpy(p, "ΤΡΙΑ");
			p += 4;
			break;
		case 4:
			strcpy(p, "ΤΕΣΣΕΡΑ");
			p += 7;
			break;
		case 5:
			strcpy(p, "ΠΕΝΤΕ");
			p += 5;
			break;
		case 6:
			strcpy(p, "ΕΞΗ");
			p += 3;
			break;
		case 7:
			strcpy(p, "ΕΠΤΑ");
			p += 4;
			break;
		case 8:
			strcpy(p, "ΟΚΤΩ");
			p += 4;
			break;
		case 9:
			strcpy(p, "ΕΝΝΕΑ");
			p += 5;
			break;
		}
	}
	return p - s;
}
