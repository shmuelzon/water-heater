# Tools
ARDUINO=/Applications/Arduino.app/Contents/MacOS/Arduino
ESP_DIR=~/Library/Arduino15/packages/esp8266
ESPOTA=python $(ESP_DIR)/hardware/esp8266/2.0.0/tools/espota.py

# Targets
BUILD=.build
SRC=water-heater.ino
BIN=$(BUILD)/$(SRC).bin

# Target parameters
HOST=water-heater
PORT=8266

# Image parameters
$(BUILD):
	mkdir $@

$(BIN): $(BUILD) water-heater.ino
	rm -rf $(BUILD)/*
	$(ARDUINO) --verify --board esp8266:esp8266:generic:FlashSize=4M1M,FlashMode=dio,CpuFrequency=80 --pref build.path=$(BUILD) --verbose $(SRC)

all: $(BIN)

clean:
	rm -f $(BIN)
	rm -rf $(BUILD)

upload: $(BIN)
	(export IP=`dig +short $(HOST) | head -n1`; \
	  $(ESPOTA) -rf $< -i $$IP -p $(PORT))

.PHONY: all clean upload
.DEFAULT_GOAL=all
