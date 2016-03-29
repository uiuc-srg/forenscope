#!/bin/sh

mount / -o rw,remount
export PATH=/usr/X11R6/bin:$PATH

while true
do
  echo -n "
 - Run [B]ochs
 - Run [S]hell
 - [R]eboot
Choose an option: "
  read
  case $REPLY in
    B|b)
      startx $(which bochs-bin) -qf /etc/bochs-init/bochsrc
    ;;
    S|s)
      login
    ;;
    R|r)
      sync
      mount / -o ro,remount
      reboot
    ;;
  esac
done

