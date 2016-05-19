#!/bin/bash
set -e

export LD_LIBRARY_PATH=.:`cat /etc/ld.so.conf.d/* | grep -v -E "#" | tr "\\n" ":" | sed -e "s/:$//g"`
sudo add-apt-repository -y ppa:kevinkreiser/prime-server
sudo apt-get update -o Dir::Etc::sourcelist="sources.list.d/kevinkreiser-prime-server-$(lsb_release -c -s).list" -o Dir::Etc::sourceparts="-" -o APT::Get::List-Cleanup="0"
sudo apt-get install -y autoconf automake pkg-config libtool make pkg-config gcc g++ lcov libboost1.54-all-dev libprime-server-dev
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ -n "$1" ] && [ -d "$1" ]; then
        pushd "$1"
else
        pushd .
fi

#clone async
mkdir -p deps
for dep in midgard baldr; do
	rm -rf $dep
	(
		git clone --depth=1 --recurse-submodules --single-branch --branch=master https://github.com/valhalla/$dep.git deps/$dep
		git fetch origin 'refs/tags/*:refs/tags/*'
	) &
done
wait

#build sync
for dep in midgard baldr; do
	pushd $dep
	./autogen.sh
	./configure CPPFLAGS="-DBOOST_SPIRIT_THREADSAFE -DBOOST_NO_CXX11_SCOPED_ENUMS"
	make -j4
	sudo make install
	popd
done
wait

popd
