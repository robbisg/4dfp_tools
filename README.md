# 4dfp utilities from Washington University
Git repository of edited 4dfp_tools by Washington University, I edited some makefiles to be compiled with current versions of gcc on Ubuntu 16.04.

Source code for 4dfp utilities.
These utilities have been downloaded from ftp://imaging.wustl.edu/pub/raichlab/4dfp_tools/

Following instruction are copied from above website.

To build the files follow these instruction.

Make instructions follow.
The Linux OS should have installed gcc 4.1.2 or better.
Three key environmental variables must be set for correct compilation and operation:
NILSRC		directory in which the executables will be compiled
RELEASE		directory where executables will reside


It is recommended that the linux startup file be modified to set the
above three environmental variables. The login procedure should also be modifed
to include $RELEASE and . in environmental variable PATH.

1. Make sure environmental variable $NILSRC is properly set.
   Put nil-tools.tar in $NILSRC
   cd into $NILSRC and untar nil-tools.tar.

2. Make sure environmental variable $RELEASE is properly set.
3. run sudo groupadd program
4. Run tcsh make_nil-tools.csh. This will take a couple of minutes.
   You may ignore warnings about time stamps and potentially incomplete 
   makes. On successful completion make_nil-tools.csh reports
   "successful 4dfp suite make complete". All compiled executables
   then should be in $RELEASE.

3. Put 4dfp_scripts.tar into $RELEASE. cd into $RELEASE
   and untar 4dfp_scripts.tar
