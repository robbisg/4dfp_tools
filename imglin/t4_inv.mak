#$Id: t4_inv.mak,v 1.5 2014/08/04 03:51:23 avi Exp $
#$Log: t4_inv.mak,v $
# Revision 1.5  2014/08/04  03:51:23  avi
# eliminate chgrp program
#
# Revision 1.4  2009/02/25  21:29:26  avi
# accommodate 64bit architecture
#
# Revision 1.3  2007/09/21  21:59:07  avi
# gcc v3 and v4 compliant
#
# Revision 1.2  2007/05/01  01:19:25  avi
# new C main version (gcc v3 compliant)[20~
#

PROG	= t4_inv
CSRCS	= ${PROG}.c
FSRCS	= t4inv.f
RMS	= ${NILSRC}/librms
LOBJS	= ${RMS}/matopr.o
OBJS	= ${FSRCS:.f=.o} ${CSRCS:.c=.o}

CFLAGS	= -O -I${RMS}
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
	mv ${PROG} ${RELEASE}

checkout:
	co $(CSRCS) $(FSRCS) 

checkin:
	ci $(CSRCS) $(FSRCS) 
