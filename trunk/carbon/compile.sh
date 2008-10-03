#!/bin/bash
if [[ ! -h ./sysmodel ]]
then echo "Creating sysmodel symlink..." && ln -s /usr/local/include/sysmodel ./sysmodel
fi
if [[ ! -h ./jar ]]
then echo "Creating jar symlink..." && ln -s /usr/local/jar ./jar
fi
echo "Generating new application files..."
java -jar ./jar/lc.jar --app $1.xml
echo "Generating new main file..."
java -jar ./jar/lc.jar --main $1.xml sysmodel/proc16sys.xml
#echo "Generating profile files..."
#java -jar ./jar/lc.jar --prof $1.xml sysmodel/proc16sys.xml
echo "Compiling..."
make 
