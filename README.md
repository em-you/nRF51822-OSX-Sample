nRF51822-OSX-Sample
===================

You need to install a GCC toolchain (I use CodeSourcery).  I have it in my home directory and you can see I point to that in the Makefile.  A better way would be to put all the pre-compiled toolchains for each architecture (e.g., OSX, Windows) in the project so that everyone is using same toolchain for their architecture.

This project is just from Nordic's sample ble hrs.  There are some warnings that can be fixed by proper casting.

Directory Structure
-------------------

bin/ - map files
build/ - hex/bin/log files, this is where your flash file will end up
config/ - linker scripts
lib/ - any 3rd party SDK (currently only Nordic)
obj/ - .o files
projects/ - any IDE configuration files (Xcode project sample in there)
src/ - source files
test/ - unit tests
tools/ - platform specific toolchains and jtag emulators

Makefile targets
----------------

make
make debug
make flash-softdevice (flashes Softdevice via JLink)
make flash (flashes output hex file via JLink)
make erase-all (erases flash)
make startdebug - (starts GDBServer & GDB via JLink)
make stopdebug - (stops GDBServer)

XCode Info
----------

The main target is an External Build System project.  There is also a target called XCodeIndex.  XCodeIndex is a command line tool project that you should add all your source files to, in addition to your main target.  By adding your source files, this allows Xcode to use Clang to enable code completion and symbol drill-down.  Xcode will not index any source files if not added to it.  You can build and clean from inside Xcode.

JLink
-----

You will need to configure the Makefile to match your JLink setup.  I use the JLink OSX libraries, so that make file is pointing to that.  You can setup the makefile to support OSX and Windows fairly easily.  On occasion, you might need to disconnect the JLink from your computer if it seems like flashing is not working.  Also, JLink drivers are pretty buggy on OSX and Linux, so you might want to be careful when upgrading to latest driver versions as they don't seem to test their driver updates very well.  I am considering getting another JTag board and trying OpenOCD.

Hopefully, this is helpful.  Please let me know if you come up with any better tweaks or insights.

NOTE: add the following to your .bashrc file:
export DYLD_LIBRARY_PATH=/Users/<your home directory>/jlink:/opt/local/lib:$DYLD_LIBRARY_PATH

