/*
          Copyright (C) 1993, 1994, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993, 1994 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993, 1994 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/
/* Copyright marker.  Copyright will be inserted above.  Do not remove */

/*
**				DICOM 93
**		     Electronic Radiology Laboratory
**		   Mallinckrodt Institute of Radiology
**		Washington University School of Medicine
**
** Module Name(s):
** Author, Date:	Stephen M. Moore, 2-Jun-93
** Intent:		This include file defines constants for all of
**			the standard UIDs defined in the DICOM standard.
** Last Update:		$Author: mohanar $, $Date: 2006/08/25 20:55:49 $
** Source File:		$RCSfile: dicom_uids.h,v $
** Revision:		$Revision: 1.1 $
** Status:		$State: Exp $
*/

#ifndef DICOM_UIDS_IS_IN
#define DICOM_UIDS_IS_IN 1

#ifdef  __cplusplus
extern "C" {
#endif

#define	MIR_IMPLEMENTATIONCLASSUID		"1.2.840.113654.2.3.1995.3.0.6"
#define	MIR_IMPLEMENTATIONVERSIONNAME		"MIRCTN11MAR2003"
#define	MIR_SOPCLASSKILLSERVER			"1.2.840.113654.2.30.1"
#define	MIR_SOPCLASSRESETSERVER			"1.2.840.113654.2.30.2"

#define DICOM_SOPCLASSVERIFICATION		"1.2.840.10008.1.1"

#define DICOM_TRANSFERLITTLEENDIAN		"1.2.840.10008.1.2"
#define DICOM_TRANSFERLITTLEENDIANEXPLICIT	"1.2.840.10008.1.2.1"
#define DICOM_TRANSFERBIGENDIANEXPLICIT		"1.2.840.10008.1.2.2"
#define DICOM_TRANSFERJPEGBASELINEPROCESS1	"1.2.840.10008.1.2.4.50"
#define DICOM_TRANSFERJPEGEXTENDEDPROC2AND4	"1.2.840.10008.1.2.4.51"
#define DICOM_TRANSFERJPEGEXTENDEDPROC3AND5	"1.2.840.10008.1.2.4.52"
#define DICOM_TRANSFERJPEGSPECTRALPROC6AND8	"1.2.840.10008.1.2.4.53"
#define DICOM_TRANSFERJPEGSPECTRALPROC7AND9	"1.2.840.10008.1.2.4.54"
#define DICOM_TRANSFERJPEGFULLPROGRESSPROC10AND12 "1.2.840.10008.1.2.4.55"
#define DICOM_TRANSFERJPEGFULLPROGRESSPROC11AND13 "1.2.840.10008.1.2.4.56"
#define DICOM_TRANSFERJPEGLOSSLESSPROC14	"1.2.840.10008.1.2.4.57"
#define DICOM_TRANSFERJPEGLOSSLESSPROC15	"1.2.840.10008.1.2.4.58"
#define DICOM_TRANSFERJPEGEXTENDEDPROC16AND18	"1.2.840.10008.1.2.4.59"
#define DICOM_TRANSFERJPEGEXTENDEDPROC17AND19	"1.2.840.10008.1.2.4.60"
#define DICOM_TRANSFERJPEGSPECTRALPROC20AND22	"1.2.840.10008.1.2.4.61"
#define DICOM_TRANSFERJPEGSPECTRALPROC21AND23	"1.2.840.10008.1.2.4.62"
#define DICOM_TRANSFERJPEGFULLPROGRESSPROC24AND26 "1.2.840.10008.1.2.4.63"
#define DICOM_TRANSFERJPEGFULLPROGRESSPROC25AND27 "1.2.840.10008.1.2.4.64"
#define DICOM_TRANSFERJPEGLOSSLESSPROC28	"1.2.840.10008.1.2.4.65"
#define DICOM_TRANSFERJPEGLOSSLESSPROC29	"1.2.840.10008.1.2.4.66"
#define DICOM_TRANSFERJPEGLOSSLESSPROCFIRSTORDERREDICT "1.2.840.10008.1.2.4.70"


/* Define the UIDS for the service classes defined by the DICOM standard
*/
#define DICOM_SOPCLASSBASICSTUDYCONTENTNOTIFICATION "1.2.840.10008.1.9"
#define	DICOM_SOPCLASSSTORAGECOMMITMENTPUSHMODEL "1.2.840.10008.1.20.1"
#define	DICOM_WELLKNOWNSTORAGECOMMITMENTPUSHMODEL "1.2.840.10008.1.20.1.1"
#define	DICOM_SOPCLASSSTORAGECOMMITMENTPULLMODEL "1.2.840.10008.1.20.2"
#define	DICOM_WELLKNOWNSTORAGECOMMITMENTPULLMODEL "1.2.840.10008.1.20.2.1"

#define DICOM_STDAPPLICATIONCONTEXT		"1.2.840.10008.3.1.1.1"

#define	DICOM_SOPCLASSDETACHEDPATIENTMGMT	"1.2.840.10008.3.1.2.1.1"
#define DICOM_SOPCLASSDETACHEDPATIENTMGMTMETA	"1.2.840.10008.3.1.2.1.4"
#define DICOM_SOPCLASSDETACHEDVISITMGMT		"1.2.840.10008.3.1.2.2.1"
#define DICOM_SOPCLASSDETACHEDSTUDYMGMT		"1.2.840.10008.3.1.2.3.1"
#define DICOM_SOPCLASSSTUDYCOMPONENTMGMT	"1.2.840.10008.3.1.2.3.2"
#define DICOM_SOPCLASSMPPS			"1.2.840.10008.3.1.2.3.3"
#define DICOM_SOPCLASSDETACHEDRESULTSMGMT	"1.2.840.10008.3.1.2.5.1"
#define DICOM_SOPCLASSDETACHEDRESULTSMGMTMETA	"1.2.840.10008.3.1.2.5.4"
#define DICOM_SOPCLASSDETACHEDSTUDYMGMTMETA	"1.2.840.10008.3.1.2.5.5"
#define DICOM_SOPCLASSDETACHEDINTERPRETMGMT	"1.2.840.10008.3.1.2.6.1"

#define DICOM_SOPCLASSBASICFILMSESSION		"1.2.840.10008.5.1.1.1"
#define	DICOM_SOPCLASSBASICFILMBOX		"1.2.840.10008.5.1.1.2"
#define	DICOM_SOPCLASSBASICGREYSCALEIMAGEBOX	"1.2.840.10008.5.1.1.4"
#define	DICOM_SOPCLASSBASICCOLORIMAGEBOX	"1.2.840.10008.5.1.1.4.1"
#define	DICOM_SOPCLASSREFERENCEDIMAGEBOX	"1.2.840.10008.5.1.1.4.2"
#define	DICOM_SOPCLASSGREYSCALEPRINTMGMTMETA	"1.2.840.10008.5.1.1.9"
#define	DICOM_SOPCLASSREFGREYSCALEPRINTMGMTMETA	"1.2.840.10008.5.1.1.9.1"
#define	DICOM_SOPCLASSPRINTJOB			"1.2.840.10008.5.1.1.14"
#define	DICOM_SOPCLASSBASICANNOTATIONBOX	"1.2.840.10008.5.1.1.15"
#define	DICOM_SOPCLASSPRINTER			"1.2.840.10008.5.1.1.16"
#define	DICOM_SOPPRINTERINSTANCE		"1.2.840.10008.5.1.1.17"
#define	DICOM_SOPCLASSCOLORPRINTMGMTMETA	"1.2.840.10008.5.1.1.18"
#define	DICOM_SOPCLASSREFCOLORPRINTMGMTMETA	"1.2.840.10008.5.1.1.18.1"
#define DICOM_SOPCLASSVOILUT			"1.2.840.10008.5.1.1.22"
#define DICOM_SOPCLASSIMAGEOVERLAYBOX		"1.2.840.10008.5.1.1.24"

#define	DICOM_SOPCLASSSTOREDPRINT		"1.2.840.10008.5.1.1.27"
#define	DICOM_SOPCLASSHARDCOPYGRAYSCALEIMAGE	"1.2.840.10008.5.1.1.29"
#define	DICOM_SOPCLASSHARDCOPYCOLORIMAGE	"1.2.840.10008.5.1.1.30"

#define	DICOM_SOPCLASSCOMPUTEDRADIOGRAPHY	"1.2.840.10008.5.1.4.1.1.1"
#define DICOM_SOPCLASSDIGXRAYPRESENTATION       "1.2.840.10008.5.1.4.1.1.1.1"
#define DICOM_SOPCLASSDIGXRAYPROCESSING         "1.2.840.10008.5.1.4.1.1.1.1.1"
#define DICOM_SOPCLASSMAMMOXRPRESENTATION       "1.2.840.10008.5.1.4.1.1.1.2"
#define DICOM_SOPCLASSMAMMOXRPROCESSING         "1.2.840.10008.5.1.4.1.1.1.2.1"
#define DICOM_SOPCLASSINTRAORALPRESENTATION     "1.2.840.10008.5.1.4.1.1.1.3"
#define DICOM_SOPCLASSINTRAORALPROCESSING       "1.2.840.10008.5.1.4.1.1.1.3.1"
#define	DICOM_SOPCLASSCT			"1.2.840.10008.5.1.4.1.1.2"
#define	DICOM_SOPCLASSUSMULTIFRAMEIMAGE1993	"1.2.840.10008.5.1.4.1.1.3"
#define	DICOM_SOPCLASSUSMULTIFRAMEIMAGE		"1.2.840.10008.5.1.4.1.1.3.1"
#define	DICOM_SOPCLASSMR			"1.2.840.10008.5.1.4.1.1.4"
#define	DICOM_SOPCLASSNM1993			"1.2.840.10008.5.1.4.1.1.5"
#define	DICOM_SOPCLASSUS1993			"1.2.840.10008.5.1.4.1.1.6"
#define	DICOM_SOPCLASSUS			"1.2.840.10008.5.1.4.1.1.6.1"
#define	DICOM_SOPCLASSSECONDARYCAPTURE		"1.2.840.10008.5.1.4.1.1.7"
#define DICOM_SOPCLASSSTANDALONEOVERLAY		"1.2.840.10008.5.1.4.1.1.8"
#define DICOM_SOPCLASSSTANDALONECURVE		"1.2.840.10008.5.1.4.1.1.9"
#define	DICOM_SOPCLASSWAVEFORMSTORAGE		"1.2.840.10008.5.1.4.1.1.9.1"
#define	DICOM_SOPCLASSECGWAVEFORMSTORAGE	"1.2.840.10008.5.1.4.1.1.9.1.1"
#define DICOM_SOPCLASSSTANDALONEMODALITYLUT	"1.2.840.10008.5.1.4.1.1.10"
#define DICOM_SOPCLASSSTANDALONEVOILUT		"1.2.840.10008.5.1.4.1.1.11"
#define DICOM_SOPCLASSGREYSCALEPS		"1.2.840.10008.5.1.4.1.1.11.1"
#define	DICOM_SOPCLASSXRAYANGIO			"1.2.840.10008.5.1.4.1.1.12.1"
#define	DICOM_SOPCLASSXRAYFLUORO		"1.2.840.10008.5.1.4.1.1.12.2"
#define DICOM_SOPCLASSXRAYANGIOBIPLANE_RET      "1.2.840.10008.5.1.4.1.1.12.3"
#define	DICOM_SOPCLASSNM			"1.2.840.10008.5.1.4.1.1.20"
#define DICOM_SOPCLASSVLENDOSCOPIC              "1.2.840.10008.5.1.4.1.1.77.1.1"
#define DICOM_SOPCLASSVLMICROSCOPIC             "1.2.840.10008.5.1.4.1.1.77.1.2"
#define DICOM_SOPCLASSVLSLIDEMICROSCOPIC        "1.2.840.10008.5.1.4.1.1.77.1.3"
#define DICOM_SOPCLASSVLPHOTOGRAPHIC            "1.2.840.10008.5.1.4.1.1.77.1.4"
#define	DICOM_SOPCLASSBASICTEXTSR		"1.2.840.10008.5.1.4.1.1.88.11"
#define	DICOM_SOPCLASSENHANCEDSR		"1.2.840.10008.5.1.4.1.1.88.22"
#define	DICOM_SOPCLASSCOMPREHENSIVESR		"1.2.840.10008.5.1.4.1.1.88.33"
#define	DICOM_SOPCLASSKEYOBJECTNOTE		"1.2.840.10008.5.1.4.1.1.88.59"
#define	DICOM_SOPCLASSPET			"1.2.840.10008.5.1.4.1.1.128"
#define	DICOM_SOPCLASSSTANDALONEPETCURVE	"1.2.840.10008.5.1.4.1.1.129"
#define DICOM_SOPRTIMAGESTORAGE			"1.2.840.10008.5.1.4.1.1.481.1"
#define DICOM_SOPRTDOSESTORAGE			"1.2.840.10008.5.1.4.1.1.481.2"
#define DICOM_SOPRTSTRUCTURESETSTORAGE		"1.2.840.10008.5.1.4.1.1.481.3"
#define DICOM_SOPRTBREAMS                       "1.2.840.10008.5.1.4.1.1.481.4"
#define DICOM_SOPRTPLANSTORAGE			"1.2.840.10008.5.1.4.1.1.481.5"
#define DICOM_SOPRTBRACHYTREATMENT              "1.2.840.10008.5.1.4.1.1.481.6"
#define DICOM_SOPRTTREATMENTSUMMARY             "1.2.840.10008.5.1.4.1.1.481.7"

#define	DICOM_SOPPATIENTQUERY_FIND		"1.2.840.10008.5.1.4.1.2.1.1"
#define	DICOM_SOPPATIENTQUERY_MOVE		"1.2.840.10008.5.1.4.1.2.1.2"
#define	DICOM_SOPPATIENTQUERY_GET		"1.2.840.10008.5.1.4.1.2.1.3"

#define	DICOM_SOPSTUDYQUERY_FIND		"1.2.840.10008.5.1.4.1.2.2.1"
#define	DICOM_SOPSTUDYQUERY_MOVE		"1.2.840.10008.5.1.4.1.2.2.2"
#define	DICOM_SOPSTUDYQUERY_GET			"1.2.840.10008.5.1.4.1.2.2.3"

#define	DICOM_SOPPATIENTSTUDYQUERY_FIND		"1.2.840.10008.5.1.4.1.2.3.1"
#define	DICOM_SOPPATIENTSTUDYQUERY_MOVE		"1.2.840.10008.5.1.4.1.2.3.2"
#define	DICOM_SOPPATIENTSTUDYQUERY_GET		"1.2.840.10008.5.1.4.1.2.3.3"

#define	DICOM_SOPMODALITYWORKLIST_FIND		"1.2.840.10008.5.1.4.31"

#define	DICOM_SOPGPWORKLIST_FIND		"1.2.840.10008.5.1.4.32.1"
#define	DICOM_SOPGPSPS				"1.2.840.10008.5.1.4.32.2"
#define	DICOM_SOPGPPPS				"1.2.840.10008.5.1.4.32.3"

typedef enum {
    UID_PATIENT = 2,
    UID_VISIT,
    UID_STUDY,
    UID_SERIES,
    UID_IMAGE,
    UID_RESULTS,
    UID_INTERPRETATION,
    UID_PRINTER,
    UID_DEVICE,
    UID_STUDYCOMPONENT,
    UID_STORAGECOMMITTRANSACTION
}   UID_TYPE;

typedef struct {
    unsigned long patient;
    unsigned long visit;
    unsigned long study;
    unsigned long series;
    unsigned long image;
    unsigned long results;
    unsigned long interpretation;
    unsigned long printer;
    unsigned long deviceType;
    unsigned long serialNumber;
    unsigned long studyComponent;
    unsigned long storageCommitTransaction;
    char root[DICOM_UI_LENGTH + 1];
}   UID_BLOCK;

typedef enum {
    UID_CLASS_K_APPLICATIONCONTEXT,
    UID_CLASS_K_IMPLEMENTATION,
    UID_CLASS_K_SOPCLASS,
    UID_CLASS_K_METASOPCLASS,
    UID_CLASS_K_TRANSFERSYNTAX,
    UID_CLASS_K_WELLKNOWNUID
}   UID_CLASS;

typedef struct {
    UID_CLASS UIDclass;
    char UID[DICOM_UI_LENGTH + 1];
    char description[64];
    char originator[32];
}   UID_DESCRIPTION;

CONDITION
UID_NewUID(UID_TYPE type, char *uid);
CONDITION
UID_NewNumber(UID_TYPE type, unsigned long *value);
CONDITION
UID_Lookup(char *UID, UID_DESCRIPTION * description);
void
UID_ScanDictionary(void (*callback)(const UID_DESCRIPTION *d1, void *ctx1),
	void *ctx);
char *UID_Message(CONDITION cond);
char* UID_Translate(const char* value);
int UID_IsStorageClass(const char* sopClassUID);

#define	UID_NORMAL		FORM_COND(FAC_UID, SEV_SUCC, 1)
#define	UID_NOUIDFILENAME	FORM_COND(FAC_UID, SEV_ERROR, 2)
#define	UID_GENERATEFAILED	FORM_COND(FAC_UID, SEV_ERROR, 3)
#define	UID_FILEOPENFAILURE	FORM_COND(FAC_UID, SEV_ERROR, 5)
#define	UID_FILECREATEFAILURE	FORM_COND(FAC_UID, SEV_ERROR, 6)
#define	UID_ILLEGALROOT		FORM_COND(FAC_UID, SEV_ERROR, 7)
#define	UID_ILLEGALNUMERIC	FORM_COND(FAC_UID, SEV_ERROR, 8)
#define	UID_NODEVICETYPE	FORM_COND(FAC_UID, SEV_ERROR, 9)
#define	UID_NOROOT		FORM_COND(FAC_UID, SEV_ERROR, 10)
#define UID_UIDNOTFOUND		FORM_COND(FAC_UID, SEV_ERROR, 11)

#ifdef  __cplusplus
}
#endif

#endif
