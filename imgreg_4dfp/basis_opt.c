/*$Header: /data/petsun4/data1/src_solaris/imgreg_4dfp/RCS/basis_opt.c,v 1.12 2013/12/25 03:43:20 avi Exp $*/
/*$Log: basis_opt.c,v $
 * Revision 1.12  2013/12/25  03:43:20  avi
 * nrestart limit
 *
 * Revision 1.11  2013/07/08  06:40:18  avi
 * starting basis weight search radius 0.5 -> 1.0
 *
 * Revision 1.10  2012/11/07  05:33:21  avi
 * -rw-rw----   1 avi      staff      25192 Nov  6 23:29 basis_opt.o
 * option -E
 *
 * Revision 1.9  2012/09/07  00:28:08  avi
 * revised Hessian mode logic
 *
 * Revision 1.8  2012/08/22  02:41:17  avi
 * improve use of gauss-Newton step
 *
 * Revision 1.7  2012/08/21  06:07:53  avi
 * debug Hessian code
 *
 * Revision 1.6  2012/08/20  03:20:49  avi
 * Gauss-Newton coded in C
 *
 * Revision 1.5  2012/08/10  05:17:28  avi
 * before recoding Gauss-Newton algorithm in C
 *
 * Revision 1.4  2012/08/05  02:04:46  avi
 * eliminate dependence on FORTRAN
 *
 * Revision 1.3  2012/08/01  00:51:53  avi
 * before eliminating FORTRAN
 *
 * Revision 1.2  2012/07/30  06:14:06  avi
 * Revision 1.1  2012/07/29  06:53:16  avi
 * Initial revision
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <endianio.h>
#include <Getifh.h>
#include <rec.h>
#include <librms.h>

#define MAXL	256
#define MAXS	256
#define E_RANGE	0.005
#define NITER	3

/***********/
/* globals */
/***********/
static char	program[MAXL];
static char	rcsid[] = "$Id: basis_opt.c,v 1.12 2013/12/25 03:43:20 avi Exp $";
static char	unwarp_Tyler[MAXL] = "";
static int	nw;		/* number of basis functions */
static int	n;		/* dimensionality of linear system = nw + ((freeze_ES) ? 0 : 1) */
static int	niter = NITER;	/* number of iterations */
static int	do_hess = 0;
static int	freeze_ES = 0;	/* prevent optimization of echo spacing */
static float	*weights = 0, *w_range = 0, ES, w_scale = 100000., e_range = E_RANGE, eta;
static float	*imgu = 0, *imge = 0, *imgb = 0;
static int	imgdim[4],  orient,  isbig, dimension;
static int	imgdimb[4], orientb, isbigb;
static float	voxdim[3], voxdimb[3];	
static char	control ='\0';
static char	pfile[MAXL], wfile[MAXL], tfile[MAXL] = "", phase[MAXL], imgfile[MAXL];

double **calloc_double2 (int n1, int n2) {
	int	i;
	double	**a;

	if (!(a = (double **) malloc (n1 * sizeof (double *)))) errm (program);
	if (!(a[0] = (double *) calloc (n1 * n2, sizeof (double)))) errm (program);
	for (i = 1; i < n1; i++) a[i] = a[0] + i*n2;
	return a;
}

void free_double2 (double **a) {
	free (a[0]);
	free (a);
}

void setprog (char *program, char **argv) {
	char *ptr;

	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; 
	else ptr++;
	strcpy (program, ptr);
}

void read_file_float (char *filename, float *stack, int dimension, char *program, int isbig) {
        FILE *fp;
 
	printf ("Reading: %s\n", filename);
        if (!(fp = fopen (filename, "rb"))
        ||  eread (stack, dimension, isbig, fp)
        ||  fclose (fp)) errr (program, filename);
}

static int split (char *string, char *srgv[], int maxp) {
	int	i, m;
	char	*ptr;

	if (ptr = strchr (string, '#')) *ptr = '\0';
	i = m = 0;
	while (m < maxp) {
		while (!isgraph ((int) string[i]) && string[i]) i++;
		if (!string[i]) break;
		srgv[m++] = string + i;
		while (isgraph ((int) string[i])) i++;
		if (!string[i]) break;
		string[i++] = '\0';
	}
	return m;
}

