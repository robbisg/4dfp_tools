#$Header: /data/petsun4/data1/src_solaris/var_4dfp/RCS/set_undefined_4dfp.mak,v 1.2 2014/08/04 04:07:09 avi Exp $
#$Log: set_undefined_4dfp.mak,v $
# Revision 1.2  2014/08/04  04:07:09  avi
# eliminate chgrp program
#
# Revision 1.1  2013/08/23  04:13:16  avi
# Initial revision
#

PROG	= set_undefined_4dfp
CSRCS	= ${PROG}.c
FSRCS	=
LOBJS	=
BLR	= ${NILSRC}/imgblur_4dfp
TRX	= ${NILSRC}/TRX
LOBJS	= ${TRX}/rec.o ${TRX}/Getifh.o ${TRX}/endianio.o ${BLR}/fimgblur.o
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}

.c.o:
	${CC} -c $<

CFLAGS	= -I${TRX} -O
ifeq (${OSTYPE}, linux)
	CC	= gcc -std=c99 ${CFLAGS}	# -std=c99 enables use of isnormal()
	FC	= gcc -O -ffixed-line-length-132 -fcray-pointer
	LIBS	= -lm -lgfortran
else
	CC	= cc ${CFLAGS}
	FC	= f77 -O -I4 -e
	LIBS	= -lm -lsunmath
endif

${PROG}: ${OBJS} 
	${FC} -o ${PROG} ${OBJS} ${LOBJS} ${LIBS}

release: ${PROG}
	chmod 771 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${OBJS} ${PROG}
