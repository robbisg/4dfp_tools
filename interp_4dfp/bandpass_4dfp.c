/* $Header: /data/petsun4/data1/src_solaris/interp_4dfp/RCS/bandpass_4dfp.c,v 1.13 2014/05/16 03:53:45 avi Exp $*/
/* $Log: bandpass_4dfp.c,v $
 * Revision 1.13  2014/05/16  03:53:45  avi
 * correct fmtptr housekeeping logic to work for single files
 *
 * Revision 1.12  2014/04/14  06:52:37  avi
 * trap illegal format
 *
 * Revision 1.11  2014/04/14  02:38:23  avi
 * major revision - format specifiable on command line
 * interpolation code in gap.c
 * margin computed based on TR_vol
 * input image value zero counts as undefined
 *
 * Revision 1.10  2013/02/05  23:19:19  avi
 * default nskip 4 -> 0
 * always allocate scratch arrays a,b and used new subroutine butt1dbs_() instead of butt1db_()
 * remove option -c
 * add option -r (retain linear trend)
 *
 * Revision 1.9  2011/02/06  08:27:08  avi
 * option -B
 *
 * Revision 1.8  2007/11/20  03:16:10  avi
 * linux compatible (remove f_init() and f_exit())
 *
 * Revision 1.7  2006/09/25  16:53:34  avi
 * Solaris 10
 *
 * Revision 1.6  2006/08/07  03:25:58  avi
 * correct 1.e-37 test
 *
 * Revision 1.5  2004/12/29  00:42:05  avi
 * conc functionality
 * unconfuse low/high half-frequencies vs. low/high pass
 *
 * Revision 1.4  2004/11/22  22:15:59  rsachs
 * Installed 'setprog'. Replaced 'Get4dfpDimN' with 'get_4dfp_dimo'.
 *
 * Revision 1.3  2004/08/31  04:55:38  avi
 * improved algorithm for computing DC shift and linear trend
 * -a now controls only retaining DC shift
 *
 * Revision 1.2  2004/05/26  20:55:42  avi
 * improve rec file filter characteristic listing
 *
 * Revision 1.1  2004/05/26  20:31:21  avi
 * Initial revision
 *
 * Revision 1.4  2002/06/26  05:37:44  avi
 * better usage
 *
 * Revision 1.3  2002/06/26  05:14:20  avi
 * -a (keepDC) option
 *
 * Revision 1.2  2002/06/25  05:32:02  avi
 * impliment Butterworth highpasss filter
 *
 * Revision 1.1  2002/06/25  02:39:29  avi
 * Initial revision
 **/
/*************************************************************
Purpose:	time series filter  
Date:		6/22/02
Author:		Avi Snyder
**************************************************************/
#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	<stdlib.h>
#include	<unistd.h>		/* R_OK */
#include	<string.h>
#include	<assert.h>
#include	<Getifh.h>
#include	<endianio.h>
#include        <rec.h>
#include 	<conc.h>

#define MAXL		256
#define ORDER_LO	0
#define ORDER_HI	0
#define NSKIP		0
#define TRAILER		"bpss"

/*************/
/* externals */
/*************/
extern int	npad_ (int *tdim, int *margin);	/* FORTRAN librms */
extern void 	butt1dbs_ (float *data,          int *n, float *delta, float *fhalf_lo, int *iorder_lo, float *fhalf_hi, int *iorder_hi, float *a, float *b);
extern void	pbutt1dcs_ (float *data, int *n0, int *n, float *delta, float *fhalf_lo, int *iorder_lo, float *fhalf_hi, int *iorder_hi, float *a, float *b);
extern int 	find_gaps (char *format, int tpad, int *t0, int *t1, int ngapmax);	/* gap.c */
extern void	fill_gaps (float *fpad,  int tpad, int *t0, int *t1, int ngap);		/* gap.c */
extern int	expandf (char *string, int len);					/* expandf.c */