static void evalTyler (float *testws) {
        FILE	*fp;
	int	i, j, k, m, status, debug = 0;
	char	command[MAXL], string[MAXL], *srgv[MAXS];

	if (debug) {
		printf ("evalTyler testws:\n");
		for (i = 0; i < nw + 1; i++) printf (" %8.5f", testws[i]);
		printf ("\n");
	}
/********************************/
/* create estimated phase image */
/********************************/
	for (k = 0; k < dimension; k++) imge[k] = imgu[k];
	for (i = 0; i < nw; i++) {
		for (k = 0; k < dimension; k++) imge[k] += testws[i]*w_scale*(imgb + i*dimension)[k];
	}
	sprintf (imgfile, "%s.4dfp.img", phase);
	if (debug) printf ("Writing: %s\n", imgfile);
        if (!(fp = fopen (imgfile, "wb"))
        ||  ewrite (imge, dimension, isbig, fp)
        ||  fclose (fp)) errw (program, imgfile);
	writeifhe (program, imgfile, imgdim, voxdim, orient, control);

/********************************/
/* create temporary weight file */
/********************************/
	sprintf (tfile, "%s.tmp", wfile);
	if (debug) printf ("writing: %s\n", tfile);
	if (!(fp = fopen (tfile, "w"))) errw (program, tfile);
	fprintf (fp, "Echo Spacing: %.6f\n", testws[nw]);
	fclose (fp);

/************************/
/* run unwarping script */
/************************/
	sprintf (command, "%s %s %s", unwarp_Tyler, pfile, tfile);
	printf ("%s\n", command);
	status = system (command);
	if (status) exit (status);

	if (debug) printf ("Reading: %s\n", tfile);
	if (!(fp = fopen (tfile, "r"))) errr (program, tfile);
	while (fgets (string, MAXL, fp)) {
		if (!(m = split (string, srgv, MAXS))) continue;
		if (!strcmp (srgv[0], "Eta:")) {
			eta = atof (srgv[m - 1]);
			continue;
		}
	}
	fclose (fp);
}

