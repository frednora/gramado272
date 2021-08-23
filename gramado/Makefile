
# This file builds all the components in gramado/ folder.

all:

# client-side library
	make -C libgws

# Gramado Core
# The window server and the main client
	make -C core/server    
	make -C core/client    

# Gramado shell
	make -C shell/terminal  
	make -C shell/logon
	make -C shell/gwm
	make -C shell/fileman
	make -C shell/editor
	make -C shell/browser

# Clean libraries
	-rm -rf libgws/obj/*.o