void usage (char* program) {
	printf ("Usage:\t%s <(4dfp|conc) input> <TR_vol>\n", program);
	printf ("e.g.:\t%s qst1_b1_rmsp_dbnd_xr3d[.4dfp.img] 2.36 -bl0.01 -ol1 -bh0.15 -oh2\n", program);
	printf ("\toption\n");
	printf ("\t-b[l|h]<flt>\tspecify low end or high end half frequency in hz\n");
	printf ("\t-o[l|h]<int>\tspecify low end or high end Butterworth filter order\n");
	printf ("\t-n<int>\tspecify number of pre-functional frames (default = %d)\n", NSKIP);
	printf ("\t-f<string>\tspecify frames-to-count format (overrides option -n)\n");
	printf ("\t-t<str>\tchange output filename trailer (default=\"_%s\")\n", TRAILER);
	printf ("\t-a\tretain DC (constant) component\n");
	printf ("\t-r\tretain linear trend\n");
	printf ("\t-E\tcode undefined voxels as 1.e-37\n");
	printf ("\t-B\tcompute gain using correct Butterworth formula (default squared Butterworth gain)\n");
	printf ("\t-@<b|l>\toutput big or little endian (default input endian)\n");
	printf ("N.B.:\tundefined values are zero, NaN, or 1.e-37\n");
	printf ("N.B.:\tinput conc files must have extension \"conc\"\n");
	printf ("N.B.:\toutput filnename root is <input>_<trailer>\n", TRAILER);
	printf ("N.B.:\tomitting low  end order specification disables high pass component\n");
	printf ("N.B.:\tomitting high end order specification disables low  pass component\n");
	exit (1);
}

void setprog (char *program, char **argv) {
	char *ptr;

	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; 
	else ptr++;
	strcpy (program, ptr);
}

static char rcsid[] = "$Id: bandpass_4dfp.c,v 1.13 2014/05/16 03:53:45 avi Exp $";
int main (int argc, char *argv[]) {
	char		imgroot[MAXL], imgfile[MAXL], outroot[MAXL], outfile[MAXL];
	char		trailer[MAXL] = TRAILER;
	FILE		*fp;
	CONC_BLOCK	conc_block;		/* conc i/o control block */
	IFH		ifh;			/* for non-conc input */

	char		control = '\0', *format, *fmtpad;
	int		c, i, j, k, l, ix, iy, iz, ivox, orient, isbig, osbig;
	int		nfile, ifile, fmtptr;
	int		*t0, *t1, igap, ngap, ngapmax;
	int		imgdim[4], vdim, dimension, tdim, tpad, margin;
	int		order_lo = ORDER_LO, order_hi = ORDER_HI, nskip = NSKIP;

	float		fhalf_lo = 0.0, fhalf_hi = 0.0, TR_vol;
	float		*imgt, *imga;
	float		*a, *b;		/* scratch buffers used by Butterworth filters */
	float		voxdim[3];
	short		*mask;		/* set to 1 at undefined (1.e-37) voxels */
	short		*imgn;		/* denominator for average volume */

/****************************/
/* linear trend computation */
/****************************/
	float		*x, sx, sy, sxy, sxx, a0, a1;
	float		*fpad, q, det;

/***********/
/* utility */
/***********/
	char		*ptr, program[MAXL], command[MAXL];

/*********/
/* flags */
/*********/
	int		defined, printnow;
	int		status;
	int		conc_flag = 0;
	int		keepDC = 0;
	int		keepramp = 0;
	int		E_flag = 0;
	int		B_flag = 0;

	fprintf (stdout, "%s\n", rcsid);
	setprog (program, argv);
	if (!(format = (char *) calloc (4, sizeof (char)))) errm (program);

/******************************/
/* get command line arguments */
/******************************/
	for (k = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]); ptr = command;
			while (c = *ptr++) switch (c) {
				case 'a': keepDC++;			break;
				case 'r': keepramp++;			break;
				case 'E': E_flag++;			break;
				case 'B': B_flag++;			break;
				case '@': control = *ptr++;		*ptr = '\0'; break;
				case 'f': l = strlen (ptr) + 2;
					if (!(format = (char *) realloc (format, (l + 4)*sizeof (char)))) errm (program);
					strcpy  (format,  ptr);		*ptr = '\0'; break;
				case 'n': nskip = atoi (ptr);		*ptr = '\0'; break;
				case 't': strcpy (trailer, ptr);	*ptr = '\0'; break;
				case 'o': switch (*ptr++) {
					case 'l': order_lo = atoi (ptr);	break;
					case 'h': order_hi = atoi (ptr);	break;
					default:  usage (program);		break;
					}				*ptr = '\0'; break;
				case 'b': switch (*ptr++) {
					case 'l': fhalf_lo = atof (ptr);	break;
					case 'h': fhalf_hi = atof (ptr);	break;
					default:  usage (program);      	break;
					}				*ptr = '\0'; break;
			}
		} else switch (k) {
		 	case 0: getroot (argv[i], imgroot);
				conc_flag = (strstr (argv[i], ".conc") == argv[i] + strlen (imgroot));
								k++; break;
		 	case 1: TR_vol = atof (argv[i]);	k++; break;
		}
	}
	if (k < 2) usage (program);
	printf ("fhalf_lo %.4f order_lo %d fhalf_hi %.4f order_hi %d\n", fhalf_lo, order_lo, fhalf_hi, order_hi);
	if (fhalf_lo <= 0.0 || order_lo < 0) order_lo = 0;
	if (fhalf_hi <= 0.0 || order_hi < 0) order_hi = 0;

	if (conc_flag) {
		conc_init (&conc_block, program);
		conc_open (&conc_block, imgroot);
		for (k = 0; k < 4; k++) imgdim[k] = conc_block.imgdim[k];
		vdim = conc_block.vdim;
		nfile = conc_block.rnfile;
		isbig = conc_block.isbig;
	} else {
		sprintf (imgfile, "%s.4dfp.img", imgroot);
		if (get_4dfp_dimoe (imgfile, imgdim, voxdim, &orient, &isbig) < 0) errr (program, imgfile);
		if (Getifh (imgfile, &ifh)) errr (program, imgroot);
		for (vdim = 1, k = 0; k < 3; k++) vdim *= imgdim[k];
		nfile = 1;
		sprintf (outroot, "%s_%s", imgroot, trailer);
		sprintf (outfile, "%s.4dfp.img", outroot);
	}

	if (!control) control = (isbig) ? 'b' : 'l';
	if (conc_flag) conc_newe (&conc_block, trailer, control);
	sprintf (outroot, "%s_%s", imgroot, trailer);
	if (!(mask = (short *) calloc (vdim, sizeof (short)))) errm (program);
	if (!(imgn = (short *) calloc (vdim, sizeof (short)))) errm (program);	/* denominator for imga */
	if (!(imga = (float *) calloc (vdim, sizeof (float)))) errm (program);	/* average volume */

