CXX		=	g++
HLLIB_VERS	=	2.3.0
LDFLAGS		=	-shared -Wl,-soname,libhl.so.2
CXXFLAGS	=	-O2 -g -fpic -funroll-loops -fvisibility=hidden
PREFIX		=	/usr/local
sources		=	BSPFile.cpp Checksum.cpp DebugMemory.cpp DirectoryFile.cpp \
			DirectoryFolder.cpp DirectoryItem.cpp Error.cpp FileMapping.cpp \
			FileStream.cpp GCFFile.cpp GCFStream.cpp HLLib.cpp \
			Mapping.cpp MappingStream.cpp MemoryMapping.cpp MemoryStream.cpp \
			NCFFile.cpp NullStream.cpp PAKFile.cpp Package.cpp ProcStream.cpp \
			SGAFile.cpp Stream.cpp StreamMapping.cpp Utility.cpp VBSPFile.cpp \
			VPKFile.cpp WADFile.cpp Wrapper.cpp XZPFile.cpp ZIPFile.cpp
objs		=	$(sources:.cpp=.o)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $<

all: libhl.so.$(HLLIB_VERS)

clean:
	rm -f \#* *~ *.o *.so.*

install: libhl.so.$(HLLIB_VERS)
	install -g root -m 0755 -o root -d $(PREFIX)/lib $(PREFIX)/include
	install -g root -m 0644 -o root libhl.so.$(HLLIB_VERS) $(PREFIX)/lib
	install -g root -m 0644 -o root ../lib/HLLib.h $(PREFIX)/include/hl.h
	ln -fs $(PREFIX)/lib/libhl.so.$(HLLIB_VERS) $(PREFIX)/lib/libhl.so.2
	ln -fs $(PREFIX)/lib/libhl.so.$(HLLIB_VERS) $(PREFIX)/lib/libhl.so

libhl.so.$(HLLIB_VERS): $(objs)
	$(CXX) $(LDFLAGS) -o $@ $(objs)
