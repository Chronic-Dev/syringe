BASE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
	
include $(BASE_DIR)/platform.mk
include $(BASE_DIR)/edam.mk

