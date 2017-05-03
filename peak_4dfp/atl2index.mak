#$Header: /data/petsun4/data1/src_solaris/peak_4dfp/RCS/atl2index.mak,v 1.2 2014/08/04 03:53:44 avi Exp $
#$Log: atl2index.mak,v $
# Revision 1.2  2014/08/04  03:53:44  avi
# eliminate chgrp program
#
# Revision 1.1  2013/12/19  22:42:48  avi
# Initial revision
#

PROG	= atl2index
CSRCS	= ${PROG}.c
FSRCS	=
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}
RMS	= ${NILSRC}/librms
FT4	= ${NILSRC}/t4imgs_4dfp
REG	= ${NILSRC}/imgreg_4dfp
TRX	= ${NILSRC}/TRX
LOBJS	= ${TRX}/Getifh.o ${TRX}/endianio.o ${REG}/imgvalx.o ${FT4}/ft4imgo.o  ${FT4}/set_rnan.o \
	  ${RMS}/param6opr.o ${RMS}/matopr.o ${RMS}/polfit.o

CFLAGS	= -O -I${TRX}
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
	FC	= gcc -O -ffixed-line-length-132 -fno-second-underscore
	Q	= $(wildcard /usr/lib*/libgfortran.so.1)
	ifeq ($(Q), "")
		LIBS	= -lm -lg2c
	else
		LIBS	= -lm -lgfortran
	endif
else
	CC	= cc ${CFLAGS}
	FC	= f77 -O -I4 -e
	LIBS	= -lm
endif

.c.o:
	${CC} -c $<
.f.o:
	${FC} -c $<

${PROG}: ${OBJS} 
	${FC} -o $@ ${OBJS} ${LOBJS} ${LIBS}

release: ${PROG}
	chmod 755 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${OBJS} ${PROG}
