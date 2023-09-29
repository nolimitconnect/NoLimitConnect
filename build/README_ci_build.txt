=== use Docker Image for building nolimitconnect ===
# install ability to run docker images from https://www.docker.com
# you will need a docker account
# special thanks to carlonluca for doing all the hard work of creating images with qt build tools
# carlonluca blog is https://bugfreeblog.duckdns.org/

NOTE: docker image version will likely change in the future
At the time of this written the version is 1.0.3

# get the docker image from docker repo
docker pull brettrjonesdocker/nolimitappimage:nlcdevimage-1.0.3

# run the docker image
docker run -it brettrjonesdocker/nolimitappimage:nlcdevimage-1.0.3 bash

# check out source code
git clone https://gitlab.com/nolimitconnectapps/nolimitapp.git

# cmake only requires one special parameter that defines TARGET_OS_WINDOWS or TARGET_OS_LINUX or TARGET_OS_ANDROID

# make build directory
cd nolimitapp/build
mkdir nlcbuild
cd nlcbuild

=== build No Limit Connect ===

rm -rf *
# if building linux
cmake  -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_LINUX=true -DMAKE_ENV=CI
# if building android
cmake  -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true -DMAKE_ENV=CI

# compile.. expect 2 hours or more
make (Optional parameter -jXX where XX is the number of CPUs/threads to build with)

=== create installer package for linux ===
rm -rf ../../package/linux
cpack -C CPackConfig.cmake
 
# the installer .deb package noimitconnect-X.X.X-Linux.deb for this example will be written to nolimitapp/package/linux
 
 === create installer package for android ===
 # so far I have not figured out how to create a aab into nolimitapp/package/android
 # instead you can copy the apk file from
 
