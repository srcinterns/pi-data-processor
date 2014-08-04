
Radar Data Processor
===================

Records Radar data from a stereo microphone port, and outputs via UDP display data to a target IP address.


Dependencies
------------

 * alsa library (libasound2-dev)
 * fftw library (libfftw-dev)

The above dependencies can be installed on a raspberry pi running Raspbian, or any Debian-based Linux machine, by running the command `sudo apt-get install libfftw-dev libasound2-dev`


Install fftw
------------

 * Get the tar file: http://www.fftw.org/fftw-3.3.4.tar.gz
 * (go to directory for downloads)
 * tar -xvf fftw-3.3.4.tar.gz
 * cd fftw-3.3.4
 * ./configure
 * make
 * make install

Usage
-----
Run `make` in the root directory. Run with `./processor <alsa device> <destination ip> <destination port>`.
