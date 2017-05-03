#$Header: /data/petsun4/data1/src_solaris/aff_conv/RCS/aff_conv.mak,v 1.4 2014/08/04 03:47:42 avi Exp $
#$Log: aff_conv.mak,v $
# Revision 1.4  2014/08/04  03:47:42  avi
# eliminate chgrp program
#
# Revision 1.3  2012/04/18  23:34:56  coalsont
# remove -std=c99 from compile flags to prevent fscanf from requiring newer glibc
#
# Revision 1.2  2011/11/23  20:48:43  coalsont
# added missing needed header
#
# Revision 1.1  2010/08/16  19:31:41  coalsont
# Initial revision

# Tim Coalson [tsc5yc@mst.edu], NRG, 29 June 2009
# Kevin P. Barry [ta0kira@users.berlios.de], BMCLAB, 22 May 2009

TRX	= ${NILSRC}/TRX
IMGLIN = ${NILSRC}/imglin
NII=${NILSRC}/nifti_4dfp
LOBJS = ${TRX}/endianio.o ${TRX}/Getifh.o ${TRX}/rec.o ${IMGLIN}/t4_io.o \
	${NII}/split.o ${NII}/transform.o ${NII}/4dfp-format.o ${NII}/nifti-format.o
SUFFIXES = .o .c
PROG=aff_conv
ifeq ($(shell uname), SunOS)
	COMPILE=-W -Wall -O2
else
	COMPILE=-W -Wall -O2 -DHAVE_STDINT
endif
INCLUDE=-isystem -I. -I${TRX} -I${IMGLIN} -I${NII}
LINK=-lm -O2
HEADERS=
IHEADERS=${IMGLIN}/t4_io.h ${NII}/common-format.h ${NII}/nifti-format.h ${NII}/4dfp-format.h ${NII}/transform.h
CSRCS=aff_conv.c
OBJECTS=aff_conv.o

.PHONY: all clean

.SUFFIXES: ${SUFFIXES}

all: $(PROG)

.c.o: $(HEADERS) ${IHEADERS} aff_conv.mak
	gcc $(COMPILE) $(INCLUDE) -c $< -o $@

$(PROG): $(OBJECTS) aff_conv.mak
	gcc $(OBJECTS) ${LOBJS} $(LINK) -o $(PROG)

release: ${PROG}
	chmod 771 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	rm -f $(PROG) $(OBJECTS)

checkout:
	co ${CSRCS} ${HEADERS}

checkin:
	ci ${CSRCS} ${HEADERS}

