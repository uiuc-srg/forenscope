pushd ../..
export MTOOLSRC="config/mtools.conf";
mcopy c:
mv DD-F.img tools/diff/
popd

# $1 is the pmemsave image
head -c 134217728 $1 > pmemsave-final.img

head -c 134217728 dd-f.img > memdump-final.img

diff -q pmemsave-final.img memdump-final.img
./diff pmemsave-final.img memdump-final.img 4096
