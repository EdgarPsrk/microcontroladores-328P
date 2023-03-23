# MAKEFILE TEMPLATE FOR GCC-AVR PROJECTS
#cambiar el nombre del target
F_CPU = 16000000UL 
TARGET = invertedPendulum
MCU = atmega328p
FORMAT = ihex
#src carpeta de origen para los archivos
SRC_DIR = src
#include directrio de cabecera
INC_DIR = include/
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/objects
LST_DIR = $(BUILD_DIR)/listings
# C source files
SRC = $(wildcard $(SRC_DIR)/*.c)
# Assembler source files
ASRC = $(wildcard $(SRC_DIR)/*.S)
OPT = s

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -frv
MV = mv -f

# avrdude settings and variables
AVRDUDE_PROGRAMMER = arduino
#cambiar al puerto del arduino
AVRDUDE_PORT = /dev/ttyACM0
AVRDUDE_WRITE_FLASH = -U flash:w:$(BUILD_DIR)/$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(BUILD_DIR)/$(TARGET).eep
#AVRDUDE_ERASE_COUNTER = -y
#AVRDUDE_NO_VERIFY = -V
AVRDUDE_VERBOSE = -v
AVRDUDE_BASIC = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS = $(AVRDUDE_BASIC) $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE) $(AVRDUDE_ERASE_COUNTER)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS = -D F_CPU=${F_CPU}

# Place -I options here
CINCS = $(INC_DIR)

# C flags
CDEBUG = -g
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
CFLAGS = $(CINCS) $(CDEBUG) $(CDEFS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)

# ASM flags
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 

#Additional libraries.

# External memory options
EXTMEMOPTS =

#LDMAP = $(LDFLAGS) -Wl,-Map=$(TARGET).map,--cref
LDFLAGS = $(EXTMEMOPTS) $(LDMAP) 

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I $(CFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I -x assembler-with-cpp $(ASFLAGS)

# Define all object files.
OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o) $(ASRC:%.S=$(OBJ_DIR)/%.o)

# Define all listing files.
LST = $(ASRC:%.S=$(LST_DIR)/%.lst) $(SRC:%.c=$(LST_DIR)/%.lst)

# Default target.
all: build

build: elf hex eep

elf: $(BUILD_DIR)/$(TARGET).elf
hex: $(BUILD_DIR)/$(TARGET).hex
eep: $(BUILD_DIR)/$(TARGET).eep
lss: $(BUILD_DIR)/$(TARGET).lss 
sym: $(BUILD_DIR)/$(TARGET).sym

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 

coff: $(BUILD_DIR)/$(TARGET).elf
	$(COFFCONVERT) -O coff-avr $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).cof

extcoff: $(BUILD_DIR)/$(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).cof

.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
$(BUILD_DIR)/$(TARGET).elf: $(OBJ)
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
$(OBJ_DIR)/%.s: %.c
	@mkdir -p $(@D)
	$(CC) -S $(ALL_CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(@D)
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Program the device.  
upload: $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).eep
	@clear
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

clean:
	$(REMOVE) $(BUILD_DIR)

.PHONY: all build elf hex eep lss sym upload coff extcoff clean