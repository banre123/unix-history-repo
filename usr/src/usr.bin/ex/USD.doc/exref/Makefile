#	@(#)Makefile	6.3 (Berkeley) %G%

DIR=	usd/13.ex
SRCS=	ex.rm
MACROS=	-msU
CLEANFILES=summary.*

paper.ps ${SRCS} summary.${PRINTER}
	${ROFF} ${SRCS} > ${.TARGET}

summary.ps: ex.summary
	${TBL} ex.summary | ${ROFF} > ${.TARGET}

.include <bsd.doc.mk>