void optimize () {
	float	*p_temp, *p_grad;
	float	eta0, etas[3], a1, a2;
	double	t, det, cndnum, **hess, **hest, **eigv;
	int	i, j, k, ii, jj;
	int	restart, nrestart, iter;
	int	debug = 0;

	printf ("parameters\n");
	for (j = 0; j < nw + 1; j++) printf (" %10.4f", weights[j]); printf ("\n");
	evalTyler (weights);
	if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
	printf ("parameter search radius\n");
	for (j = 0; j < n; j++) printf (" %10.4f", w_range[j]); printf ("\n");
	if (!(p_temp = (float *) calloc (nw + 1, sizeof (float)))) errm (program);
	if (!(p_grad = (float *) calloc (n,      sizeof (float)))) errm (program);

	eta0 = eta;
	nrestart = 0;
ASCEND:	if (++nrestart > 5) {
		fprintf (stderr, "%s gradient ascent restart limit exceeded\n", program);
		free (p_temp); free (p_grad);
		exit (-1);
	}
	for (iter = restart = 0; iter < niter; iter++) {
	printf ("begin gradient ascent iter = %d\n", iter + 1);
		for (restart = j = 0; j < n; j++) {
			printf ("j=%4d\n", j + 1); fflush (stdout);
			etas[1] = eta0;
			for (k = 0; k < nw + 1; k++)	p_temp[k] = weights[k];
ASCEND1:		for (i = -1; i <= 1; i += 2) {
				p_temp[j] = weights[j] + i*w_range[j];
				evalTyler (p_temp);
				if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
				etas[1 + i] = eta;
			}
			printf ("j=%4d  etas= %10.5f%10.5f%10.5f\n", j + 1, etas[0], etas[1], etas[2]);
			if (etas[0] > etas[1] && etas[0] > etas[2]) {
				etas[2] = etas[1];
				etas[1] = etas[0];
				weights[j] = weights[j] - w_range[j];
				p_temp[j]  = weights[j] - w_range[j];
				evalTyler (p_temp);
				if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
				etas[0] = eta;
				restart++;
				goto ASCEND1;
			} else if (etas[2] > etas[0] && etas[2] > etas[1]) {
				etas[0] = etas[1];
				etas[1] = etas[2];
				weights[j] = weights[j] + w_range[j];
				p_temp[j]  = weights[j] + w_range[j];
				evalTyler (p_temp);
				if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
				etas[2] = eta;
				restart++;
				goto ASCEND1;
			} else if (etas[1] > etas[0] && etas[1] > etas[2]) {
				a1 = 0.5*(etas[2] - etas[0])/w_range[j];
				a2 = 0.5*(etas[0] - 2.*etas[1] + etas[2])/(w_range[j]*w_range[j]); assert (a2 < 0.);
				printf ("a2 = %f\n", a2);
				printf ("old search radius = %f\n", w_range[j]);
				w_range[j] = sqrt(-e_range/a2);
				printf ("new search radius = %f\n", w_range[j]);
				t = weights[j];
				printf ("old weight = %10.6f\n", t);
				weights[j] -= 0.5*a1/a2;
				printf ("new weight = %10.6f\n", weights[j]);
				evalTyler (weights);
				if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
				eta0 = eta;
			}
		}	/* j loop */
		if (restart) {printf ("restarting %d\n", nrestart); goto ASCEND;}
	}	/* iter loop */
	if (!do_hess) goto FINISH;

	etas[1] = eta0 = eta;
	printf ("evaluating gradient and Hessian diagonals\n");
	hess = calloc_double2 (n, n);
	hest = calloc_double2 (n, n);
	eigv = calloc_double2 (n, n);
	for (j = 0; j < n; j++) {
		for (k = 0; k < nw + 1; k++)	p_temp[k] = weights[k];
		for (i = -1; i <= 1; i += 2) {
			p_temp[j] = weights[j] + i*w_range[j];
			evalTyler (p_temp);
			if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
			etas[1 + i] = eta;
		}
		printf ("j=%4d  etas= %10.5f%10.5f%10.5f\n", j + 1, etas[0], etas[1], etas[2]);
		if (!(etas[1] > etas[0] && etas[1] > etas[2])) goto FINISH;
		p_grad[j] = 0.5*(etas[2] - etas[0])/w_range[j];
		hess[j][j] = (etas[0] - 2.*etas[1] + etas[2])/(w_range[j]*w_range[j]); assert (hess[j][j] < 0.);
	}
	printf ("evaluating Hessian off-diagonals\n");
	for (j = 0; j < n - 1; j++) for (jj = j + 1; jj < n; jj++) {
		printf ("j, jj = %3d %3d\n", j, jj);
		for (k = 0; k < nw + 1; k++)	p_temp[k] = weights[k];
		t = 0.;
		for (i = -1; i <= 1; i += 2) for (ii = -1; ii <= 1; ii += 2) {
			if (debug) printf ("i, ii = %3d %3d\n", i, ii);
			p_temp[j]  = weights[j]  +  i*w_range[j];
			p_temp[jj] = weights[jj] + ii*w_range[jj];
			evalTyler (p_temp);
			if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
			t += ii*i*eta;
		}
		hess[jj][j] = hess[j][jj] = 0.25*t/(w_range[j]*w_range[jj]);
	}
	for (i = 0; i < n; i++) {
		p_grad[i] *= -100.;
		for (j = 0; j < n; j++) hess[j][i] *= -100.;
	}

/************************/
/* report linear system */
/************************/
	printf ("linear system x100\n");
	k = (n <= 15) ? n : 15;
	for (i = 0; i < n; i++) {
		printf ("%10.5f  ", p_grad[i]); for (j = 0; j < k; j++) printf ("%10.5f", hess[j][i]); printf ("\n");
	}

/****************************/
/* analyze Hessian for skew */
/****************************/
	for (i = 0; i < n; i++) for (j = 0; j < n; j++) hest[i][j] = hess[i][j]/sqrt(hess[i][i]*hess[j][j]);
	printf ("normalized Hessian\n");
	k = (n <= 16) ? n : 16;
	for (i = 0; i < n; i++) {
		for (j = 0; j < k; j++) printf ("%10.5f", hest[j][i]); printf ("\n");
	}
	deigen_  (hest[0], eigv[0], &n);
	printf ("normalized Hessian eigenvalues\n");
	for (j = 0; j < n; j++) printf ("%10.5f", hest[j][j]); printf ("\n");
	cndnum = hest[0][0]/hest[n - 1][n - 1];
	if (cndnum < 1.) cndnum = 1./cndnum;

	printf ("parameters before Gauss-Newton\n");
	for (j = 0; j < n; j++) printf (" %10.5f", weights[j]); printf ("\n");
	printf ("eta before Gauss-Newton %10.5f\n", eta0);
/*********************/
/* Gauss-Newton step */
/*********************/
	dmatcop_ (hess[0], hest[0], &n);
	dmatinv_ (hest[0], &n, &det);
	printf ("normalized Hessian determinant  %10.4e  condition number %10.4e\n", det, cndnum);
	for (k = 0; k < n; k++)	p_temp[k] = weights[k];
	for (i = 0; i < n; i++) for (j = 0; j < n; j++) weights[i] -= p_grad[j]*hest[j][i];
	free_double2 (hess); free_double2 (hest); free_double2 (eigv);
	printf ("parameters  after Gauss-Newton\n");
	for (j = 0; j < n; j++) printf (" %10.5f", weights[j]); printf ("\n");
	evalTyler (weights);
	if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
	printf ("eta  after Gauss-Newton %10.5f\n", eta);
if (0) {
	if (eta < eta0) {
		printf ("no increase in eta with Hessian optimization\n");
		for (k = 0; k < n; k++)	weights[k] = p_temp[k];
	}
}

FINISH:	evalTyler (weights);
	if (debug) printf ("return from evalTyler eta=%10.5f\n", eta);
	printf ("final parameters\n");
	for (j = 0; j < nw + 1; j++) printf (" %10.4f", weights[j]); printf ("\n");
	printf ("eta %10.6f\n", eta);

	free (p_temp); free (p_grad);
}

