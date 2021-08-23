all:

# client-side library
	make -C libgws

# Gramado Core
# The window server and the main client
	make -C core/server    
	make -C core/client    

# clients
	make -C gwm
	make -C logon
	make -C editor
	make -C fileman
	make -C terminal  
	make -C browser


# clean the lib
	-rm -rf libgws/obj/*.o
