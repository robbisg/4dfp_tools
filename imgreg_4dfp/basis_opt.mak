PROG	= basis_opt
CSRCS	= ${PROG}.c
FSRCS	= 
RMS	= ${NILSRC}/librms
TRX	= ${NILSRC}/TRX
OBJS	= ${FSRCS:.f=.o} ${CSRCS:.c=.o}
LOBJS	= ${TRX}/rec.o ${TRX}/Getifh.o ${TRX}/endianio.o ${RMS}/dmatopr.o ${RMS}/dmatinv.o ${RMS}/deigen.o
LIBS	= -lm

CFLAGS	= -O -I${RMS} -I${TRX}
ifeq (${OSTYPE}, unix)
	CC	= cc ${CFLAGS}
	FC	= f77 -O -I4 -e
	LIBS	= -lm
else
	CC	= gcc ${CFLAGS}
	FC	= gcc -O -ffixed-line-length-132 -fno-second-underscore -fcray-pointer
	LIBS	= -lm -lgfortran
endif

${PROG}: ${OBJS}
	$(FC) -o $@ ${OBJS} ${LOBJS} ${LIBS}

.c.o:
	$(CC) -c $<

.f.o:
	$(FC) -c $<

clean:
	/bin/rm ${OBJS} ${PROG}

release: ${PROG} 
	chmod 771 ${PROG}
	chgrp program ${PROG}
	mv ${PROG} ${RELEASE}

checkout:
	co $(CSRCS) $(FSRCS) 

checkin:
	ci $(CSRCS) $(FSRCS) 