int main (int argc, char **argv) {
/************/
/* file i/o */
/************/
	char		ph_mean[MAXL], ph_basis[MAXL], scrdir[MAXL];

/***********/
/* utility */
/***********/
	FILE		*fp;
	char		command[MAXL], string[MAXL], *srgv[MAXS], *ptr;
	int		c, i, j, k, m;

	printf ("%s\n", rcsid);
	setprog (program, argv);
/************************/
/* process command line */
/************************/
	for (k = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]); ptr = command;
			while (c = *ptr++) switch (c) {
				case 'e': e_range = atof (ptr);		*ptr = '\0'; break;
				case 'n': niter = atoi (ptr);		*ptr = '\0'; break;
				case 'E': freeze_ES++;			break;
				case 'H': do_hess++;			break;
				default:				break;
			
			}
		} else switch (k) {
			case 0:	strcpy (pfile, argv[i]);	k++; break;
			case 1:	strcpy (wfile, argv[i]);	k++; break;
			default:				break;
		}
	}
	if (k < 2) {
		printf ("Usage:\t%s <params.txt> <weights.txt>\n", program);
                printf (" e.g.:\t%s 120313_TC35039.params 120313_TC35039.weights\n", program);
                printf ("	options\n");
                printf ("	-e<flt>	specify e_range (default = %.4f)\n", E_RANGE);
                printf ("	-n<int>	specify number gradient ascent iterations (default = %d)\n", NITER);
                printf ("	-E	freeze echo spacing\n");
                printf ("	-H	perform a Gauss-Newton step after completing gradent ascent iteration\n");
                exit (1);
	}

/***************/
/* read params */
/***************/
	printf ("Reading: %s\n", pfile);
	if (!(fp = fopen (pfile, "r"))) errr (program, pfile);
	while (fgets (string, MAXL, fp)) {
		if (!(m = split (string, srgv, MAXS))) continue;
		if (!strcmp (srgv[1], "mean")) {
			strncpy (ph_mean, srgv[m - 1], MAXL);
			continue;
		}
		if (!strcmp (srgv[1], "basis")) {
			strncpy (ph_basis, srgv[m - 1], MAXL);
			continue;
		}
		if (!strcmp (srgv[1], "scratch")) {
			strncpy (scrdir, srgv[m - 1], MAXL);
			continue;
		}
		if (!strcmp (srgv[1], "unwarp_Tyler")) {
			strncpy (unwarp_Tyler, srgv[m - 1], MAXL);
			continue;
		}
		if (!strcmp (srgv[1], "phase")) {
			getroot (srgv[m - 1], phase);
			continue;
		}
	}
	fclose (fp);

