/****************************************************************************************/
/* Copyright 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,	*/
/* 2011, 2012, 2013, 2014					        		*/
/* Washington University, Mallinckrodt Institute of Radiology.                       	*/
/* All Rights Reserved.                                                              	*/
/* This software may not be reproduced, copied, or distributed without written       	*/
/* permission of Washington University. For further information contact A. Z. Snyder 	*/
/****************************************************************************************/
/*$Header: /data/petsun4/data1/src_solaris/TRX/RCS/rec.c,v 1.12 2014/08/23 00:29:02 avi Exp $*/
/*$Log: rec.c,v $
 * Revision 1.12  2014/08/23  00:29:02  avi
 * MAX -> 2048
 * strcat -> strncat; strcpy -> strncpy
 *
 * Revision 1.11  2007/05/03  22:28:14  avi
 * gcc -Wall
 *
 * Revision 1.10  2006/09/29  22:53:33  avi
 * MAXL -> 1024
 *
 * Revision 1.9  2006/09/23  20:52:50  avi
 *  startrecle()  startrece () final argument now control
 *
 * Revision 1.8  2006/09/23  06:30:33  avi
 * provision for variable endian status
 *
 * Revision 1.7  2004/09/03  20:07:01  avi
 * add .conc to special catrec file extensions
 *
 * Revision 1.6  2004/01/14  05:45:52  avi
 * add "Including:" stdout message to catrec ()
 *
 * Revision 1.5  2002/01/07  22:50:31  avi
 * terminate continuation lines with '\\' in startrecl ()
 *
 * Revision 1.4  2000/12/13  06:02:41  avi
 * replace external get_date_log () with get_time_usr ()
 *
 * Revision 1.3  2000/12/13  02:47:57  avi
 * copyright
 *
 * Revision 1.2  1999/01/21  07:51:41  avi
 * prototyping
 *
 * Revision 1.1  1999/01/21  07:20:10  avi
 * Initial revision
 **/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>		/* R_OK and W_OK */
#include <time.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <endianio.h>

#define  MAXL	2028		/* accommodate very long filenames and commands */

/********************/
/* global variables */
/********************/
static char recfile[MAXL] = "";
static char rcsid[] = "$Id: rec.c,v 1.12 2014/08/23 00:29:02 avi Exp $";

void rec_rcsid (void) {printf ("%s\n", rcsid);}

int get_machine_info (char *string) {
	struct utsname u_name;
	int rtn = 1, r;
	
	r = uname (&u_name);	
	if (r >= 0) {
		sprintf (string, "%s %s %s %s", u_name.nodename, u_name.sysname, u_name.release, u_name.machine);
 	} else {
		rtn = 0;	
 	}
 	return rtn; 
}

