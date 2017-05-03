/*$Header: /data/petsun4/data1/src_solaris/algebra_4dfp/RCS/ROI_resolve_multivol_4dfp.c,v 1.1 2014/08/09 22:26:48 avi Exp $*/
/*$Log: ROI_resolve_multivol_4dfp.c,v $
 * Revision 1.1  2014/08/09  22:26:48  avi
 * Initial revision
 **/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>		/* R_OK */
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <endianio.h>
#include <Getifh.h>
#include <rec.h>

#define MAXL		256	/* maximum string length */

/********************/
/* global externals */
/********************/
void vrtflip  (int *iori, int *imgdim, float *centeri, float *mmppixi, float *centerr, float *mmppixr);	/* cvrtflip.c */

/********************/
/* global variables */
/********************/
char		program[MAXL];

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

void setprog (char *program, char **argv) {
	char *ptr;

	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; 
	else ptr++;
	strcpy (program, ptr);
}

typedef struct {
	float	roicom[3];
	int	nvox, nvox1;
} ROI_INFO;

void usage () {
	fprintf (stderr, "Usage:	%s <(4dfp) multi-volume_ROI>\n", program);
	fprintf (stderr, "	option\n");
	fprintf (stderr, "	-@<b|l>\toutput big or little endian (default CPU endian)\n");
	fprintf (stderr, "N.B.:	output 4dfp fileroot is same as input with appended \"z\"\n");
	exit (1);
}

static char rcsid[] = "$Id: ROI_resolve_multivol_4dfp.c,v 1.1 2014/08/09 22:26:48 avi Exp $";
int main (int argc, char *argv[]) {
	FILE		*fp;
	char		imgroot[MAXL], imgfile[MAXL], ifhfile[MAXL], outroot[MAXL], outfile[MAXL];
	IFH		ifh;
	float		mmppixr[3], centerr[3];
	float		**imag, *imgr;
	float		amax = -FLT_MAX, amin = FLT_MAX;
	int		ix, iy, iz, jndex, dimension, isbig;
	int		*imgw;		/* tracks winning ROI at each voxel */
	char		control = '\0';

/**********************/
/* roi center of mass */
/**********************/
	int		nroi = 0;
	ROI_INFO	*roi_info;
	double		fndex[3], r1[3], rsq1;

/***********/
/* utility */
/***********/
	int		c, i, j, k;
	char		*ptr, command[MAXL];
	char		*srgv[MAXL];				/* list file string field pointers */

/*********/
/* flags */
/*********/
	int		debug = 0;
	int		status = 0;

	printf ("%s\n", rcsid);
	setprog (program, argv);

/************************/
/* process command line */
/************************/
	for (k = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]); ptr = command;
			while (c = *ptr++) switch (c) {
				case 'd': debug++;			break;
				case '@': control = *ptr++;		*ptr = '\0'; break;
			}
		} else switch (k) {
			case 0: getroot (argv[i], imgroot); k++;	break;
		}	
	}
	if (k < 1) usage ();

/************************/
/* get input dimensions */
/************************/
	sprintf (ifhfile, "%s.4dfp.ifh", imgroot);
	printf ("Reading: %s\n", ifhfile);
	if (Getifh (ifhfile, &ifh)) errr (program, ifhfile);
	isbig = strcmp (ifh.imagedata_byte_order, "littleendian");
	if (!control) control = (isbig) ? 'b' : 'l';

/*****************************************/
/* virtual flip instead of x4dfp2ecat () */
/*****************************************/
	vrtflip (&ifh.orientation, ifh.matrix_size, ifh.center, ifh.mmppix, centerr, mmppixr);
	if (debug) {
		printf ("orient=%d\n", ifh.orientation);
		printf ("image dimensions         %10d%10d%10d\n",
						ifh.matrix_size[0], ifh.matrix_size[1], ifh.matrix_size[2]);
		printf ("virtually flipped mmppix %10.6f%10.6f%10.6f\n", mmppixr[0], mmppixr[1], mmppixr[2]);
		printf ("virtually flipped center %10.4f%10.4f%10.4f\n", centerr[0], centerr[1], centerr[2]);
	}
	dimension = 1; for (k = 0; k < 3; k++) dimension *= ifh.matrix_size[k];
	nroi = ifh.matrix_size[3];
	imag = calloc_float2 (nroi, dimension);
	if (!(roi_info = calloc (nroi, sizeof (ROI_INFO))))	errm (program);
	if (!(imgw = calloc (dimension, sizeof (int))))		errm (program);
	if (!(imgr = calloc (dimension, sizeof (float))))	errm (program);

