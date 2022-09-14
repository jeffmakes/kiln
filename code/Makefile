ARCH=msp430f2274
CFLAGS := -mmcu=$(ARCH) -g -Wall 
# CFLAGS := -mmcu=$(ARCH) -g -Wall -O
# CFLAGS += -mendup-at=main
LDFLAGS := -I/opt/ti/msp430/gcc/include/

CC := msp430-elf-gcc

C_FILES := main.c lcd.c encoder.c adc10.c thermocouple.c triac.c scheduler.c
H_FILES := lcd.h encoder.h adc10.h thermocouple.h triac.h watchdog.h control.h scheduler.h

main: $(C_FILES) $(H_FILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

clean: 
	-rm -f main 

