ARCH    = $(shell uname -m)
OBJDIR  = OBJECTS/$(ARCH)
LIBS    = -L../LIBERTAD/LIB/$(ARCH) -L../MINILOG/LIB/$(ARCH) -llibertad -lminilog
INCLUDE = -I../LIBERTAD/INCLUDE -I../MINILOG/INCLUDE
objects = $(addprefix $(OBJDIR)/,netlib.o)

All: $(OBJDIR) netlib.exe

$(OBJDIR) :
	mkdir -p $(OBJDIR)

netlib.exe : $(objects) ../LIBERTAD/LIB/$(ARCH)/liblibertad.a ../MINILOG/LIB/$(ARCH)/libminilog.a
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
