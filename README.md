
Radar Data Processor
===================

Records Radar data from a stereo microphone port, and outputs via UDP display data to a target IP address.


Dependencies
------------

 * alsa library
 * fftw library

Usage
-----
Run `make` in the root directory. Run with `./processor <alsa device> <destination ip> <destination port>`.
