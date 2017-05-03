#$Header: /data/petsun4/data1/src_solaris/jitter/RCS/jitter.mak,v 1.2 2014/08/04 03:52:13 avi Exp $
#$Log: jitter.mak,v $
# Revision 1.2  2014/08/04  03:52:13  avi
# eliminate chgrp program
#
# Revision 1.1  2014/01/04  03:26:53  avi
# Initial revision
#

PROG	= jitter
CSRCS	= ${PROG}.c
FSRCS	=
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}

CFLAGS	= -O
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
	LIBS	= -lm
else
	CC	= cc ${CFLAGS}
	LIBS	= -lm
endif

.c.o:
	${CC} -c $<
.f.o:
	${FC} -c $<

${PROG}: ${OBJS} 
	${CC} -o $@ ${OBJS} -lm

release: ${PROG}
	chmod 755 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${OBJS} ${PROG}