int printrec (char *string) {
 	FILE		*recfp;

	if (!(recfp = fopen (recfile, "a"))) {
	 	fprintf (stderr, "printrec: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "%s", string);
	fclose (recfp);
	return 0;
}

int catrec (char *file) {
	FILE		*recfp;
	char		filerec[MAXL], command[MAXL];
	int		k, debug = 0;
	int		isimg;

	if (access (recfile, W_OK)) {
		fprintf (stderr, "catrec: recfile not initialized\n");
		return -1;
	}
	strncpy (filerec, file, MAXL - 1);
	k = strlen (filerec);
	isimg = (!strcmp (filerec + k - 4, ".img"))
	     || (!strcmp (filerec + k - 4, ".trk"))
	     || (!strcmp (filerec + k - 5, ".conc"));
	if (isimg) strncat (filerec, ".rec", MAXL - 1);
	if (access (filerec, R_OK)) {	
		if (!(recfp = fopen (recfile, "a"))) {
	 		fprintf (stderr, "printrec: %s write error\n", recfile);
			return -1;
		}
		fprintf (recfp, "%s not found\n", filerec);
		fclose (recfp);
		return 1;
	} else {
		printf ("including: %s in %s\n", filerec, recfile);
		sprintf (command, "cat -s %s >> %s", filerec, recfile);
		if (debug) fprintf (stderr, "%s\n", command);
		return system (command);
	}
}

int startrec (char *outfile, int argc, char *argv[], char *rcsid) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		*str, string[MAXL];
	int 		k;

	strncpy (string, outfile, MAXL - 1);
	while ((str = strrchr (string, '.'))) {
		if (!strcmp (str, ".rec")) *str = '\0';
		else break;
	}
	strncpy (recfile, string, MAXL - 1);
	strncat (recfile, ".rec", MAXL - 1);
	if (!(recfp = fopen (recfile, "w"))) {
		fprintf (stderr, "startrec: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "rec %s", string);
	get_time_usr (string);
	fprintf (recfp, "  %s\n", string);
	for (k = 0; k < argc; k++) fprintf (recfp, "%s ", argv[k]);
	fprintf (recfp, "\n%s\n", rcsid); 
	fclose (recfp);
	return 0;
}

int startrecl (char *outfile, int argc, char *argv[], char *rcsid) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		*str, string[MAXL];
	int 		k;

	strncpy (string, outfile, MAXL - 1);
	while ((str = strrchr (string, '.'))) {
		if (!strcmp (str, ".rec")) *str = '\0';
		else break;
	}
	strncpy (recfile, string, MAXL - 1);
	strncat (recfile, ".rec", MAXL - 1);
	if (!(recfp = fopen (recfile, "w"))) {
		fprintf (stderr, "startrecl: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "rec %s", string);
	get_time_usr (string);
	fprintf (recfp, "  %s\n", string);
	for (k = 0; k < argc - 1; k++) fprintf (recfp, "\t%s\t\\\n", argv[k]);
	fprintf (recfp, "\t%s\n", argv[k]);
	fprintf (recfp, "%s\n", rcsid); 
	fclose (recfp);
	return 0;
}

int startrece (char *outfile, int argc, char *argv[], char *rcsid, char control) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		*str, string[MAXL];
	int 		k, osbig;

	strncpy (string, outfile, MAXL - 1);
	while ((str = strrchr (string, '.'))) {
		if (!strcmp (str, ".rec")) *str = '\0';
		else break;
	}
	strncpy (recfile, string, MAXL - 1);
	strncat (recfile, ".rec", MAXL - 1);
	if (!(recfp = fopen (recfile, "w"))) {
		fprintf (stderr, "startrece: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "rec %s", string);
	get_time_usr (string);
	fprintf (recfp, "  %s", string);
	if (get_machine_info (string)) {
	 	fprintf (recfp, "@%s\n", string);
	} else {
		fprintf (recfp, "\n");
	}	
	for (k = 0; k < argc; k++) fprintf (recfp, "%s ", argv[k]);
	fprintf (recfp, "\n%s\n", rcsid);
	switch (control) {
		case 'b': case 'B': osbig = 1; break;
		case 'l': case 'L': osbig = 0; break;
		default: osbig = CPU_is_bigendian(); break;
	}
	fprintf (recfp, "%s\n", ((osbig) ? "bigendian" : "littleendian")); 
	fclose (recfp);
	return 0;
}

int startrecle (char *outfile, int argc, char *argv[], char *rcsid, char control) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		*str, string[MAXL];
	int 		k, osbig;

	strncpy (string, outfile, MAXL - 1);
	while ((str = strrchr (string, '.'))) {
		if (!strcmp (str, ".rec")) *str = '\0';
		else break;
	}
	strncpy (recfile, string, MAXL - 1);
	strncat (recfile, ".rec", MAXL - 1);
	if (!(recfp = fopen (recfile, "w"))) {
		fprintf (stderr, "startrecle: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "rec %s", string);
	get_time_usr (string);
	fprintf (recfp, "  %s", string);
	if (get_machine_info (string)) {
	 	fprintf (recfp, "@%s\n", string);
	} else {
		fprintf (recfp,"\n");
	}	
	for (k = 0; k < argc - 1; k++) fprintf (recfp, "\t%s\t\\\n", argv[k]);
	fprintf (recfp, "\t%s\n", argv[k]);
	fprintf (recfp, "%s\n", rcsid); 
	switch (control) {
		case 'b': case 'B': osbig = 1; break;
		case 'l': case 'L': osbig = 0; break;
		default: osbig = CPU_is_bigendian(); break;
	}
	fprintf (recfp, "%s\n", ((osbig) ? "bigendian" : "littleendian")); 
	fclose (recfp);
	return 0;
}

int endrec (void) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		string[MAXL];
	
	if (!(recfp = fopen (recfile, "a"))) {
		fprintf (stderr, "endrec: recfile write error\n");
		return -1;
	}
	get_time_usr (string);
	fprintf (recfp, "endrec %s\n", string);
	fclose (recfp);
	return 0;
}

void get_time_usr (char *string) {
	time_t		time_sec;
	struct passwd	*pw;

	time (&time_sec);
	strcpy (string, ctime (&time_sec));
	string[24] = '\0';
	strncat (string, "  ", MAXL - 1);
	pw = getpwuid(geteuid());
	if ((pw != NULL) && (pw->pw_name != NULL)) {
		strncat (string, pw->pw_name, MAXL - 1);
	} else {
		strncat (string, "UNKNOWN", MAXL - 1);
	}
}