/**********************************/
/* compute frames-to-count format */
/**********************************/
	if (!(format = (char *) realloc (format, (imgdim[3] + 1)*sizeof (char)))) errm (program);
	format[imgdim[3]] = '\0';
	if (!strlen (format)) {
		for (k = 0; k < imgdim[3]; k++) format[k] = '+';
		for (fmtptr = ifile = 0; ifile < nfile; ifile++) {
			for (k = 0; k < nskip; k++) format[fmtptr + k] = 'x';
			fmtptr += (conc_flag) ? conc_block.nvol[ifile] : imgdim[3];
		}
		assert (fmtptr == imgdim[3]);			/* total concatenated frames */
	} else {
		if (k = expandf (format, imgdim[3] + 1)) exit (-1);
	}
	printf ("%s\n", format);
	margin = 35/TR_vol;

/***********************/
/* loop on input files */
/***********************/
for (fmtptr = ifile = 0; ifile < nfile; ifile++) {		/* begin ifile loop */
	if (conc_flag) imgdim[3] = conc_block.nvol[ifile];	/* frames in one run */
	dimension = vdim * imgdim[3];
	if (!(imgt = (float *) malloc (dimension * sizeof (float)))) errm (program);
	if (conc_flag) strcpy (imgfile, conc_block.imgfile0[ifile]);
	printf ("Reading: %s\n", imgfile);
	if (!(fp = fopen (imgfile, "rb")) || eread (imgt, dimension, isbig, fp)
	|| fclose (fp)) errr (program, imgfile);

/***********************************************************************/
/* allocate time series buffers and compute run-specific padded format */
/***********************************************************************/
	tdim = imgdim[3];
	tpad = npad_ (&tdim, &margin);
	printf ("original time series length %d padded to %d\n", tdim, tpad);
	if (!(fpad =	(float *) calloc (tpad, sizeof (float)))) errm (program);
	if (!(fmtpad =	(char *)  calloc (tpad, sizeof (char))))  errm (program);
	if (!(x =	(float *) calloc (tdim, sizeof (float)))) errm (program);
	for (i = 0; i < tdim; i++) x[i] = -1. + 2.*i/(tdim - 1);
	ngapmax = imgdim[3]/2;
	if (!(t0 = (int *) calloc (ngapmax, sizeof (int)))) errm (program);
	if (!(t1 = (int *) calloc (ngapmax, sizeof (int)))) errm (program);
	for (i = 0;    i < tdim; i++) fmtpad[i] = format[fmtptr + i];
	for (i = tdim; i < tpad; i++) fmtpad[i] = 'x';
	ngap = find_gaps (fmtpad, tpad, t0, t1, ngapmax);
	if (ngap < 0) {
		printf ("%s: illegal format\n", program);
		exit (-1);
	}
	if (1) for (igap = 0; igap < ngap; igap++) printf ("gap%5d%5d%5d\n", igap + 1, t0[igap], t1[igap]);
	if (!(a = (float *) calloc (tpad/2 + 1, sizeof (float)))) errm (program);
	if (!(b = (float *) calloc (tpad/2 + 1, sizeof (float)))) errm (program);

/*********************************/
/* process all voxels of one run */
/*********************************/
	for (ivox = 0; ivox < vdim; ivox++) mask[ivox] = 0;
	ivox = 0;
	printf ("processing slice");
	for (iz = 0; iz < imgdim[2]; iz++) {printf(" %d", iz + 1); fflush (stdout);
	for (iy = 0; iy < imgdim[1]; iy++) {
	for (ix = 0; ix < imgdim[0]; ix++) {
		printnow = (ix == imgdim[0]/2 && iy == imgdim[1]/2 && iz == imgdim[2]/2);	/* debug code */
		for (i = 0; i < imgdim[3]; i++) {
			q = (imgt + ivox)[i * vdim];
			defined = (q != 0.0) && (q != (float) 1.e-37) && !isnan (q);
			if (!defined) mask[ivox] = 1;
			fpad[i] = q;
		}
		if (!defined) goto UNDFVOX;

/******************************/
/* remove DC and linear trend */
/******************************/
		for (sy = sx = sxy = sxx = k = i = 0; i < imgdim[3]; i++) if (fmtpad[i] != 'x') {
			sy	+= fpad[i];
			sxy	+= fpad[i]*x[i];
			sx	+=         x[i];
			sxx	+=    x[i]*x[i];
			k++;
		}
		if (keepramp) {
			a0 = sy/k;
			a1 = 0.;
			for (i = 0; i < imgdim[3]; i++) fpad[i] -= a0;
		} else {
			det = k*sxx - sx*sx;
			a0 = (sxx*sy - sx*sxy)/det;
			a1 = (-sx*sy + k*sxy)/det;
			for (i = 0; i < imgdim[3]; i++) fpad[i] -= (a0 + x[i]*a1);
		}
		if (0) printf ("\na0 a1 %f %f\n", a0, a1);

/**********************************/
/* linearly interpolate over gaps */
/**********************************/
		fill_gaps (fpad, tpad, t0, t1, ngap);

/**********/
/* filter */
/**********/
		if (B_flag) {
			pbutt1dcs_ (fpad, &tdim, &tpad, &TR_vol, &fhalf_lo, &order_lo, &fhalf_hi, &order_hi, a, b);
		} else {
			 butt1dbs_ (fpad,        &tpad, &TR_vol, &fhalf_lo, &order_lo, &fhalf_hi, &order_hi, a, b);
		}

/*********************************************************************************/
/* force unpadded timeseries to zero mean and put filtered results back in image */
/*********************************************************************************/
		for (sy = k = i = 0; i < imgdim[3]; i++) if (fmtpad[i] != 'x') {
			sy += fpad[i];
			k++;
		}
		q = sy/k;
		for (i = 0; i < imgdim[3]; i++) (imgt + ivox)[i * vdim] = fpad[i] - q;


UNDFVOX:	for (i = 0; i < imgdim[3]; i++) {
			if (E_flag && mask[ivox]) {
						(imgt + ivox)[i * vdim] = 1.e-37;
			} else if (!conc_flag &&  keepDC) {
						(imgt + ivox)[i * vdim] += a0;
			}
		}
		if (!E_flag || !mask[ivox]) {
			imga[ivox] += a0;
			imgn[ivox]++;
		}
		ivox++;
	}}}
	printf("\n");

/**************************************/
/* write bandpass filtered 4dfp stack */
/**************************************/
	if (conc_flag) strcpy (outfile, conc_block.imgfile1[ifile]);
	printf ("Writing: %s\n", outfile);
	if (!(fp = fopen (outfile, "wb")) || ewrite (imgt, dimension, control, fp)
	|| fclose (fp)) errw (program, outfile);

	free (t0); free (t1); free (a); free (b);
	free (fpad); free (fmtpad); free (x); free (imgt);
	fmtptr += (conc_flag) ? conc_block.nvol[ifile] : imgdim[3];
}	/* end ifile loop */

