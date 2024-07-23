ARCHS = arm64 arm64e
DEBUG = 0
FINALPACKAGE = 1
FOR_RELEASE = 1
IGNORE_WARNINGS=1

MOBILE_THEOS = 1
ifeq ($(MOBILE_THEOS),1)
  # path to your sdk
  SDK_PATH = $(THEOS)/sdks/iPhoneOS11.2.sdk/
  $(info ===> Setting SYSROOT to $(SDK_PATH)...)
  SYSROOT = $(SDK_PATH)
else
  TARGET = iphone:clang:latest:10.0
endif
  
ZIP_ARCHIVE_DEFINES = -DHAVE_INTTYPES_H -DHAVE_PKCRYPT -DHAVE_STDINT_H -DHAVE_WZAES -DHAVE_ZLIB

KITTYMEMORY_PATH = ../deps/KittyMemory/KittyMemory
KITTYMEMORY_SRC = $(wildcard $(KITTYMEMORY_PATH)/*.cpp)

include $(THEOS)/makefiles/common.mk

TWEAK_NAME = DumpIL2CPP

$(TWEAK_NAME)_FILES = src/Tweak.mm src/AlertUtils.mm \
$(wildcard src/Core/*.cpp) \
$(KITTYMEMORY_SRC) \
$(wildcard includes/fmt/format.cc) \
$(wildcard includes/SCLAlertView/*.m) \
$(wildcard includes/GCDAsyncSocket/*.m) \
$(wildcard includes/SSZipArchive/SSZipArchive.m) $(wildcard includes/SSZipArchive/minizip/*.c)

$(TWEAK_NAME)_CFLAGS = -fobjc-arc $(ZIP_ARCHIVE_DEFINES) -Wno-deprecated-declarations

$(TWEAK_NAME)_CCFLAGS = -Iincludes -I$(KITTYMEMORY_PATH) -O2 -DkNO_KEYSTONE -DkNO_SUBSTRATE

$(TWEAK_NAME)_LDFLAGS = -lz -liconv -ldl

$(TWEAK_NAME)_FRAMEWORKS = UIKit Foundation Security

include $(THEOS_MAKE_PATH)/tweak.mk

clear::
	rm -rf .theos