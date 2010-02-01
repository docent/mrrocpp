#///////////////////////////////////////////////////////////////////////////////
# /** @file     Makefile
#  *
#  *  Main makefile of the MRROC++ frame system.
#  *
#  *  @author   tkornuta
#  *  @date     2007-01-12
#  *  $URL$
#  *  $LastChangedRevision$
#  *  $LastChangedDate$  
#  *  $LastChangedBy$
#  *  
#  */
#///////////////////////////////////////////////////////////////////////////////
      

#------------------------------------------------------------------------------#
# Should be modified only by developers not by users
HOMEDIR = .
CLEANDIRS = src lib lib.linux
CLEANEDFILES = ./bin/* ./bin.linux/* ./msr/* ./logs/* ./html/* ./latex/*
INSTALLDIRS = src
#======================================================#	
.PHONY: all distclean clean install dist


#------------------------------- Build rules ----------------------------------#
all:
	$(MAKE) -C src
#------------------------------------------------------------------------------#
#probably not nessery, should be removed in the future
#app:
#	for i in ${DIRS} ; do (cd $$i && $(MAKE) app) ; done
#
#proto:
#	for i in ${DIRS} ; do (cd $$i && $(MAKE) proto) ; done
#======================================================#	


#------------------------------ Maintain rules --------------------------------#
clean:
	-@rm -f $(CLEANEDFILES);
	@for i in ${CLEANDIRS} ; do ($(MAKE) -C $$i clean) ;done

distclean: clean
	@rm -f `find . -type d -name "\.deps"`
	@rm -f cscope.out tags
#------------------------------------------------------------------------------#
install:
	@for i in ${INSTALLDIRS} ; do ($(MAKE) -C $$i install) ; done
#------------------------------------------------------------------------------#
dist: clean
	export VERSION && cd .. &&   tar czvf mrrocpp.tgz mrrocpp   && mv mrrocpp.tgz "mrrocpp${VERSION}.tgz"

#==============================================================================#	