/************************/
/* restore DC component */
/************************/
	switch (control) {
		case 'b': case 'B': osbig = 1; break;
		case 'l': case 'L': osbig = 0; break;
		default: osbig = CPU_is_bigendian(); break;
	}
	if (conc_flag && keepDC) {
		for (ivox = 0; ivox < vdim; ivox++) if (imgn[ivox]) imga[ivox] /= imgn[ivox];
		if (!(imgt = (float *) malloc (vdim * sizeof (float)))) errm (program);
		for (ifile = 0; ifile < nfile; ifile++) {
			printf ("Adding back DC %s frame", conc_block.imgfile1[ifile]);
			if (!(fp = fopen (conc_block.imgfile1[ifile], "r+b"))) errr (program, conc_block.imgfile1[ifile]);
			for (k = 0; k < conc_block.nvol[ifile]; k++) {printf(" %d", k + 1); fflush (stdout);
				if (fseek (fp, (long) k*vdim*sizeof (float), SEEK_SET)
				||  eread (imgt, vdim, osbig, fp))
					errr (program, conc_block.imgfile1[ifile]);
				for (ivox = 0; ivox < vdim; ivox++) {
					if (E_flag && imgt[ivox] == (float) 1.e-37) continue;
					imgt[ivox] += imga[ivox];
				}
				if (fseek (fp, (long) k*vdim*sizeof (float), SEEK_SET)
				||  ewrite (imgt, vdim, control, fp))
					errw (program, conc_block.imgfile1[ifile]);
			}
			printf ("\n"); fflush (stdout);
			if (fclose (fp)) errw (program, conc_block.imgfile1[ifile]);
		}
		free (imgt);
	}

