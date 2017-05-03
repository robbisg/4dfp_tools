#$Header: /data/petsun4/data1/src_solaris/interp_4dfp/RCS/bandpass_4dfp.mak,v 1.10 2014/08/04 03:51:58 avi Exp $
#$Log: bandpass_4dfp.mak,v $
# Revision 1.10  2014/08/04  03:51:58  avi
# eliminate chgrp program
#
# Revision 1.9  2014/04/14  02:41:04  avi
# gap.c and ${ACT}/expandf.o
#
# Revision 1.8  2014/04/13  02:12:56  avi
# pbutt1d.f
#
# Revision 1.7  2011/02/06  08:26:24  avi
# include pbutt1d.f
#
# Revision 1.6  2007/11/20  03:18:16  avi
# linux compatible (fftsun.o -> fftsol.o)
#
# Revision 1.5  2007/09/10  20:35:29  avi
# eliminate -lrms
#
# Revision 1.4  2006/09/25  16:53:04  avi
# ${PROG} ${RELEASE}
#
# Revision 1.3  2006/08/07  03:25:35  avi
# new ${TRX}
#
# Revision 1.2  2005/06/29  04:18:42  avi
#

PROG	= bandpass_4dfp
CSRCS	= ${PROG}.c gap.c
FSRCS	= butt1d.f pbutt1dc.f
TRX	= ${NILSRC}/TRX
RMS	= ${NILSRC}/librms
ACT	= ${NILSRC}/actmapf_4dfp
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}
LOBJS	= ${TRX}/rec.o ${TRX}/Getifh.o ${TRX}/endianio.o ${ACT}/conc.o ${ACT}/expandf.o \
	  ${RMS}/fftsol.o ${RMS}/npad.o

.c.o:
	${CC} -c $<

.f.o:
	${FC} -c $<

CFLAGS	= -O -I${RMS} -I${TRX} -I${ACT}
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
	FC	= gcc -O -ffixed-line-length-132 -fcray-pointer
	LIBS	= -lm -lgfortran
else
	CC	= cc ${CFLAGS}
	FC	= f77 -O -I4 -e
	LIBS	= -lm
endif

${PROG}: ${OBJS}
	${FC} -o $@ ${OBJS} ${LOBJS} ${LIBS}

clean:
	rm ${OBJS} ${PROG}

checkout:
	co ${CSRCS} ${FSRCS}

checkin:
	ci ${CSRCS} ${FSRCS}

release: ${PROG}
	chmod 751 ${PROG}
	/bin/mv ${PROG} ${RELEASE}
