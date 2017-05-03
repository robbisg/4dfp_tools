#$Header: /data/petsun4/data1/src_solaris/actmapf_4dfp/RCS/LOWESS.mak,v 1.1 2014/08/04 03:46:05 avi Exp $
#$Log: LOWESS.mak,v $
# Revision 1.1  2014/08/04  03:46:05  avi
# Initial revision
#

PROG	= LOWESS
CSRCS	= ${PROG}.c
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}
TRX	= ${NILSRC}/TRX
RMS	= ${NILSRC}/librms
LOBJS	= ${RMS}/dmatinv.o
LIBS	= -lm

.c.o:
	${CC} -c $<

.f.o:
	${FC} -c $<

CFLAGS	= -I. -I${TRX} -O
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

release: ${PROG}
	chmod 771 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${OBJS} ${PROG}
