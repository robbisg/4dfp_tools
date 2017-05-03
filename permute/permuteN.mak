#$Header: /data/petsun4/data1/src_solaris/permute/RCS/permuteN.mak,v 1.2 2014/08/04 03:54:14 avi Exp $
#$Log: permuteN.mak,v $
# Revision 1.2  2014/08/04  03:54:14  avi
# eliminate chgrp program
#
# Revision 1.1  2009/07/08  06:24:29  avi
# Initial revision
#

PROG	= permuteN
CSRCS	= ${PROG}.c permute.c
FSRCS	=
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}
LOBJS	=

CFLAGS	= -O
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
else
	CC	= cc ${CFLAGS}
	LIBS	= -lm
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
