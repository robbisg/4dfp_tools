#$Id: t4_ident.mak,v 1.3 2014/08/04 03:51:18 avi Exp $
#$Lo$

PROG	= t4_ident
CSRCS	= ${PROG}.c
FSRCS	=
OBJS	= ${FSRCS:.f=.o} ${CSRCS:.c=.o}

CFLAGS	= -O
CC	= cc ${CFLAGS}
LIBS	= -lm

${PROG}: ${OBJS}
	$(CC) -o $@ ${OBJS} ${LOBJS} ${LIBS}

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