/****************/
/* read weights */
/****************/
	printf ("Reading: %s\n", wfile);
	if (!(fp = fopen (wfile, "r"))) errr (program, wfile);
	while (fgets (string, MAXL, fp)) {
		if (!(m = split (string, srgv, MAXS))) continue;
		if (!strcmp (srgv[0], "Echo")) {
			ES = atof (srgv[m - 1]);
			continue;
		}
		if (!strcmp (srgv[0], "Weights:")) {
			nw = atoi (srgv[m - 1]);
			if (!(weights = (float *) calloc (nw + 1, sizeof (float)))) errm (program);
			if (!(w_range = (float *) calloc (nw + 1, sizeof (float)))) errm (program);
			for (k = 0; k < nw; k++) {
				fscanf (fp, "%f", weights + k);
				weights[k] /= w_scale;
			}
			continue;
		}
		if (!strcmp (srgv[0], "Eta:")) {
			eta = atof (srgv[m - 1]);
			continue;
		}
	}
	fclose (fp);
	weights[nw] = ES;
	for (k = 0; k < nw + 1; k++) printf (" %f", weights[k]);
	printf ("\n");

/*******************************************/ 
/* read field map mean and basis functions */
/*******************************************/ 
	getroot (ph_mean, ph_mean);
	sprintf (imgfile, "%s.4dfp.img", ph_mean);
	if (get_4dfp_dimoe (imgfile, imgdim, voxdim, &orient, &isbig) < 0) errr (program, ph_mean);
	if (!control) control = (isbig) ? 'b' : 'l';
	dimension = imgdim[0]*imgdim[1]*imgdim[2];
	if (!(imgu = (float *) malloc (dimension*sizeof (float)))) errm (program);
	if (!(imge = (float *) malloc (dimension*sizeof (float)))) errm (program);
	read_file_float (imgfile, imgu, dimension, program, isbig);

	getroot (ph_basis, ph_basis);
	sprintf (imgfile, "%s.4dfp.img", ph_basis);
	if (get_4dfp_dimoe (imgfile, imgdimb, voxdimb, &orientb, &isbigb) < 0) errr (program, ph_mean);
	if (imgdimb[3] < nw) {
		fprintf (stderr, "%s: fewer basis images (%d) than requested components (%n)\n", program, imgdimb[3], nw);
		exit (-1);
	}
	imgdimb[3] = nw;		/* ignore not needed basis images */
	if (!(imgb = (float *) malloc  (dimension*imgdimb[3]*sizeof (float)))) errm (program);
	read_file_float (imgfile, imgb, dimension*imgdimb[3], program, isbigb);

	w_range[nw] = .2;
	for (i = 0; i < nw; i++) w_range[i] = 1.0;
	n = nw + ((freeze_ES) ? 0 : 1);
	printf ("n=%d nw=%d\n", n, nw);
	optimize ();

/**************************/
/* save optimized weights */
/***************************/
	sprintf (command, "/bin/cp %s %s.bak", wfile, wfile);
	printf ("%s\n", command); system (command);
	printf ("Writing: %s\n", wfile);
	if (!(fp = fopen (wfile, "w"))) errw (program, wfile);
	fprintf (fp, "Echo Spacing: %.6f\n", weights[nw]);
	fprintf (fp, "Weights: %d\n", nw);
	for (i = 0; i < nw; i++) fprintf (fp, "%12.4f\n", weights[i]*w_scale);
	fprintf (fp, "Eta: %10.5f\n", eta);
	if (fclose(fp)) errw (program, wfile);

/***************************/
/* delete tmp weights file */
/***************************/
	if (strlen (tfile)) remove (tfile);

	if (imgu) free (imgu); if (imgb) free (imgb); if (imge) free (imge);
	if (weights) free (weights); if (w_range) free (w_range);
	exit (0);
}