/***************/
/* ifh and hdr */
/***************/
	if (conc_flag) {
		conc_ifh_hdr_rec (&conc_block, argc, argv, rcsid);
		conc_free (&conc_block);
		sprintf (outfile, "%s.conc", outroot);
		sprintf (imgfile, "%s.conc", imgroot);
		printf ("Writing: %s\n", outfile);
	} else {
		if (Writeifh (program, outfile, &ifh, control)) errw (program, outroot);
		sprintf (command, "ifh2hdr %s", outroot); status |= system (command);
	}

/*******/
/* rec */
/*******/
	startrece (outfile, argc, argv, rcsid, control);
	sprintf (command, "TR_vol (sec) %.4f\n", TR_vol);					printrec (command);
	if (!conc_flag) {
		sprintf (command, "Original time series length %d padded to %d\n", tdim, tpad);
		printrec (command);
	}
	if (keepDC) {
		sprintf (command, "DC shift retained\n");
	} else {
		sprintf (command, "DC shift removed\n");
	}											printrec (command);
	if (keepramp) {
		sprintf (command, "Linear trend retained\n");
	} else {
		sprintf (command, "Linear trend removed\n");
	}											printrec (command);
	if (order_lo) {
		sprintf (command, "High pass fhalf=%.4f (Hz)  order=%d\n", fhalf_lo, order_lo);	printrec (command);
	}
	if (order_hi) {
		sprintf (command, "Low  pass fhalf=%.4f (Hz)  order=%d\n", fhalf_hi, order_hi);	printrec (command);
	}
	catrec (imgfile);
	endrec ();

	free (format);
	free (mask); free (imgn); free (imga);
	exit (0);
}
