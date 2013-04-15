# .LIB reader
# (c) David Berthelot 2008, all rights reserved.
# For licence of use: contact david.berthelot@gmail.com

ARCH    = $(shell uname -m)
OBJDIR  = OBJECTS/$(ARCH)
LIBS    = -L..//LIB/$(ARCH) -llibertad
INCLUDE = -I../INCLUDE
objects = $(addprefix $(OBJDIR)/,libreader.o)

All: $(OBJDIR) libreader.exe

$(OBJDIR) :
	mkdir -p $(OBJDIR)

libreader.exe : $(objects) ../LIB/$(ARCH)/liblibertad.a
	 $(CXX) -o $@ $(objects) $(LIBS)

$(OBJDIR)/%.o : %.c
	$(CXX) -g -Wall -c $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $< -o $@

$(OBJDIR)/%.o : %.cxx
	$(CXX) -g -Wall -c $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $< -o $@

clean: 
	rm -rf *.exe *.yy.c *.tab.?xx $(OBJDIR)

depend:
	makedepend -- $(CFLAGS) $(CPPFLAGS) -- *cxx *c
# DO NOT DELETE
