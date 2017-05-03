/*$Header: /data/petsun4/data1/src_solaris/algebra_4dfp/RCS/generate_rand_4dfp.c,v 1.2 2013/12/23 02:57:38 avi Exp $*/
/*$Log: generate_rand_4dfp.c,v $
 * Revision 1.2  2013/12/23  02:57:38  avi
 * option -i
 *
 * Revision 1.1  2013/12/23  01:43:43  avi
 * Initial revision
 **/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>		/* R_OK */
#include <string.h>
#include <math.h>
#include <float.h>
#include <endianio.h>
#include <Getifh.h>
#include <rec.h>

#define MAXL		256	/* maximum string length */

/*************/
/* externals */
/*************/
extern double	dnormal (void);

/********************/
/* global variables */
/********************/
static char	rcsid[] = "$Id: generate_rand_4dfp.c,v 1.2 2013/12/23 02:57:38 avi Exp $";
static char	program[MAXL];
static int	debug = 0;

void setprog (char *program, char **argv) {
	char *ptr;

	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; 
	else ptr++;
	strcpy (program, ptr);
}

void usage () {
	fprintf (stderr, "Usage:	%s <(4dfp) model ifh> <(4dfp) outroot>\n", program);
	fprintf (stderr, "	option\n");
	fprintf (stderr, "	-n<int>	specify number of output volumes (default 1)\n");
	fprintf (stderr, "	-i<int>	specify srand48 seed (default 0)\n");
	fprintf (stderr, "	-c<flt>	scale output values by specified factor\n");
	fprintf (stderr, "	-@<b|l>\toutput big or little endian (default input endian)\n");
	fprintf (stderr, "N.B.: %s generates random normal deviates in output voxels\n", program);
	exit (1);
}

int main (int argc, char *argv[]) {
	FILE		*fp;
	IFH		ifh;
	char		imgroot[MAXL], imgfile[MAXL], outroot[MAXL], outfile[MAXL];
	float		*imgt, scale = 1.0;
	float		voxdim[3];
	int		orient, imgdim[4], nvol = 1, vdim, isbig;
	long		iseed = 0;
	char		control = '\0';


/***********/
/* utility */
/***********/
	int		c, i, j, k;
	char		*ptr, command[MAXL];

/*********/
/* flags */
/*********/
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
				case 'i': iseed = atoi (ptr);		*ptr = '\0';	break;
				case 'n': nvol = atoi (ptr);		*ptr = '\0';	break;
				case 'c': scale = atof (ptr);		*ptr = '\0';	break;
				case '@': control = *ptr++;		*ptr = '\0';	break;
			}
		} else switch (k) {
			case 0: getroot (argv[i], imgroot);		k++; break;
			case 1:	getroot (argv[i], outroot);		k++; break;
		}	
	}
	if (k < 2) usage ();

	if (Getifh (imgroot, &ifh)) errr (program, imgroot);
	if (get_4dfp_dimoe (imgroot, imgdim, voxdim, &orient, &isbig)) exit (-1);
	if (!control) control = (isbig) ? 'b' : 'l';
	vdim = imgdim[0]*imgdim[1]*imgdim[2];
	if (!(imgt = (float *) calloc (vdim, sizeof (float)))) errm (program);

/**********************/
/* initialize srand48 */
/**********************/
	srand48 (iseed);

/***********************/
/* create output image */
/***********************/
	sprintf (outfile, "%s.4dfp.img", outroot);
	printf ("Writing: %s\n", outfile);
	if (!(fp = fopen (outfile, "wb"))) errw (program, outfile);
	for (k = 0; k < nvol; k++) {
		for (j = 0; j < vdim; j++) imgt[j] = scale*dnormal();
		if (ewrite (imgt, vdim, control, fp)) errw (program, outfile);
	}
	if (fclose (fp)) errw (program, outfile);

/*******/
/* ifh */
/*******/
	ifh.matrix_size[3] = nvol;
	Writeifh (program, outfile, &ifh, control);

/*******/
/* hdr */
/*******/
	sprintf (command, "ifh2hdr %s -r%.0fto%.0f", outroot, -3.*scale, 3.*scale);
	printf ("%s\n", command);
	status |= system (command);

/*******/
/* rec */
/*******/
	startrecle (outfile, argc, argv, rcsid, control);
	endrec ();

	free (imgt);
	exit (status);
}
