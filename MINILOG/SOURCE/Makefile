# Verilog netlist reader
# (c) David Berthelot 2007-2008, all rights reserved.
# For licence of use: contact david.berthelot@gmail.com

ARCH    = $(shell uname -m)
OBJDIR  = ../OBJECTS/$(ARCH)
LIBDIR  = ../LIB/$(ARCH)
LIBS    = -L../../UTILS/LIB/$(ARCH) -lutil
INCLUDE = -I../INCLUDE -I../../UTILS/INCLUDE
objects = $(addprefix $(OBJDIR)/,vlogobjects.o vlognetlist.tab.o vlognetlist.yy.o)

All: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libminilog.a

Doc: ../INCLUDE/vlogobjects.hxx
	doxygen Doxyfile

$(OBJDIR) :
	mkdir -p $(OBJDIR)

$(LIBDIR) :
	mkdir -p $(LIBDIR)

$(LIBDIR)/libminilog.a : $(objects) ../../UTILS/OBJECTS/$(ARCH)/LexemeTable.o
	 $(AR) -cr $@ $(objects) ../../UTILS/OBJECTS/$(ARCH)/LexemeTable.o

%.tab.cxx %.tab.hxx: %.yxx
	bison -p $(*F) -d $<

%.yy.c : %.l %.tab.hxx
	flex -P$(*F) -o$@ $<

$(OBJDIR)/%.o : %.c
	$(CXX) -g -Wall -c $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $< -o $@

$(OBJDIR)/%.o : %.cxx vlognetlist.tab.hxx
	$(CXX) -g -Wall -c $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $< -o $@

clean: 
	rm -rf *.exe *.yy.c *.tab.?xx $(OBJDIR) $(LIBDIR)

depend:
	makedepend -- $(CFLAGS) $(CPPFLAGS) -- *cxx *c
# DO NOT DELETE
