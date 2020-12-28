#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cht.h>

static char *addstr(char *s, char *p)
{
	while (*p)
		*s++ = *p++;

	*s = ' ';
	return s + 1;
}

static char *oudetero100(char *s, int n)
{
	int ekatontades = n / 100;
	int more_100 = n % 100;
	int dekades = more_100 / 10;
	int monades = n % 10;

	if (n == 100)
		return addstr(s, "ΕΚΑΤΟ");

	switch (ekatontades) {
	case 1:
		s = addstr(s, "ΕΚΑΤΟΝ");
		break;
	case 2:
		s = addstr(s, "ΔΙΑΚΟΣΙΑ");
		break;
	case 3:
		s = addstr(s, "ΤΡΙΑΚΟΣΙΑ");
		break;
	case 4:
		s = addstr(s, "ΤΕΤΡΑΚΟΣΙΑ");
		break;
	case 5:
		s = addstr(s, "ΠΕΝΤΑΚΟΣΙΑ");
		break;
	case 6:
		s = addstr(s, "ΕΞΑΚΟΣΙΑ");
		break;
	case 7:
		s = addstr(s, "ΕΠΤΑΚΟΣΙΑ");
		break;
	case 8:
		s = addstr(s, "ΟΚΤΑΚΟΣΙΑ");
		break;
	case 9:
		s = addstr(s, "ΕΝΝΙΑΚΟΣΙΑ");
		break;
	}

	switch (more_100) {
	case 11:
		s = addstr(s, "ΕΝΤΕΚΑ");
		break;
	case 12:
		s = addstr(s, "ΔΩΔΕΚΑ");
		break;
	default:
		switch (dekades) {
		case 1:
			s = addstr(s, "ΔΕΚΑ");
			break;
		case 2:
			s = addstr(s, "ΕΙΚΟΣΙ");
			break;
		case 3:
			s = addstr(s, "ΤΡΙΑΝΤΑ");
			break;
		case 4:
			s = addstr(s, "ΣΑΡΑΝΤΑ");
			break;
		case 5:
			s = addstr(s, "ΠΕΝΗΝΤΑ");
			break;
		case 6:
			s = addstr(s, "ΕΞΗΝΤΑ");
			break;
		case 7:
			s = addstr(s, "ΕΒΔΟΜΗΝΤΑ");
			break;
		case 8:
			s = addstr(s, "ΟΓΔΟΝΤΑ");
			break;
		case 9:
			s = addstr(s, "ΕΝΕΝΗΝΤΑ");
			break;
		}

		switch (monades) {
		case 1:
			s = addstr(s, "ΕΝΑ");
			break;
		case 2:
			s = addstr(s, "ΔΥΟ");
			break;
		case 3:
			s = addstr(s, "ΤΡΙΑ");
			break;
		case 4:
			s = addstr(s, "ΤΕΣΣΕΡΑ");
			break;
		case 5:
			s = addstr(s, "ΠΕΝΤΕ");
			break;
		case 6:
			s = addstr(s, "ΕΞΙ");
			break;
		case 7:
			s = addstr(s, "ΕΠΤΑ");
			break;
		case 8:
			s = addstr(s, "ΟΚΤΩ");
			break;
		case 9:
			s = addstr(s, "ΕΝΝΕΑ");
			break;
		}
	}

	return s;
}

