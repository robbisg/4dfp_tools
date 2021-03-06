/*$Header: /data/petsun4/data1/src_solaris/imglin/RCS/t4_inv.c,v 1.1 2007/05/01 01:20:20 avi Exp $*/
/*$Log: t4_inv.c,v $
 * Revision 1.1  2007/05/01  01:20:20  avi
 * Initial revision
 **/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <librms.h>

#define MAXL	256

static char	program[MAXL];
static char	rcsid[] = "$Id: t4_inv.c,v 1.1 2007/05/01 01:20:20 avi Exp $";

/*************/
/* externals */
/*************/
void	t4inv_ (float *t4, float *invt4);		/* t4inv.f */

void errr (char* program, char* filespc) {
	fprintf (stderr, "%s: %s read error\n", program, filespc);
	exit (-1);
}

void errw (char* program, char* filespc) {
	fprintf (stderr, "%s: %s write error\n", program, filespc);
	exit (-1);
}

int split (char *string, char *srgv[], int maxp) {
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

int	t4read (FILE *fp, float *t4) {
	int	i, m;

	for (i = 0; i < 4; i++) {
		m = fscanf (fp, " %f %f %f %f", t4 + i + 0,  t4 + i + 4, t4 + i + 8, t4 + i + 12);
		if (m != 4) return -1;
	}
	return 0;
}

void	t4list (FILE *fp, float *t4) {
	int	i, m;

	fprintf (fp, "t4\n");
	for (i = 0; i < 4; i++) {
		fprintf (fp, "%10.6f%10.6f%10.6f%10.4f\n", (t4 + i)[0], (t4 + i)[4], (t4 + i)[8], (t4 + i)[12]);
	}
}

void write_command_line (FILE *outfp, int argc, char *argv[]) {
	int		i;

	fprintf (outfp, "#%s", program);
	for (i = 1; i < argc; i++) fprintf (outfp, " %s", argv[i]);
	fprintf (outfp, "\n#%s\n", rcsid);
}

void setprog (char *program, char **argv) {
	char *ptr;

	if (!(ptr = strrchr (argv[0], '/'))) ptr = argv[0]; 
	else ptr++;
	strcpy (program, ptr);
}

int main (int argc, char *argv[]) {
	FILE		*fp;
	char		t4file[2][MAXL];
	char		imgfile[2][MAXL];
	float		t4[2][16], scale[2] = {0., 0.};

/***********/
/* utility */
/***********/
	char		*str, command[MAXL], string[MAXL], *srgv[MAXL];
	char		*str1, *str2;
	int 		c, i, k, m, n;
	int		four = 4;

/*********/
/* flags */
/*********/
	int		debug = 0;
	int		done;
	int		noscale = 0;

	fprintf (stdout, "%s\n", rcsid);
	setprog (program, argv);
	t4file[1][0] = '\0';
/************************/
/* process command line */
/************************/
	for (n = 0, i = 1; i < argc; i++) {
		if (*argv[i] == '-') {
			strcpy (command, argv[i]); str = command;
			while (c = *str++) switch (c) {
				case 'd': debug++;		break;
				case 'u': noscale++;		break;
			}
		} else if (n < 2) {
			strcpy (t4file[n++], argv[i]);
		}
	}
	if (n < 1) {
		printf ("Usage:	%s <t4file> [inv_t4file]\n", program);
		printf ("e.g.,	%s vm11b_anat_ave_to_vm11b_234-3_t4 [vm11b_234-3_to_vm11b_anat_ave_t4]\n", program);
		printf ("\toption\n");
		printf ("\t-u	suppress (intensity) scale field in output\n");
		exit (1);
	}
	
	for (k = 0; k < 1; k++) {
		if (!(fp = fopen (t4file[k], "r"))) errr (program, t4file[k]);
		printf ("Reading: %s\n", t4file[k]);
		done = 0; while (fgets (string, MAXL, fp)) {
			if (!(m = split (string, srgv, MAXL))) continue;		/* skip blank lines */
			if (!strcmp (srgv[0], "scale:")) scale[k] = atof (srgv[1]);
			if (m == 1 & !strcmp (srgv[0], "t4")) {
				if (t4read (fp, t4[k])) errr (program, t4file[k]);
				done++;
			}
		}
		if (!done) {
			rewind (fp);
			if (t4read (fp, t4[k])) errr (program, t4file[k]);
		}
		fclose (fp);
		t4list (stdout, t4[k]);
		printf ("scale: %.6f\n", scale[k]);
	}	

	if (!strlen (t4file[1])) {
/*************************************************/
/* automatically compute name of inverted t4file */
/*************************************************/
		if (!(str = strrchr (t4file[0], '/'))) str = t4file[0]; else str++;
		strcpy (string, str);
		if (!(str1 = strstr (string, "_to_"))) exit (1);
		*str1 = '\0';
		strcpy (imgfile[0], string);
		str1 += 4;
		if (!(str2 = strstr (str1, "_t4"))) exit (1);
		*str2 = '\0';
		strcpy (imgfile[1], str1);
		sprintf (t4file[1], "%s_to_%s_t4", imgfile[1], imgfile[0]);
	}

	t4inv_ (t4[0], t4[1]);
	t4list (stdout, t4[1]);
	if (!noscale && scale[0] != 0) {
		scale[1] = 1./scale[0];
		printf ("scale: %.6f\n", scale[1]);
	}

	if (!(fp = fopen (t4file[1], "w"))) errw (program, t4file[1]);
	printf ("Writing: %s\n", t4file[1]);
	write_command_line (fp, argc, argv);
	t4list (fp, t4[1]);
	if (scale[1] != 0) fprintf (fp, "scale:    %10.6f\n", scale[1]);
	if (fclose (fp)) errw (program, t4file[1]);

	exit (0);
}
