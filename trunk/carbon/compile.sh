#!/bin/bash
if [[ ! -h ./sysmodel ]]
then echo "Creating symlink..." && ln -s /projects/lasr/space5/lagniappe/include/sysmodel ./sysmodel
fi
if [[ ! -h ./jar ]]
then echo "Creating jar symlink..." && ln -s /projects/lasr/space5/lagniappe/jar ./jar
fi
echo "Generating new application files..."
java -jar /projects/lasr/space5/lagniappe/jar/lc.jar --app $1.xml
echo "Generating new main file..."
java -jar ./jar/lc.jar --main $1.xml sysmodel/cscamera.xml
#echo "Generating profile files..."
#java -jar ./jar/lc.jar --prof $1.xml sysmodel/cscamera.xml
echo "Compiling..."
cd ../lib/cryptopp/
make
cd - 
make 

