/*$Heade$*/
/*$Log: jitter.c,v $
 * Revision 1.8  2014/01/04  03:28:25  avi
 * update main()
 *
 * Revision 1.7  2014/01/04  03:26:29  avi
 * correct minor usage typo
 *
 * Revision 1.6  2005/08/06  00:18:58  avi
 * include delframe in output listing
 *
 * Revision 1.5  2005/07/04  23:28:12  avi
 * better usage
 *
 * Revision 1.4  2005/07/02  05:28:57  avi
 * -F (flat distribution)
 * last event ALWAYS on frame nframe
 *
 * Revision 1.3  2005/07/01  04:57:00  avi
 * radically revised algorithm works well
 *
 * Revision 1.2  2005/04/21  02:56:49  avi
 * increase numerical stability (compute initial a) and write output event-style file
 *
 * Revision 1.1  2002/06/02  00:25:12  avi
 * Initial revision
 **/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAXL		256
#define T_MAX		30.

void errm (char* program) {
	fprintf (stderr, "%s: memory allocation error\n", program);
	exit (-1);
}

void errw (char* program, char* filespc) {
	fprintf (stderr, "%s: %s write error\n", program, filespc);
	exit (-1);
}

typedef struct {
	float	d;	/* delay */
	float	t, ti;	/* event time */
	int	iframe;
	long	val;	/* randomization */
} SE;

int secompare (const void *p1, const void *p2) {
	SE		*SE1, *SE2;
	long		diff;

	SE1 = (SE *) p1; SE2 = (SE *) p2;
	diff = SE1->val - SE2->val;
	if (diff) return (diff < 0) ? -1 : 1;
	else return 0;
}

/********************/
/* global variables */
/********************/
SE		*list;
float		tr_vol, t_tot, t_on, t_min = 0., t_max = T_MAX;
double		a, b;
int		nevent, nframe, skip = 0;
int		verbose = 0, debug = 0, status = 0;
int		mode = 0;	/* 0 => Poisson process; 1 => flat */
char		program[MAXL];

double A (void) {
	return -a*log((1. + b)/((float)nevent + b));
}
double dAdb (void) {
	return -a*(1./(1. + b) - 1./((float)nevent + b));
}
double B (void) {
	double		q;
	int		k;

	for (q = 0.0, k = 1; k <= nevent; k++) {
		q -= a*log(((float)k + b)/((float)nevent + b));
	}
	return q;
}
double dBdb (void) {
	double		q;
	int		k;

	for (q = 0.0, k = 1; k <= nevent; k++) {
		q += 1./((float)k + b) - 1./((float)nevent + b);
	}
	return -a*q;
}
double R (void) {
	return A()/B();
}
double dRdb (void) {
	double		q;
	q = B();
	
	return (q*dAdb() - A()*dBdb())/(q*q);
}
void print_params () {
	printf ("%10.4f%10.4f%10.4f%10.4f%10.4f%10.4f%10.4f%10.4f\n",
		a, b, A(), B(), R(), dAdb(), dBdb(), dRdb());
}

void print_list (FILE *fp) {
	int		j, delframe;
	float		t;

	fprintf (fp, "%5s%10s%10s%10s%10s%10s\n", "event", "time", "duration", "frametime", "frame", "delframe");
	t = skip*tr_vol;
	for (j = 0; j <= nevent; j++) {
		list[j].t = t;
		list[j].iframe = 0.499999 + t/tr_vol;
		list[j].ti = tr_vol*list[j].iframe;
		t += list[j].d + t_min;
	}
	for (j = 0; j < nevent; j++) {
		delframe = (j) ? list[j].iframe - list[j - 1].iframe : list[j].iframe;
		fprintf (fp, "%-5d%10.4f%10.4f%10.4f%10d%10d\n",
		j + 1, list[j].t, list[j].d + t_min, list[j].ti, list[j].iframe, delframe);
	}
	fprintf (fp, "%-5d%10.4f%10s%10.4f%10d%10d\n",
		j + 1, list[j].t, "", list[j].ti, list[j].iframe, list[j].iframe - list[j - 1].iframe);
}