/**************/
/* read input */
/**************/
	sprintf (imgfile, "%s.4dfp.img", imgroot);
	printf ("Reading: %s\n", imgfile);
	if (!(fp = fopen (imgfile, "rb")) || eread (imag[0], nroi*dimension, isbig, fp)
	|| fclose (fp)) errr (program, imgfile);

/**************************************/
/* compute center of mass for all ROI */
/**************************************/
	for (i = 0; i < nroi; i++) {
		jndex = 0;
		for (iz = 0; iz < ifh.matrix_size[2]; iz++) {fndex[2] = (double) (iz + 1);
		for (iy = 0; iy < ifh.matrix_size[1]; iy++) {fndex[1] = (double) (iy + 1);
		for (ix = 0; ix < ifh.matrix_size[0]; ix++) {fndex[0] = (double) (ix + 1);
			if (imag[i][jndex] != 0.0 && imag[i][jndex] != (float) 1.e-37) {
				for (k = 0; k < 3; k++) roi_info[i].roicom[k] += fndex[k]*mmppixr[k] - centerr[k];
				roi_info[i].nvox++;
			}
			jndex++;
		}}}
		assert (jndex == dimension);
		for (k = 0; k < 3; k++) roi_info[i].roicom[k] /= roi_info[i].nvox;
	}

/*****************************************************/
/* compute winning ROI at each voxel and zero losers */
/*****************************************************/
	jndex = 0;
	for (iz = 0; iz < ifh.matrix_size[2]; iz++) {fndex[2] = (double) (iz + 1);
	for (iy = 0; iy < ifh.matrix_size[1]; iy++) {fndex[1] = (double) (iy + 1);
	for (ix = 0; ix < ifh.matrix_size[0]; ix++) {fndex[0] = (double) (ix + 1);
		for (i = 0; i < nroi; i++) if (imag[i][jndex] > 0.) {
			for (rsq1 = k = 0; k < 3; k++) {
				r1[k] = fndex[k]*mmppixr[k] - centerr[k] - roi_info[i].roicom[k];
				rsq1 += r1[k]*r1[k];
			}
			if (!imgw[jndex] || rsq1 < imgr[jndex]) {
				imgr[jndex] = rsq1;
				imgw[jndex] = i + 1;
			}
		}
		for (i = 0; i < nroi; i++) if (imgw[jndex] != i + 1) imag[i][jndex] = 0.;
		jndex++;
	}}}
	assert (jndex == dimension);

/*********************************/
/* compile revised ROI voxcounts */
/*********************************/
	for (i = 0; i < nroi; i++) {
		roi_info[i].nvox1 = jndex = 0;
		for (iz = 0; iz < ifh.matrix_size[2]; iz++) {
		for (iy = 0; iy < ifh.matrix_size[1]; iy++) {
		for (ix = 0; ix < ifh.matrix_size[0]; ix++) {
			if (imag[i][jndex] != 0.0 && imag[i][jndex] != (float) 1.e-37) roi_info[i].nvox1++;
			if (imag[i][jndex] > amax) amax = imag[i][jndex];
			if (imag[i][jndex] < amin) amin = imag[i][jndex];
			jndex++;
		}}}
		assert (jndex == dimension);
	}

/*****************/
/* write results */
/*****************/
	sprintf (outroot, "%sz", imgroot);
	sprintf (outfile, "%s.4dfp.img", outroot);
	printf ("Writing: %s\n", outfile);
	if (!(fp = fopen (outfile, "wb")) || ewrite (imag[0], nroi*dimension, control, fp)
	|| fclose (fp)) errr (program, outfile);

/*******/
/* ifh */
/*******/
	Writeifh (program, outfile, &ifh, control);

/*******/
/* hdr */
/*******/
	sprintf (command, "ifh2hdr %s -r%.0fto%.0f", outroot, amin - 0.4999, amax + 0.4999);
	printf ("%s\n", command);
	status |= system (command);

/*******/
/* rec */
/*******/
	startrece (outfile, argc, argv, rcsid, control);
	for (i = 0; i < nroi; i++) {
		sprintf (command, "ROI %4d voxel count %4d -> %4d\n", i + 1, roi_info[i].nvox, roi_info[i].nvox1);
		printrec (command);
	}
	catrec (imgfile);
	endrec ();

	free (imgw); free (imgr); free (roi_info);
	free_float2 (imag);
	exit (status);
}
