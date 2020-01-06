PROG=proj

SRCS = proj.c mouse.c keyboard.c utils.c video_card.c rtc.h rtc.c rtc_macros.h keyboard.h timer.c video_card.h

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
