# Lexeme Table
# (c) David Berthelot 2007-2008, all rights reserved.
# For licence of use: contact david.berthelot@gmail.com

ARCH    = $(shell uname -m)
OBJDIR  = ../OBJECTS/$(ARCH)
LIBDIR  = ../LIB/$(ARCH)
INCLUDE = -I../INCLUDE
objects = $(addprefix $(OBJDIR)/,LexemeTable.o)

All: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libutil.a

$(OBJDIR) :
	mkdir -p $(OBJDIR)

$(LIBDIR) :
	mkdir -p $(LIBDIR)

$(LIBDIR)/libutil.a : $(objects)
	 $(AR) -cr $@ $(objects)

$(OBJDIR)/%.o : %.cxx
	$(CXX) -g -Wall -c $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $< -o $@

clean: 
	rm -rf *.exe *.yy.c *.tab.?xx *.ebnf.indexed $(OBJDIR) $(LIBDIR)

depend:
	makedepend -- $(CFLAGS) $(CPPFLAGS) -- *cxx *c
# DO NOT DELETE
