# pax

It's a c++ PortAudio wrapper. I use this in Blockhead.

In Blockhead I require:
 - A single audio stream to be open at a time with 2 output channels and any number of input channels.
 - Ability to hot-switch between audio devices
 - Ability to query if a device is a WASAPI loopback device
 - Ability to rescan available audio devices

If this matches your requirements then maybe this is useful for you.
