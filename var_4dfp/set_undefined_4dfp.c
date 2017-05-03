/*$Header: /data/petsun4/data1/src_solaris/var_4dfp/RCS/set_undefined_4dfp.c,v 1.2 2013/08/23 04:58:54 avi Exp $*/
/*$Log: set_undefined_4dfp.c,v $
 * Revision 1.2  2013/08/23  04:58:54  avi
 * simpler algorithm
 *
 * Revision 1.1  2013/08/23  04:13:02  avi
 * Initial revision
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <unistd.h>
#include <Getifh.h>
#include <endianio.h>
#include <rec.h>

#define MAXL	256
#define ALPHA	0.02		/* minimum value of convolution kernel used by imgblur3d_() */

/*************/
/* externals */
/*************/
extern void	imgblur3d_(float *fwhm, float *alpha, float *voxsiz, float *img0, int *imgdim, float *img1);	/* fimgblur.f */

/********************/
/* global variables */
/********************/
static char	rcsid[] = "$Id: set_undefined_4dfp.c,v 1.2 2013/08/23 04:58:54 avi Exp $";
static char	program[MAXL];
static int	debug = 0;

float **calloc_float2 (int n1, int n2) {
	int	i;
	float	**a;

	if (!(a = (float **) malloc (n1 * sizeof (float *)))) errm (program);
	if (!(a[0] = (float *) calloc (n1 * n2, sizeof (float)))) errm (program);
	for (i = 1; i < n1; i++) a[i] = a[0] + i*n2;
	return a;
}

void free_float2 (float **a) {
	free (a[0]);
	free (a);
}

int write_4dfp (char *imgroot, char *trailer, float *imgt, int *imgdim, IFH *ifh, int argc, char *argv[], char control);	/* below */

void setprog (char *program, char **argv) {
	char *ptr;

	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; 
	else ptr++;
	strcpy (program, ptr);
}

void usage (char* program) {
	printf ("Usage:\t%s <4dfp stack>\n", program);
	printf ("	option\n");
	printf ("	-d	debug mode\n");
	exit (1);
}

int main (int argc, char **argv) {
	FILE 		*imgfp;
	IFH		ifh;
	char 		imgroot[MAXL], imgfile[MAXL];
	char 		outroot[MAXL] = "", outfile[MAXL], trailer[MAXL];

        int  		imgdim[4], mskdim[4], vdim, isbig;
	float		*imgv, *imgo;			/* variance over all frames */
	float		*imga;				/* average over all frames */
	float		**imgt;
	float		alpha = ALPHA, fwhm = 10.;	/* preblur FWHM in mm */
	char		control = '\0';

/***********/
/* utility */
/***********/
	char 		command[MAXL], program[MAXL], *ptr;
	int		c, i, j, k, m;
	double		q;

/*********/
/* flags */
/*********/
	int		debug = 0;

	fprintf (stdout, "%s\n", rcsid);
	setprog (program, argv);
/************************/
/* process command line */
/************************/
	for (k = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]);
			ptr = command;
			while (c = *ptr++) switch (c) {
				case 'd': debug++;		break;
			}
		}
		else switch (k) {
			case 0:	getroot (argv[i], imgroot);	k++; break;
		}	
	}
	if (k < 1) usage (program);

/**************************************/
/* get 4dfp vdims and open read/write */
/**************************************/
	sprintf (imgfile, "%s.4dfp.img", imgroot);
	if (Getifh (imgfile, &ifh)) errr (program, imgfile);
	for (k = 0; k < 4; k++) imgdim[k] = ifh.matrix_size[k];
	isbig = strcmp (ifh.imagedata_byte_order, "littleendian");
	if (!(imgfp = fopen (imgfile, "r+b"))) errw (program, imgfile);
	printf ("Reading: %s\n", imgfile);
	control = (isbig) ? 'b' : 'l';
	vdim = imgdim[0]*imgdim[1]*imgdim[2];

	if (!(imga = (float *) calloc (vdim,  sizeof (float)))) errm (program);
	if (!(imgv = (float *) calloc (vdim,  sizeof (float)))) errm (program);
	if (!(imgo = (float *) calloc (vdim,  sizeof (float)))) errm (program);
	imgt = calloc_float2 (imgdim[3], vdim);
	for (k = 0; k < imgdim[3]; k++) if (eread (imgt[k], vdim, isbig, imgfp)) errr (program, imgfile);

/****************************************/
/* compute and binarize mask of low sd1 */
/****************************************/
	for (i = 0; i < vdim; i++) {
		for (k = 0; k < imgdim[3]; k++) imga[i] += imgt[k][i];
		imga[i] /= imgdim[3];
		for (k = 0; k < imgdim[3]; k++) {
			q = imgt[k][i] - imga[i];
			imgv[i] += q*q;
		}
		imgv[i] = sqrt (imgv[i]/imgdim[3]);
		imgv[i] = (imgv[i] < .5) ? 0. : 1.0;
	}

/***********************************/
/* dilate low sd1 mask by blurring */
/***********************************/
	imgblur3d_(&fwhm, &alpha, ifh.scaling_factor, imgv, imgdim, imgo);
	for (k = 0; k < 3; k++) mskdim[k] = imgdim[k];
	mskdim[3] = 1;
	if (0) write_4dfp (imgroot, "msk", imgo, mskdim, &ifh, argc, argv, control);

	for (i = 0; i < vdim; i++) if (imgo[i] < 0.9) for (k = 0; k < imgdim[3]; k++) imgt[k][i] = 1.e-37;
	rewind (imgfp);
	printf ("Writing: %s\n", imgfile);
	for (k = 0; k < imgdim[3]; k++) if (ewrite (imgt[k], vdim, control, imgfp)) errw (program, imgfile);
	fclose (imgfp);

DONE:	free_float2 (imgt);
	free (imga); free (imgv); free (imgo);
	exit (0);
}

int write_4dfp (char *imgroot, char *trailer, float *imgt, int *imgdim, IFH *ifh, int argc, char *argv[], char control) {
	FILE		*imgfp;
	char		*ptr, imgfile[MAXL], outfile[MAXL], command[MAXL];
	int		i, tdim, status;
	float		amax;

	tdim = imgdim[0]*imgdim[1]*imgdim[2];
	for (amax = i = 0; i < tdim; i++) if (imgt[i] > amax) amax = imgt[i];

/********************/
/* assemble outroot */
/********************/
	if (ptr = strrchr (imgroot, '/')) ptr++; else ptr = imgroot;
	sprintf (outfile, "%s_%s.4dfp.img", ptr, trailer);

/*********/
/* write */
/*********/
	printf ("Writing: %s\n", outfile);
	if (!(imgfp = fopen (outfile, "wb")) || ewrite (imgt, tdim, control, imgfp)
	|| fclose (imgfp)) errw (program, outfile);

/*******/
/* ifh */
/*******/
	writeifhmce (program, outfile, imgdim,
		ifh->scaling_factor, ifh->orientation, ifh->mmppix, ifh->center, control);

/*******/
/* hdr */
/*******/
	sprintf (command, "ifh2hdr -r%d %s", (int) (amax + 0.5), outfile);
	printf ("%s\n", command); status |= system (command);

/*******/
/* rec */
/*******/
	sprintf (imgfile, "%s.4dfp.img", imgroot);
	startrecle (outfile, argc, argv, rcsid, control);
	catrec (imgfile);
	endrec ();
	return status;
}
