# Start of "alldeps.mak" definitions
ALLDEPFILES = $(ALLDEPFILES_MAINDIR) $(ALLDEPFILES_SUBDIR)
ALLDEPFILES_MAINDIR=\
a68v-xdep.c\
altos-xdep.c\
am29k-pinsn.c\
am29k-tdep.c\
arm-convert.s\
arm-pinsn.c\
arm-tdep.c\
arm-xdep.c\
convex-pinsn.c\
convex-tdep.c\
convex-xdep.c\
coredep.c\
corelow.c\
exec.c\
fork-child.c\
go32-nat.c\
go32-xdep.c\
gould-pinsn.c\
gould-xdep.c\
h8300-tdep.c\
hp300ux-xdep.c\
hppa-pinsn.c\
hppab-core.c\
hppab-nat.c\
hppab-tdep.c\
hppah-tdep.c\
i386-pinsn.c\
i386-tdep.c\
i386b-nat.c\
i386mach-nat.c\
i386v-nat.c\
i386v4-nat.c\
i387-tdep.c\
i960-pinsn.c\
i960-tdep.c\
infptrace.c\
inftarg.c\
irix4-nat.c\
m68k-pinsn.c\
m68k-tdep.c\
m88k-nat.c\
m88k-pinsn.c\
m88k-tdep.c\
mips-nat.c\
mips-pinsn.c\
mips-tdep.c\
news-xdep.c\
nindy-tdep.c\
ns32k-pinsn.c\
procfs.c\
pyr-pinsn.c\
pyr-tdep.c\
pyr-xdep.c\
remote-adapt.c\
remote-eb.c\
remote-es1800.c\
remote-hms.c\
remote-mm.c\
remote-nindy.c\
remote-st2000.c\
remote-vx.c\
rs6000-nat.c\
rs6000-pinsn.c\
rs6000-tdep.c\
ser-bsd.c\
ser-go32.c\
solib.c\
sparc-nat.c\
sparc-pinsn.c\
sparc-tdep.c\
sun3-nat.c\
sun386-nat.c\
symm-tdep.c\
symm-xdep.c\
tahoe-pinsn.c\
ultra3-nat.c\
ultra3-xdep.c\
umax-xdep.c\
vax-pinsn.c\
xcoffexec.c\
xcoffsolib.c

ALLDEPFILES_SUBDIR=\
nindy-share/Onindy.c\
nindy-share/nindy.c\
nindy-share/ttybreak.c\
nindy-share/ttyflush.c\
vx-share/xdr_ld.c\
vx-share/xdr_ptrace.c\
vx-share/xdr_rdb.c\
vx-share/xdr_regs.c

ALLPARAM=\
tm-29k.h\
tm-3b1.h\
tm-68k-noun.h\
tm-68k-un.h\
tm-altos.h\
tm-amix.h\
tm-arm.h\
tm-bigmips.h\
tm-convex.h\
tm-delta88.h\
tm-es1800.h\
tm-h8300.h\
tm-hp300bsd.h\
tm-hp300hpux.h\
tm-hppab.h\
tm-hppah.h\
tm-i386bsd.h\
tm-i386v.h\
tm-i386v4.h\
tm-irix3.h\
tm-isi.h\
tm-linux.h\
tm-m88k.h\
tm-merlin.h\
tm-mips.h\
tm-news.h\
tm-nindy960.h\
tm-np1.h\
tm-os68k.h\
tm-pn.h\
tm-pyr.h\
tm-rs6000.h\
tm-spc-noun.h\
tm-spc-un.h\
tm-st2000.h\
tm-sun2.h\
tm-sun2os4.h\
tm-sun3.h\
tm-sun386.h\
tm-sun3os4.h\
tm-sun4os4.h\
tm-sun4sol2.h\
tm-symmetry.h\
tm-tahoe.h\
tm-ultra3.h\
tm-umax.h\
tm-vax.h\
tm-vx68.h\
tm-vx960.h\
xm-3b1.h\
xm-altos.h\
xm-amix.h\
xm-apollo68b.h\
xm-apollo68v.h\
xm-arm.h\
xm-bigmips.h\
xm-convex.h\
xm-delta88.h\
xm-go32.h\
xm-hp300bsd.h\
xm-hp300hpux.h\
xm-hppab.h\
xm-hppah.h\
xm-i386bsd.h\
xm-i386mach.h\
xm-i386sco.h\
xm-i386v.h\
xm-i386v32.h\
xm-i386v4.h\
xm-irix3.h\
xm-irix4.h\
xm-isi.h\
xm-linux.h\
xm-m88k.h\
xm-merlin.h\
xm-mips.h\
xm-news.h\
xm-news1000.h\
xm-np1.h\
xm-pn.h\
xm-pyr.h\
xm-rs6000.h\
xm-rtbsd.h\
xm-sun2.h\
xm-sun3.h\
xm-sun386.h\
xm-sun3os4.h\
xm-sun4os4.h\
xm-sun4sol2.h\
xm-symmetry.h\
xm-tahoe.h\
xm-ultra3.h\
xm-umax.h\
xm-vaxbsd.h\
xm-vaxult.h

