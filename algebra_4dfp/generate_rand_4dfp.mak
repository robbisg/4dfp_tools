#$Header: /data/petsun4/data1/src_solaris/algebra_4dfp/RCS/generate_rand_4dfp.mak,v 1.3 2014/08/04 03:48:12 avi Exp $
#$Log: generate_rand_4dfp.mak,v $
# Revision 1.3  2014/08/04  03:48:12  avi
# eliminate chgrp program
#
# Revision 1.2  2013/12/23  02:58:07  avi
# eliminate -std=c99 which is incompatible with srand48
#
# Revision 1.1  2013/12/23  01:43:34  avi
# Initial revision
#

PROG	= generate_rand_4dfp
CSRCS	= ${PROG}.c
FSRCS	=
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}
TRX	= ${NILSRC}/TRX
LIN	= ${NILSRC}/imglin
LOBJS	= ${TRX}/rec.o ${TRX}/Getifh.o ${TRX}/endianio.o ${LIN}/dnormal.o

CFLAGS	= -I${TRX} -O
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
	LIBS	= -lm
else
	CC	= cc ${CFLAGS}
	LIBS	= -lm -lsunmath
endif

.c.o:
	${CC} -c $<
.f.o:
	${FC} -c $<

${PROG}: ${OBJS}
	${CC} -o $@ ${OBJS} ${LOBJS} ${LIBS}

release: ${PROG}
	chmod 755 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${OBJS} ${PROG}
