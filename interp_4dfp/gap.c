/*$Header: /data/petsun4/data1/src_solaris/interp_4dfp/RCS/gap.c,v 1.3 2014/04/14 20:49:00 avi Exp $*/
/*$Log: gap.c,v $
 * Revision 1.3  2014/04/14  20:49:00  avi
 * *** empty log message ***
 *
 * Revision 1.2  2014/04/14  06:51:56  avi
 * correct algorithm
 *
 * Revision 1.1  2014/04/14  02:38:08  avi
 * Initial revision
 **/
#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	<string.h>

#define NGAPMAX	 32

int  find_gaps (char *format, int tpad, int *t0, int *t1, int ngapmax);		/* below */
void fill_gaps (float *f,     int tpad, int *t0, int *t1, int ngap);		/* below */

/* int main (int argc, char *argv[]) { */
void gap_test1 (int argc, char *argv[]) {
	int	tdim = 164, tpad = 256;
	int	t0[NGAPMAX], t1[NGAPMAX], t, ngap, igap;
	float	x[tpad], x0[tpad];
	char	format[tpad];

	strcpy (format, "xx++x+++++++++++++++++++++xxx+++x+xxx++x++++++++++++x++++++x+++++++++++++++++++++++++++++x++x+++++++++++x+xxxxxx+++++++++xxxxxxxxx+++++++++++++++++++++++++++x+x++x+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	for (t = 0; t < tdim; t++) {
		x0[t] = x[t] = t/10.;
	}
	for (t = tdim; t < tpad; t++) {
		x0[t] = x[t] = 0.;
		format[t] = 'x';
	}

	ngap = find_gaps (format, tpad, t0, t1, NGAPMAX);
	printf ("ngap=%d\n", ngap);
	if (1) for (igap = 0; igap < ngap; igap++) printf ("%5d%5d\n", t0[igap], t1[igap]);

	fill_gaps (x, tpad, t0, t1, ngap);
	for (t = 0; t < tpad; t++) printf ("%5d%10.6f%10.6f\n", t, x0[t], x[t]);

	exit (0);
}

void gap_test0 (int argc, char *argv[]) {
	int	tdim = 64, tpad = 80;
	int	t0[16], t1[16], t, ngap, igap;
	float	x[tpad], x0[tpad];
	char	format[tpad];

	for (t = 0; t < tdim; t++) {
		x0[t] = x[t] = t/10.;
		format[t] = '+';
	}
	for (t = tdim; t < tpad; t++) {
		x0[t] = x[t] = 0.;
		format[t] = 'x';
	}
	format[3] = format[10] = format[11] = format[12] = 'x';
	x0[3] = x0[10] = x0[11] = x0[12] = 0.;

	ngap = find_gaps (format, tpad, t0, t1, 16);
	if (1) for (igap = 0; igap < ngap; igap++) printf ("%5d%5d\n", t0[igap], t1[igap]);

	fill_gaps (x, tpad, t0, t1, ngap);
	for (t = 0; t < tpad; t++) printf ("%5d%10.6f%10.6f\n", t, x0[t], x[t]);

	exit (0);
}

int	mt (int t, int n) {
	return (t + n) % n;
}

int find_gaps (char *format, int tpad, int *t0, int *t1, int ngapmax) {
	int	t, i, ngap, igap, nx;
	char	c;

	for (nx = t = 0; t < tpad; t++) if (format[t] == 'x') nx++;
	if (!nx) return 0;
	if (nx >= tpad - 1) return -1;

/*******************************/
/* find left edge of first gap */
/*******************************/
	c = format[0];
	for (t = tpad - 1; t > 0; t--) {
		if (format[t] != 'x' && c == 'x') break;
		c = format[t];
	}
	t0[0] = t;

/*****************/
/* find all gaps */
/*****************/
	ngap = 0;
	do {
		do {t = mt(++t, tpad);} while (format[t] == 'x');
		t1[ngap++] = t;
		do {t = mt(++t, tpad);} while (format[t] != 'x');
		t0[ngap] = mt(t - 1, tpad);
		if (t0[ngap] == t0[0]) break;
	} while (ngap < ngapmax);

	return ngap;
}

void fill_gaps (float *f, int tpad, int *t0, int *t1, int ngap) {
	int	t, i, igap, nx;

	for (igap = 0; igap < ngap; igap++) {
		nx = mt(t1[igap] - t0[igap], tpad);
		for (i = 1; i < nx; i++) {
			f[mt(t0[igap] + i, tpad)] = f[t0[igap]] + (f[t1[igap]] - f[t0[igap]])*i/(float) nx;
		}
	}
}
