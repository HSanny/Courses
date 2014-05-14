

current=$(pwd)
cd build/ 
pintos-mkdisk filesys.dsk --filesys-size=2 
pintos -f -q
pintos -p ../../examples/ls -a ./bin/ls -- -q
cd $current
