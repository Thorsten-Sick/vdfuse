vdfuse
======

Virtualbox VDI files fuse mount tool. From https://sourceforge.net/projects/xspect3d/files/gordstuff/

My goal is to get it compatible to the newest virtualbox version and add some features. Maybe it can be push to virtualbox repository sooner or later.

Building
========

You will need the virtualbox sourcecode. Please load the package from their homepage

bash vdbuild_new /path/to/virtualbox/source/include/ vdfuse-v82a.c

FUSE setup
==========

If you want to mount VMs as a default user, you will have to edit /etc/fuse.conf and add "user_allow_other" to the config file

Using it
========

./vdfuse -h
DESCRIPTION: This Fuse module uses the VirtualBox access library to open a 
VirtualBox supported VD image file and mount it as a Fuse file system.  The
mount point contains a flat directory containing the files EntireDisk,
Partition1 .. PartitionN.  These can then be loop mounted to access the
underlying file systems
Version: 0.83

USAGE: ./vdfuse [options] -f image-file mountpoint
	-h	help
	-r	readonly
	-t	specify type (VDI, VMDK, VHD, or raw; default: auto)
	-f	VDimage file
	-s	Snapshot file(s) to load on top of the image file
	-a	allow all users to read disk
	-w	allow all users to read and write to disk
	-g	run in foreground
	-v	verbose
	-d	debug

NOTE: you must add the line "user_allow_other" (without quotes)
to /etc/fuse.confand set proper permissions on /etc/fuse.conf
for this to work.

Mounting a VDF r/w:

./vdfuse -w -f box-disk1.vmdk /mnt/vdf_image
mount /mnt/vdf_image/Partition2 /mnt/part

If you also want to mount snapshots add them with -s to the command line

Known issues
============

There have been issues mounting VDFs writeable. Mounting them for writing seems not to be reliable
