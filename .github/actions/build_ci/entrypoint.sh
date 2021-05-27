#!/bin/bash

readonly TARGET="$1"

ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk
ZEPHYR_SDK_VERSION=0.12.4
ZEPHYR_SDK_DOWNLOAD_FOLDER=https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v$ZEPHYR_SDK_VERSION
ZEPHYR_SDK_SETUP_BINARY=zephyr-sdk-$ZEPHYR_SDK_VERSION-x86_64-linux-setup.run
ZEPHYR_SDK_DOWNLOAD_URL=$ZEPHYR_SDK_DOWNLOAD_FOLDER/$ZEPHYR_SDK_SETUP_BINARY

FREERTOS_ZIP_URL=https://cfhcable.dl.sourceforge.net/project/freertos/FreeRTOS/V10.0.1/FreeRTOSv10.0.1.zip

pre_build(){
	# fix issue related to tzdata install
	echo 'Etc/UTC' > /etc/timezone &&
	ln -s /usr/share/zoneinfo/Etc/UTC /etc/localtime &&
	apt update &&
   	apt-get install -y cmake make
}

build_linux(){
	echo  " Build for linux"
	apt-get install -y libsysfs-dev libhugetlbfs-dev gcc &&
	mkdir -p build-linux &&
	cd build-linux &&
	cmake .. -DWITH_TESTS_EXEC=on &&
	make VERBOSE=1 all test &&
	exit 0
}

build_generic(){
	echo  " Build for generic platform "
      	apt-get install -y gcc-arm-none-eabi &&
	mkdir -p build-generic &&
	cd build-generic &&
	cmake .. -DCMAKE_TOOLCHAIN_FILE=template-generic &&
	make VERBOSE=1 &&
	exit 0
}

build_freertos(){
	echo  " Build for freertos OS "
      	apt-get install -y gcc-arm-none-eabi unzip &&
      	wget $FREERTOS_ZIP_URL > /dev/null &&
      	unzip FreeRTOSv10.0.1.zip > /dev/null &&
	mkdir -p build-freertos &&
	cd build-freertos && export &&
      	cmake .. -DCMAKE_TOOLCHAIN_FILE=template-freertos -DCMAKE_C_FLAGS="-I$PWD/../FreeRTOSv10.0.1/FreeRTOS/Source/include/ -I$PWD/../FreeRTOSv10.0.1/FreeRTOS/Demo/CORTEX_STM32F107_GCC_Rowley -I$PWD/../FreeRTOSv10.0.1/FreeRTOS/Source/portable/GCC/ARM_CM3" &&
	make VERBOSE=1 &&
	exit 0
}

build_zephyr(){
	echo  " Build for Zephyr OS "
        sudo apt-get install libc6-dev-i386 make gperf gcc g++ python3-ply python3-yaml python3-pip device-tree-compiler ncurses-dev uglifyjs -qq &&
        sudo pip3 install pyelftools &&
	pip3 install --user -U west
	echo 'export PATH=~/.local/bin:"$PATH"' >> ~/.bashrc
	source ~/.bashrc

	wget $ZEPHYR_SDK_DOWNLOAD_URL &&
    	chmod +x $ZEPHYR_SDK_SETUP_BINARY &&
    	rm -rf $ZEPHYR_SDK_INSTALL_DIR &&
    	./$ZEPHYR_SDK_SETUP_BINARY --quiet -- -y -d $ZEPHYR_SDK_INSTALL_DIR 2> /dev/null &&

	west init ./zephyrproject
	cd ./zephyrproject
	west update
	west zephyr-export
	pip3 install --user -r ./zephyr/scripts/requirements.txt

    	cd ./zephyr &&
    	source zephyr-env.sh &&
	cd ../.. &&
	cmake . -DWITH_ZEPHYR=on -DBOARD=qemu_cortex_m3 -DWITH_TESTS=on -Bbuild-zephyr &&
	cd build-zephyr &&
	make VERBOSE=1 &&
	exit 0
}

main(){
	pre_build;

	if [[ "$TARGET" == "linux" ]]; then
   		build_linux
   	fi
	if [[ "$TARGET" == "generic" ]]; then
   		build_generic
   	fi
	if [[ "$TARGET" == "freertos" ]]; then
   		build_freertos
   	fi
	if [[ "$TARGET" == "zephyr" ]]; then
   		build_zephyr
   	fi
}

main
