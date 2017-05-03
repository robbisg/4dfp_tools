/*$Header: /data/petsun4/data1/src_solaris/peak_4dfp/RCS/atl2index.c,v 1.1 2013/12/19 22:42:57 avi Exp $*/
/*$Log: atl2index.c,v $
 * Revision 1.1  2013/12/19  22:42:57  avi
 * Initial revision
 **/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <endianio.h>
#include <Getifh.h>

#define MAXL		256

/*************/
/* externals */
/*************/
extern void	vrtflip_  (int *orientation, int *imgdim, float *center, float *mmppix, float *centerr, float *mmppixr);
extern void	index_flip (int orientation, int *imgdim, float *fndex);	/* below */

/********************/
/* global variables */
/********************/
static IFH	ifh;
static float	mmppixr[3], centerr[3];
static char	program[MAXL];
static char	rcsid[] = "$Id: atl2index.c,v 1.1 2013/12/19 22:42:57 avi Exp $";

int main (int argc, char *argv[]) {
	FILE		*lstfp, *outfp;	
	char		*ptr, command[MAXL];
	char		ifhroot[MAXL], ifhfile[MAXL], lstfile[MAXL], outfile[MAXL] = ""; 

/***************/
/* computation */
/***************/
	float		fndex[3], x[3];
	int		c, i, k;

/*********/
/* flags */
/*********/
	int		fortran = 0;
	int		analyze = 0;
	int		status = 0;
	int		debug = 0;
	
	printf ("#%s\n", rcsid);
	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; else ptr++;
	strcpy (program, ptr);

/******************************/
/* get command line arguments */
/******************************/
	for (k = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]); ptr = command;
			while ((c = *ptr++)) switch (c) {
				case 'f': fortran++;			break;
				case 'a': analyze++;			break;
				case 'd': debug++;			break;
				case 'o': strncpy (outfile, ptr, MAXL);	*ptr = '\0';break;
			}
		} else switch (k) {
		 	case 0: getroot (argv[i], ifhroot);	k++; break;
		 	case 1: strcpy  (lstfile, argv[i]);	k++; break;
		}	
	}
	if (k < 2) {
		printf ("Usage: %s <(4dfp) ifhroot> <coords_file>\n", program);
		printf (" e.g.: %s -af my_tstat_333.4dfp.ifh my_333_coords.lst\n", program);
		printf ("\toption\n");
		printf ("\t-f\toutput indices using FORTRAN convention (first index=1) (default first index=0)\n");
		printf ("\t-a\toutput indices for displays loaded with orientation-specific 4dfp<->analyze flips\n");
		printf ("\t-o<str>\toutput indices to specified file\n");
		printf ("N.B.:\t<(4dfp) ifhroot> corresponds to the 4dfp image from which the coords were read\n");
		exit (1);
	}

/************/
/* read ifh */
/************/
	sprintf (ifhfile, "%s.4dfp.img", ifhroot);
	printf ("#Reading: %s\n", ifhfile);
	if (Getifh (ifhfile, &ifh)) errr (program, ifhfile);
	printf ("#dimensions \t%10d%10d%10d%10d\n",
		ifh.matrix_size[0], ifh.matrix_size[1], ifh.matrix_size[2], ifh.matrix_size[3]);
	printf ("#mmppix     \t%10.6f%10.6f%10.6f\n", ifh.mmppix[0], ifh.mmppix[1], ifh.mmppix[2]);
	printf ("#center     \t%10.4f%10.4f%10.4f\n", ifh.center[0], ifh.center[1], ifh.center[2]);
	printf ("#orientation\t%10d\n", ifh.orientation);

/*****************************************/
/* virtual flip instead of x4dfp2ecat () */
/*****************************************/
	vrtflip_ (&ifh.orientation, ifh.matrix_size, ifh.center, ifh.mmppix, centerr, mmppixr);
	if (debug) {
		printf ("atlas mmppix     \t%10.6f%10.6f%10.6f\n", mmppixr[0], mmppixr[1], mmppixr[2]); 
		printf ("atlas center     \t%10.4f%10.4f%10.4f\n", centerr[0], centerr[1], centerr[2]);
	}

	if (!(lstfp = fopen (lstfile, "r"))) errr (program, lstfile);
	if (strlen (outfile)) {
		if (!(outfp = fopen (outfile, "w"))) errw (program, outfile);
	}
	printf ("#|__input coordinates (mm)____|");
	printf ("|___________indices___________|\n");
	while (fgets (command, MAXL, lstfp)) {
/*		if (debug) printf ("%s", command);
		if ((ptr = strchr (command, '#'))) *ptr = '\0';
		if (sscanf (command, "%f%f%f", fndex + 0, fndex + 1, fndex + 2) != 3) continue;
		printf ("%10.4f%10.4f%10.4f", fndex[0], fndex[1], fndex[2]);
		if (!fortran) for (k = 0; k < 3; k++) fndex[k] += 1.0;
		if (analyze) index_flip (ifh.orientation, ifh.matrix_size, fndex);
		printf ("%10.4f%10.4f%10.4f", fndex[0], fndex[1], fndex[2]);
		for (k = 0; k < 3; k++) x[k] = fndex[k]*mmppixr[k] - centerr[k];
		printf ("%10.4f%10.4f%10.4f\n", x[0], x[1], x[2]);
*/
		if (debug) printf ("%s", command);
		if ((ptr = strchr (command, '#'))) *ptr = '\0';
		if (sscanf (command, "%f%f%f", x + 0, x + 1, x + 2) != 3) continue;
		printf ("%10.4f%10.4f%10.4f", x[0], x[1], x[2]);
		for (k = 0; k < 3; k++) fndex[k] = (x[k] + centerr[k])/mmppixr[k];
		if (analyze) index_flip (ifh.orientation, ifh.matrix_size, fndex);
		if (!fortran) for (k = 0; k < 3; k++) fndex[k] -= 1.0;
		printf ("%10.4f%10.4f%10.4f\n", fndex[0], fndex[1], fndex[2]);
		if (strlen (outfile)) {
			fprintf (outfp, "%10.4f%10.4f%10.4f\n", fndex[0], fndex[1], fndex[2]);
		}
	}
	fclose (lstfp);
	if (strlen (outfile)) fclose (outfp);
 	exit (status);
}

/************************************************/
/* convert (flip) array indices 4dfp to analyze */
/************************************************/
void index_flip (int orientation, int* imgdim, float* fndex) {
	switch (orientation) {
		case 4:	fndex[0] = (float) imgdim[0] + 1. - fndex[0];
		case 3:	fndex[2] = (float) imgdim[2] + 1. - fndex[2];
		case 2:	fndex[1] = (float) imgdim[1] + 1. - fndex[1];
		return;
		break;
	}
}
