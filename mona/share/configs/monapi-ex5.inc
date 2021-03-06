IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .cc .o
.cc.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ $<

all: $(TARGET).EX5

$(TARGET).EX5: $(TARGET).EXE
	`which bim2bin` in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

$(TARGET).EXE:  $(ADDTARGETDEP) $(LINKDEP) $(OBJECTS) $(LINKDEP)
	#echo  "test" $(ADDTARGETDEP) $(LINKDEP) $(OBJECTS) $(LINKDEP)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi_crt.o $(OBJECTS) -L$(LIBDIR) $(LINK)
	#$(LD) --Map $(TARGET).map $(LFLAGS)  -L$(LIBDIR) $(LINK) -o $@ $(LIBDIR)/monapi_crt.o $(OBJECTS)
#	$(STRIP) $@

install: force
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)
force: $(TARGET).EX5 ;

clean:
	rm -f $(OBJECTS) $(TARGET).EXE $(TARGET).EX5 $(CLEANFILES) $(TARGET).map dependencies

depend dependencies:$(ADDTARGETDEP)
	rm -f dependencies
ifneq ($(SOURCES),)
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(CSOURCES),)
	#$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) >> dependencies
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(CSOURCES) >> dependencies
endif
ifneq ($(NASMSRCS),)
	for asm in $(NASMSRCS); do echo `echo $$asm | sed "s/\.asm/.o/": $$asm` >> dependencies; done
endif

check-syntax:
	LANG=C $(CXX) -o nul -fsyntax-only $(CXXFLAGS) $(INCLUDE) -S ${CHK_SOURCES} && python $(MONADIR)/tool/cpplint.py ${CHK_SOURCES}
	
