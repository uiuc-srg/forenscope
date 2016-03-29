# $1 is the pmemsave image
head -c 134217728 $1 > pmemsave-final.img

# Skip the first 1024 bytes of the memdump image
tail -c +1025 memdump.img | head -c 134217728 > memdump-final.img

diff -q pmemsave-final.img memdump-final.img
./diff pmemsave-final.img memdump-final.img 4096
