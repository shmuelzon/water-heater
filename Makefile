# Tools
ARDUINO=/Applications/Arduino.app/Contents/MacOS/Arduino
ESP_DIR=~/Library/Arduino15/packages/esp8266
MKSPIFFS=$(ESP_DIR)/tools/mkspiffs/0.1.2/mkspiffs
ESPOTA=python $(ESP_DIR)/hardware/esp8266/2.0.0/tools/espota.py

# Targets
SPIFFS=spiffs.bin
DATA=data
CDATA=.data
# Create targets for compressed FS files
$(foreach f,$(subst data/,,$(shell find data -type f)), \
  $(eval $(CDATA)/$f.gz: $(DATA)/$f) \
  $(eval DATA_FILES+=$(CDATA)/$f.gz) \
)
CACHE_FILE=$(CDATA)/www/manifest.appcache
BUILD=.build
SRC=water-heater.ino
BIN=$(BUILD)/$(SRC).bin

# Target parameters
HOST=water-heater
PORT=8266

# Image parameters
PAGE_SIZE=256
BLOCK_SIZE=8192
IMAGE_SIZE=1028096

$(BUILD):
	mkdir $@

$(BIN): $(BUILD) water-heater.ino
	rm -rf $(BUILD)/*
	$(ARDUINO) --verify --board esp8266:esp8266:generic:FlashSize=4M1M,FlashMode=dio,CpuFrequency=80 --pref build.path=$(BUILD) --verbose $(SRC)

$(CACHE_FILE): $(DATA_FILES)
	echo "CACHE MANIFEST" > $@
	echo "" >> $@
	echo "CACHE:" >> $@
	(cd $(DATA)/www; find * -type f) >> $@
	echo "" >> $@
	echo "NETWORK:" >> $@
	echo "*" >> $@
	echo "" >> $@
	HASH=`find $(DATA)/www -type f | xargs cat | md5`; \
	  echo "# HASH: $$HASH" >> $@

$(DATA_FILES):
	@test ! -d $(dir $@) && mkdir -p $(dir $@) || true
	gzip -ck $< > $@

$(SPIFFS): $(DATA_FILES) $(CACHE_FILE)
	$(MKSPIFFS) -c $(CDATA) -p $(PAGE_SIZE) -b $(BLOCK_SIZE) -s $(IMAGE_SIZE) $@

all: $(SPIFFS) $(BIN)

clean:
	rm -f $(SPIFFS) $(BIN)
	rm -rf $(CDATA) $(BUILD)

upload: $(BIN)
	IP=`dig +short $(HOST) | head -n1`; \
	  $(ESPOTA) -rf $< -i $$IP -p $(PORT)

upload-spiffs: $(SPIFFS)
	IP=`dig +short $(HOST) | head -n1`; \
	  $(ESPOTA) -rsf $< -i $$IP -p $(PORT)

install-deps:
	$(ARDUINO) --install-boards esp8266:esp8266 || true
	$(ARDUINO) --install-library \
	  Adafruit\ NeoPixel,InputDebounce,ArduinoJson,NTPClient,Time,WiFiManager

.PHONY: all clean upload upload-spiffs install-deps
.DEFAULT_GOAL=all
