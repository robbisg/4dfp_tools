#$Header: /data/petsun4/data1/src_solaris/actmapf_4dfp/RCS/format2lst.mak,v 1.3 2014/08/04 03:47:21 avi Exp $
#$Log: format2lst.mak,v $
# Revision 1.3  2014/08/04  03:47:21  avi
# eliminate chgrp program
#
# Revision 1.2  2009/07/30  05:41:38  avi
# linux compliant
#

PROG	= format2lst
CSRCS	= ${PROG}.c expandf.c
OBJS	= ${CSRCS:.c=.o}

.c.o:
	${CC} -c $<

CFLAGS	= -I. -O
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
else
	CC	= cc  ${CFLAGS}
endif
LIBS	= -lm

${PROG}: ${OBJS} 
	${CC} -o $@ ${OBJS} -lm

release: ${PROG}
	chmod 771 ${PROG}
	mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${PROG} ${OBJS}
