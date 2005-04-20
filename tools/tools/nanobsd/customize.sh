#! /bin/sh
#
# Customize a nanoBSD flash image
#
# Copyright (c) 2004 Thomas Quinot
#
# See /usr/share/examples/etc/bsd-style-copyright for license terms.
#
# $FreeBSD$
# 
# Usage:
#
# ${CUSTOMIZE} ${WD} ${WORLDDIR} ${.CURDIR} [ ${LOCAL_FILES_LIST} ]

WD=$1
WORLDDIR=$2
CURDIR=$3
LOCAL_FILES_LIST=$4

PKG_DBDIR=${WD}/var/db/pkg
export PKG_DBDIR

# Useful stuff for diskless boot
customizecfg() {
  sed -i "" -e /beastie/d ${WD}/boot/loader.rc
  sed -i "" -e /ttyd0/s/off/on/ ${WD}/etc/ttys
  echo " -h" > ${WD}/boot.config
}

add_CURDIR() {
  case "$1" in
    /*) echo "$1" ;;
    *)  echo "${CURDIR}/$1" ;;
  esac
}

pkg_relocate() {
  sed -e "1,/^@cwd/s#^@cwd #&${WD}#"
}

installlocalfiles() {
  if [ -n "${LOCAL_FILES_LIST}" ]; then
    while read src dest;
    do
      case "x${src}" in
        x#*|x)
          ;;
        x@*)
          eval "for pkg in `add_CURDIR \"\`echo ${src} | sed 's/^@//'\`\"`;
                do pkg_add -M \${pkg} | pkg_relocate | pkg_add -S;
                done"
          ;;
        *)
          dest="${dest:-${src}}"
          mkdir -p ${WD}/`dirname "${dest}x"`
          eval cp -fp `add_CURDIR ${src}` "${WD}/${dest}"
          ;;
      esac
    done < ${LOCAL_FILES_LIST}
  fi
}

movelocaletc() {
  if [ ! -d ${WD}/etc/local -a -d ${WD}/usr/local/etc ]; then
    mv ${WD}/usr/local/etc ${WD}/etc/local
    ln -s ../../etc/local ${WD}/usr/local/etc
  fi
}

customizecfg
installlocalfiles
movelocaletc