static char rcsid[] = "$Id: jitter.c,v 1.8 2014/01/04 03:28:25 avi Exp $";
int main (int argc, char *argv[]) {
	FILE		*outfp;
	char		*ptr, command[MAXL], outfile[MAXL] = "";
	int		c, i, j, k, m;
	double		R0, q, db;
	unsigned int	iseed = 0;

	printf ("%s\n", rcsid);
	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; else ptr++;
	strcpy (program, ptr);

/******************************/
/* get command line arguments */
/******************************/
	for (k = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]); ptr = command;
			while (c = *ptr++) switch (c) {
				case 'd': debug++;			break;
				case 'v': verbose++;			break;
				case 'F': mode = 1;			break;
				case 's': skip  = atoi (ptr);		*ptr = '\0'; break;
				case 'r': iseed = atoi (ptr);		*ptr = '\0'; break;
				case 'm': t_min = atof (ptr);		*ptr = '\0'; break;
				case 'g': t_max = atof (ptr);		*ptr = '\0'; break;
				case 'o': strcpy (outfile, ptr);	*ptr = '\0'; break;
			}
		} else switch (k) {
		 	case 0: nevent = 	atoi (argv[i]);	k++; break;
		 	case 1: nframe =	atoi (argv[i]);	k++; break;
		 	case 2: tr_vol =	atof (argv[i]);	k++; break;
		}	
	}
	if (k < 3 || nevent < 3) {
		printf ("Usage:	%s <(int) nevent> <(int) nframe> <(flt) tr_vol>\n", program);
		printf (" e.g.,	%s 20 100 2.0 -s4\n", program);
		printf ("	option\n");
		printf ("	-r<int>	specify randomization seed (default=0)\n");
		printf ("	-s<int>	add specified number of skip frames to output event series (default=0)\n");
		printf ("	-g<flt>	specify max interval in sec (t_max; default=%.2f)\n", T_MAX);
		printf ("	-m<flt>	specify min interval in sec (t_min; default=tr_vol)\n");
		printf ("	-o<str>	output named fidl-type event file\n");
		printf ("	-v	verbose mode\n");
		printf ("	-F	use flat distribution of delay intervals (default Poisson process)\n");
		printf ("N.B:	%s distributes nevent events on nframe frames\n", program);
		printf ("N.B:	nevent must be at least 3\n");
		printf ("N.B:	t_max is ignored with flat delay interval distribution (-F)\n");
		printf ("N.B:	first event is ALWAYS on frame skip\n");
		printf ("	last  event is ALWAYS on frame skip + nframe, duration = Inf\n");
		printf ("	fMRI run should include additional frames at end\n");
		exit (1);
	}
	if (t_min <= 0.0) t_min = tr_vol;
	t_tot = nframe*tr_vol;
	printf ("nevent=%d nframe=%d\n", nevent, nframe);
/******************************************************************************/
/* decrement nevent as last event is always on frame nframe (counting from 0) */
/******************************************************************************/
	nevent--;
	t_on = t_tot - nevent*t_min;
	if (t_on <= 0.0) {
		fprintf (stderr, "t_min (%.4f) too large\n", t_min);
		exit (-1);
	}
	printf ("t_min=%.4f tr_vol=%.4f t_max=%.4f, t_on=%.4f t_tot=%.4f\n", t_min, tr_vol, t_max, t_on, t_tot);

	switch (mode) {
	case 0:
		R0 = (t_max - t_min)/t_on;
		if (verbose) printf ("R0=%f\n", R0);
		a = tr_vol*nframe/nevent;
		b = 0.0;
		if (verbose) printf ("%10s%10s%10s%10s%10s%10s%10s%10s\n",
			"a", "b", "A()", "B()", "R()", "dAdb()", "dBdb()", "dRdb()");
		for (i = 0; i < 10; i++) {
			if (verbose) print_params ();
			db =  (R() - R0)/dRdb();
			b -= db;
			if (b < 0.0) {
				fprintf (stderr, "t_max (%.4f) not used\n", t_max);
				b = 0.0;
				break;
			}
			if (b > nevent) {
				fprintf (stderr, "t_max (%.4f) too small or nevent (%d) too small\n", t_max, nevent);
				status = -2;
				b = nevent;
				break;
			}
			if (fabs (db) < 1.e-6) break;
		}
		q = t_on/B();
		a *= q;
		if (verbose) print_params ();
		printf ("range of intervals is %10.4f to %10.4f\n", t_min, A() + t_min);
		if (status) {
			fprintf (stderr, "%s: input parameters not compatible with Poisson process\n", program);
			exit (status);
		}
		break;		/* mode 0 */
	case 1:
		a = 2*t_on/((float) nevent*(nevent - 1));
		printf ("range of intervals is %10.4f to %10.4f\n", t_min, t_min + a*(nevent - 1));
		break;		/* mode 1 */
	}
	
	srandom (iseed);
	if (!(list = (SE *) calloc ((nevent + 1), sizeof (SE)))) errm (program);
	for (i = 0; i < nevent; i++) {
		switch (mode) {
			case 0:	list[i].d = -a*log((i + 1. + b)/(nevent + b));	break;
			case 1: list[i].d = a*i;				break;
		}
		list[i].val = random ();
	}
	if (verbose) print_list (stdout);
	qsort (list, nevent, sizeof (SE), secompare);
	print_list (stdout);

	if (strlen (outfile)) {
		printf ("Writing: %s\n", outfile);
		if (!(outfp = fopen (outfile, "w"))) errw (program, outfile);
		fprintf (outfp, "%10.4f\n", tr_vol);
		for (i = 0; i <= nevent; i++) fprintf (outfp, "%10.4f\n", list[i].ti);
		if (fclose (outfp)) errw (program, outfile);
	}

	free (list);
	exit (0);
}
