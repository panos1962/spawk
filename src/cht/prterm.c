#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <termio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cht.h>

/*
** Η function cht_prterm χρησιμοποιείται στο transparent print.
** Δέχεται σαν παράμετρο έναν ακέραιο, ο οποίος μπορεί να
** είναι διάφορος του 0, οπότε έχουμε κλείδωμα του πληκτρολογίου,
** cook mode στη γραμμή και άνοιγμα της βοηθητικής πόρτας,
** ενώ αν είναι 0, τότε έχουμε κλείσιμο της βοηθητικής πόρτας,
** raw mode στη γραμμή και ξεκλείδωμα του πληκτρολογίου.
** Για τα escape sequenses που εκτελούν τις παραπάνω εργασίες
** χρησιμοποιείται η environment variable TERM και το file
** /cht/lib/prterm το οποίο έχει το παρακάτων format:
**
**	terminal_type lock aux_open unlock aux_close
**	terminal_type lock aux_open unlock aux_close
**	terminal_type lock aux_open unlock aux_close
**	...
**
** όπου τα sequences είναι κλείδωμα πληκτρολογίου,
** άνοιγμα βοηθητικής πόρτας, ξεκλείδωμα πληκτρολογίου
** και κλείσιμο βοηθητικής πόρτας.
** Τα sequences μπορούν να γραφουν με χρήση του παρακάτω πρωτοκόλλου:
**
**	\E	escape
**	^c	control character c
**	<num>	decimal character code
**	\\	backslash
**	\^	carat
**	\<	< character
**
** Η function επιστρέφει 0 αν όλα πάνε καλά και τιμή διάφορη
** του μηδενός αν υπάρχει πρόβλημα.
**
** Όταν εκτελώ slave printing μαρκάρω το τερματικό που εκτελεί
** την εκτύπωση στο directory /local/slave_now/dev/pts. Αυτό
** το κάνω για να αποφύγω εκτύπωση μηνυμάτων που έρχονται από
** kouskous και chtwall. Έτσι, τα παραπάνω προγράμματα ελέγχουν
** το συγκεκριμένο directory και αν βρούν εκεί file για κάποιο
** τερματικό, καταγράφουν το μήνυμα εκεί και όταν τελειώσει
** η εκτύπωση ειδοποιούν το χρήστη για το μήνυμα που είχε.
*/

static int decode(register char *q, char *seq)
{
	int magic = 0;
	int control = 0;
	register char *p;

	for (p = seq; *p; p++) {
		if (magic) {
			switch (*p) {
			case 'E':	/* escape */
				*q++ = 033;
				break;
			case '\\':	/* backslash */
				*q++ = '\\';
				break;
			case '<':
				*q++ = '<';
				break;
			case '^':
				*q++ = '^';
				break;
			default:
				*q++ = *p;
				break;
			}
			magic = 0;
			continue;
		}

		if (control) {
			if (islower(*p))
				*q++ = *p - 'a' + 1;
			else if (isupper(*p))
				*q++ = *p - 'A' + 1;
			else {
				cht_error("prterm: "
					"bad control character");
				return 1;
			}
			control = 0;
			continue;
		}

		switch (*p) {
		case '\\':
			magic = 1;
			continue;
		case '^':
			control = 1;
			continue;
		}

		if (*p != '<') {
			*q++ = *p;
			continue;
		}

		if (sscanf(p + 1, "%d>", q) != 1) {
			cht_error("prterm: bad character code");
			return 1;
		}

		for (q++, p += 2; *p != '>';)
			p++;
	}
	*q = 0;
	return 0;
}

/*
** Η function mark_slave δημιουργεί mark file στο directory
** /local/slave_now/dev/pts με όνομα τον αριθμό του ψευδοτερματικού
** που εκτελεί την εκτύπωση. Αν η παράμετρος είναι μηδενική
** (τέλος εκτύπωσης), τότε διαγράφεται το mark.
*/

static void mark_slave(const int mark_on)
{
	char *tty;
	FILE *fp;
	char mark_file[1024];

	if ((tty = ttyname(1)) == (char *)(NULL))
		return;

	if (strncmp(tty, "/dev/pts/", 9))
		return;

	sprintf(mark_file, "/local/slave_now/%s", tty);
	if (mark_on) {
		if ((fp = fopen(mark_file, "a")) != (FILE *)(NULL)) {
			fclose(fp);
			chmod(mark_file, 0666);
		}

		return;
	}

	if ((fp = fopen(mark_file, "r")) != (FILE *)(NULL)) {
		int c;

		while ((c = getc(fp)) != EOF)
			putchar(c);

		fclose(fp);
	}
	unlink(mark_file);
}

int cht_prterm(int printer_on)
{
	static struct termio tio;
	static int c_oflag;
	static char lock_keyboard[32];
	static char unlock_keyboard[32];
	static char aux_on[32];
	static char aux_off[32];

	if (*aux_on == 0) {
		char *term;
		FILE *fp;
		int errs = 0;
		char t[1024];
		char lk[1024];
		char ulk[1024];
		char aon[1024];
		char aoff[1024];

		if ((term = getenv("TERM")) == (char *)0) {
			cht_error("prterm: TERM variable not set");
			return 1;
		}

		if ((fp = fopen("/cht/lib/prterm", "r")) == (FILE *)0) {
			cht_error("prterm: cannot open prterm file");
			return 2;
		}

		while (fscanf(fp, "%s%s%s%s%s", t,
			lk, aon, ulk, aoff) == 5) {
			if (strcmp(t, term) == 0)
				goto OK_TERM;
		}

		cht_error(NULL);
		fprintf(stderr, "prterm: TERM not implemented (%s)\n",
			term);
		return 3;

OK_TERM:
		errs += decode(lock_keyboard, lk);
		errs += decode(unlock_keyboard, ulk);
		errs += decode(aux_on, aon);
		errs += decode(aux_off, aoff);
		if (errs)
			return 4;
	}

	if (printer_on) {
		fflush(stdout);
		switch (ioctl(1, TCGETA, &tio)) {
		case 0:
			break;
		default:
			cht_error("prterm: ioctl error (TCGETA)");
			return 5;
		}

		mark_slave(1);
		fputs(lock_keyboard, stdout);
		fputs(aux_on, stdout);
		c_oflag = tio.c_oflag;
		tio.c_oflag |= ONLCR;

		if (ioctl(1, TCSETAW, &tio)) {
			cht_error("prterm: ioctl error (TCSETA)");
			return 5;
		}
	}
	else {
		fflush(stdout);
		tio.c_oflag = c_oflag;

		if (ioctl(1, TCSETAW, &tio)) {
			cht_error("prterm: ioctl error (TCSETA)");
			return 5;
		}

		fputs(aux_off, stdout);
		fputs(unlock_keyboard, stdout);
		mark_slave(0);
	}

	return 0;
}
