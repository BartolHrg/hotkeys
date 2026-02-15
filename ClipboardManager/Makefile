CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra
LDFLAGS = -mwindows -municode -static -static-libgcc -static-libstdc++
LIBS = -luser32 -lgdi32

SRC_DIR = Source
BUILD_DIR = Build
DATA_DIR = Data

all: $(BUILD_DIR)/ClipboardManagerDual.exe $(BUILD_DIR)/ClipboardManagerRotatingSaveInitial.exe $(BUILD_DIR)/ClipboardManagerRotatingDropInitial.exe

$(BUILD_DIR)/ClipboardManagerDual.exe: $(SRC_DIR)/ClipboardManager.cpp $(SRC_DIR)/ClipboardManagerDual.cpp $(SRC_DIR)/ClipboardManager.hpp $(SRC_DIR)/ClipboardUtils.cpp $(SRC_DIR)/CppPatterns.hpp $(SRC_DIR)/Utils.hpp | $(BUILD_DIR) $(DATA_DIR)
	$(CXX) $(CXXFLAGS) -DUNITY_BUILD \
		-include $(SRC_DIR)/ClipboardManagerDual.cpp \
		$(SRC_DIR)/ClipboardManager.cpp \
		-o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/ClipboardManagerRotatingSaveInitial.exe: $(SRC_DIR)/ClipboardManager.cpp $(SRC_DIR)/ClipboardManagerRotating.cpp $(SRC_DIR)/ClipboardManager.hpp $(SRC_DIR)/ClipboardUtils.cpp $(SRC_DIR)/CppPatterns.hpp $(SRC_DIR)/Utils.hpp | $(BUILD_DIR) $(DATA_DIR)
	$(CXX) $(CXXFLAGS) -DUNITY_BUILD -DSAVE_INITIAL \
		-include $(SRC_DIR)/ClipboardManagerRotating.cpp \
		$(SRC_DIR)/ClipboardManager.cpp \
		-o $@ $(LDFLAGS) $(LIBS)
$(BUILD_DIR)/ClipboardManagerRotatingDropInitial.exe: $(SRC_DIR)/ClipboardManager.cpp $(SRC_DIR)/ClipboardManagerRotating.cpp $(SRC_DIR)/ClipboardManager.hpp $(SRC_DIR)/ClipboardUtils.cpp $(SRC_DIR)/CppPatterns.hpp $(SRC_DIR)/Utils.hpp | $(BUILD_DIR) $(DATA_DIR)
	$(CXX) $(CXXFLAGS) -DUNITY_BUILD \
		-include $(SRC_DIR)/ClipboardManagerRotating.cpp \
		$(SRC_DIR)/ClipboardManager.cpp \
		-o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
