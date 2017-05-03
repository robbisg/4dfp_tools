#$Header: /data/petsun4/data1/src_solaris/algebra_4dfp/RCS/ROI_resolve_multivol_4dfp.mak,v 1.1 2014/08/09 22:26:57 avi Exp $
#$Log: ROI_resolve_multivol_4dfp.mak,v $
# Revision 1.1  2014/08/09  22:26:57  avi
# Initial revision
#
PROG	= ROI_resolve_multivol_4dfp
CSRCS	= ${PROG}.c
FSRCS	=
OBJS	= ${CSRCS:.c=.o} ${FSRCS:.f=.o}
TRX	= ${NILSRC}/TRX
LOBJS	= ${TRX}/rec.o ${TRX}/Getifh.o ${TRX}/endianio.o ${NILSRC}/t4imgs_4dfp/cvrtflip.o

CFLAGS	= -I${TRX} -O
ifeq (${OSTYPE}, linux)
	CC	= gcc ${CFLAGS}
else
	CC	= cc  ${CFLAGS}
endif

.c.o:
	${CC} -c $<
.f.o:
	${FC} -c $<

${PROG}: ${OBJS} 
	${CC} -o $@ ${OBJS} ${LOBJS} -lm

release: ${PROG}
	chmod 755 ${PROG}
	/bin/mv ${PROG} ${RELEASE}

clean:
	/bin/rm ${OBJS} ${PROG}