ALLCONFIG=\
./config/3b1.mh\
./config/3b1.mt\
./config/a29k-kern.mt\
./config/a29k.mt\
./config/altos.mh\
./config/altos.mt\
./config/amix.mh\
./config/amix.mt\
./config/apollo68b.mh\
./config/apollo68v.mh\
./config/arm.mh\
./config/arm.mt\
./config/bigmips.mh\
./config/bigmips.mt\
./config/convex.mh\
./config/convex.mt\
./config/decstation.mh\
./config/decstation.mt\
./config/delta88.mh\
./config/delta88.mt\
./config/es1800.mt\
./config/go32.mh\
./config/h8300hms.mt\
./config/hp300bsd.mh\
./config/hp300bsd.mt\
./config/hp300hpux.mh\
./config/hp300hpux.mt\
./config/hppabsd.mh\
./config/hppabsd.mt\
./config/hppahpux.mh\
./config/hppahpux.mt\
./config/i386aout.mt\
./config/i386bsd.mh\
./config/i386bsd.mt\
./config/i386mach.mh\
./config/i386sco.mh\
./config/i386sol2.mh\
./config/i386sol2.mt\
./config/i386v.mh\
./config/i386v.mt\
./config/i386v32.mh\
./config/i386v4.mh\
./config/i386v4.mt\
./config/irix3.mh\
./config/irix3.mt\
./config/irix4.mh\
./config/isi.mh\
./config/isi.mt\
./config/linux.mh\
./config/linux.mt\
./config/littlemips.mh\
./config/littlemips.mt\
./config/m68k-noun.mt\
./config/m68k-un.mt\
./config/m88k.mh\
./config/m88k.mt\
./config/merlin.mh\
./config/merlin.mt\
./config/ncr3000.mh\
./config/ncr3000.mt\
./config/news.mh\
./config/news.mt\
./config/news1000.mh\
./config/nindy960.mt\
./config/none.mh\
./config/none.mt\
./config/np1.mh\
./config/np1.mt\
./config/os68k.mt\
./config/pn.mh\
./config/pn.mt\
./config/pyramid.mh\
./config/pyramid.mt\
./config/rs6000.mh\
./config/rs6000.mt\
./config/rtbsd.mh\
./config/sparc-noun.mt\
./config/sparc-un.mt\
./config/st2000.mt\
./config/sun2os3.mh\
./config/sun2os3.mt\
./config/sun2os4.mh\
./config/sun2os4.mt\
./config/sun386.mh\
./config/sun386.mt\
./config/sun3os3.mh\
./config/sun3os3.mt\
./config/sun3os4.mh\
./config/sun3os4.mt\
./config/sun4os4.mh\
./config/sun4os4.mt\
./config/sun4sol2.mh\
./config/sun4sol2.mt\
./config/symmetry.mh\
./config/symmetry.mt\
./config/tahoe.mh\
./config/tahoe.mt\
./config/ultra3.mh\
./config/ultra3.mt\
./config/umax.mh\
./config/umax.mt\
./config/vax.mt\
./config/vaxbsd.mh\
./config/vaxult.mh\
./config/vxworks68.mt\
./config/vxworks960.mt

# End of "alldeps.mak" definitions