static char *thiliko100(char *s, int n)
{
	int ekatontades = n / 100;
	int more_100 = n % 100;
	int dekades = more_100 / 10;
	int monades = n % 10;

	if (n == 100)
		return addstr(s, "ΕΚΑΤΟ");

	switch (ekatontades) {
	case 1:
		s = addstr(s, "ΕΚΑΤΟΝ");
		break;
	case 2:
		s = addstr(s, "ΔΙΑΚΟΣΙΕΣ");
		break;
	case 3:
		s = addstr(s, "ΤΡΙΑΚΟΣΙΕΣ");
		break;
	case 4:
		s = addstr(s, "ΤΕΤΡΑΚΟΣΙΕΣ");
		break;
	case 5:
		s = addstr(s, "ΠΕΝΤΑΚΟΣΙΕΣ");
		break;
	case 6:
		s = addstr(s, "ΕΞΑΚΟΣΙΕΣ");
		break;
	case 7:
		s = addstr(s, "ΕΠΤΑΚΟΣΙΕΣ");
		break;
	case 8:
		s = addstr(s, "ΟΚΤΑΚΟΣΙΕΣ");
		break;
	case 9:
		s = addstr(s, "ΕΝΝΙΑΚΟΣΙΕΣ");
		break;
	}

	switch (more_100) {
	case 11:
		s = addstr(s, "ΕΝΤΕΚΑ");
		break;
	case 12:
		s = addstr(s, "ΔΩΔΕΚΑ");
		break;
	default:
		switch (dekades) {
		case 1:
			s = addstr(s, "ΔΕΚΑ");
			break;
		case 2:
			s = addstr(s, "ΕΙΚΟΣΙ");
			break;
		case 3:
			s = addstr(s, "ΤΡΙΑΝΤΑ");
			break;
		case 4:
			s = addstr(s, "ΣΑΡΑΝΤΑ");
			break;
		case 5:
			s = addstr(s, "ΠΕΝΗΝΤΑ");
			break;
		case 6:
			s = addstr(s, "ΕΞΗΝΤΑ");
			break;
		case 7:
			s = addstr(s, "ΕΒΔΟΜΗΝΤΑ");
			break;
		case 8:
			s = addstr(s, "ΟΓΔΟΝΤΑ");
			break;
		case 9:
			s = addstr(s, "ΕΝΕΝΗΝΤΑ");
			break;
		}

		switch (monades) {
		case 1:
			s = addstr(s, "ΜΙΑ");
			break;
		case 2:
			s = addstr(s, "ΔΥΟ");
			break;
		case 3:
			s = addstr(s, "ΤΡΕΙΣ");
			break;
		case 4:
			s = addstr(s, "ΤΕΣΣΕΡΙΣ");
			break;
		case 5:
			s = addstr(s, "ΠΕΝΤΕ");
			break;
		case 6:
			s = addstr(s, "ΕΞΙ");
			break;
		case 7:
			s = addstr(s, "ΕΠΤΑ");
			break;
		case 8:
			s = addstr(s, "ΟΚΤΩ");
			break;
		case 9:
			s = addstr(s, "ΕΝΝΕΑ");
			break;
		}
	}

	return s;
}

char *cht_spellevro(char *s, double x)
{
	char buf[1024];
	char *p = s;
	int c;
	int trisekatomiria;
	int disekatomiria;
	int ekatomiria;
	int xiliades;
	int monades;
	int lepta;
	int miden = 1;

	if ((x < -99999999999999999.0) || (x > 99999999999999999.0)) {
		cht_error(NULL);
		fprintf(stderr, "spellevro: %.2lf: huge amount\n", x);
		sprintf(s, "%.2lf ΕΥΡΩ", x);
		return s;
	}

	sprintf(buf, "%018.0lf", x);
	if (*buf == '-')
		p = addstr(p, "ΜΕΙΟΝ");

	c = buf[4];
	buf[4] = 0;
	trisekatomiria = atoi(buf + 1);
	buf[4] = c;

	c = buf[7];
	buf[7] = 0;
	disekatomiria = atoi(buf + 4);
	buf[7] = c;

	c = buf[10];
	buf[10] = 0;
	ekatomiria = atoi(buf + 7);
	buf[10] = c;

	c = buf[13];
	buf[13] = 0;
	xiliades = atoi(buf + 10);
	buf[13] = c;

	c = buf[16];
	buf[16] = 0;
	monades = atoi(buf + 13);
	buf[16] = c;

	lepta = atoi(buf + 16);

	if (trisekatomiria) {
		miden = 0;
		p = oudetero100(p, trisekatomiria);
		p = addstr(p, "ΤΡΙΣΕΚΑΤΟΜΜΥΡΙ");
		if (trisekatomiria == 1)
			p = addstr(p - 1, "Ο");
		else
			p = addstr(p - 1, "Α");
	}

	if (disekatomiria) {
		miden = 0;
		p = oudetero100(p, disekatomiria);
		p = addstr(p, "ΔΙΣΕΚΑΤΟΜΜΥΡΙ");
		if (disekatomiria == 1)
			p = addstr(p - 1, "Ο");
		else
			p = addstr(p - 1, "Α");
	}

	if (ekatomiria) {
		miden = 0;
		p = oudetero100(p, ekatomiria);
		p = addstr(p, "ΕΚΑΤΟΜΜΥΡΙ");
		if (ekatomiria == 1)
			p = addstr(p - 1, "Ο");
		else
			p = addstr(p - 1, "Α");
	}

	if (xiliades) {
		miden = 0;
		if (xiliades == 1)
			p = addstr(p, "ΧΙΛΙΑ");
		else {
			p = thiliko100(p, xiliades);
			p = addstr(p, "ΧΙΛΙΑΔΕΣ");
		}
	}

	if (monades) {
		miden = 0;
		p = oudetero100(p, monades);
	}

	if (!miden) {
		p = addstr(p, "ΕΥΡΩ");
		if (lepta)
			p = addstr(p, "ΚΑΙ");
	}
	else if (!lepta)
		p = addstr(p, "ΜΗΔΕΝ ΕΥΡΩ");

	if (lepta) {
		p = oudetero100(p, lepta);
		p = addstr(p, "ΛΕΠΤ");
		if (lepta == 1)
			p = addstr(p - 1, "Ο");
		else
			p = addstr(p - 1, "Α");
	}

RETURN:
	*(--p) = 0;
	return s;
}
