# Verilog netlist reader
# (c) David Berthelot 2007-2008, all rights reserved.
# For licence of use: contact david.berthelot@gmail.com

ARCH    = $(shell uname -m)
OBJDIR  = OBJECTS/$(ARCH)
LIBS    = -L..//LIB/$(ARCH) -lminilog
INCLUDE = -I../INCLUDE
objects = $(addprefix $(OBJDIR)/,vlogreader.o)

All: $(OBJDIR) vlogreader.exe

$(OBJDIR) :
	mkdir -p $(OBJDIR)

vlogreader.exe : $(objects) ../LIB/$(ARCH)/libminilog.a
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
