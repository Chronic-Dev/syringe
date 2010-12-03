UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	OS_SHARED_EXT = .dylib
	OS_STATIC_EXT = .a
	OS_EXEC_EXT = 
else
	ifeq ($(UNAME),MINGW32_NT-5.1)
		OS_SHARED_EXT = .dll
		OS_STATIC_EXT = .a
		OS_EXEC_EXT = .exe
	else
		OS_SHARED = .so
		OS_STATIC_EXT = .a
		OS_EXEC_EXT = 
	endif
endif