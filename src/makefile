CC = arm-none-eabi-gcc

APP_NAME := lab

OBJS += main.o

OBJS += stm32f4xx_hal.o stm32f4xx_hal_uart.o stm32f4xx_hal_tim.o stm32f4xx_hal_gpio.o
OBJS += stm32f4xx_hal_rcc.o stm32f4xx_hal_cortex.o stm32f4xx_hal_dma.o stm32f4xx_hal_tim_ex.o
OBJS += system_stm32f4xx.o stm32f4xx_it.o stm32f4xx_hal_usart.o stm32f4xx_hal_msp.o
OBJS += syscalls.o

#Assembly start up file
START := startup_stm32f411xe.S
START_O := $(START:.S=.o)
OBJS += $(START_O)

LINK_FILE := linker/STM32F411RE_FLASH.ld

INCLUDE += -I. -Idrivers -Idrivers/cmsis -Iboard

VPATH = . drivers drivers/cmsis board

BUILD_DIR = ../build
#BUILD_DIR = .

COMMON_FLAGS += -mcpu=cortex-m4 -mthumb -DSTM32F411xE
COMMON_FLAGS += -O0 -g3
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += -fmessage-length=0 -fsigned-char

DEPFLAGS += -MMD -MP -MF"$(BUILD_DIR)/$*.d" -MT"$(BUILD_DIR)/$*.o"

CFLAGS += $(COMMON_FLAGS) $(INCLUDE) $(DEPFLAGS) -std=gnu11

all: $(BUILD_DIR) $(BUILD_DIR)/$(APP_NAME).hex

$(BUILD_DIR):
	@echo "Creating build directory"
	mkdir -p $(BUILD_DIR)

#Ordinary compile
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Building $<"
	@$(CC) -c  $(CFLAGS) $< -o $(BUILD_DIR)/$@

#Start up asm file
$(BUILD_DIR)/$(START_O): $(START)
	@echo "Building asm $<"
#	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -DSTM32F411xE -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"startup_stm32f411xe.d" -MT"startup_stm32f411xe.o" -c -o "../build/startup_stm32f411xe.o" "startup_stm32f411xe.S"
	@$(CC) -c $(COMMON_FLAGS) $(DEPFLAGS) -x assembler-with-cpp -o "$(BUILD_DIR)/$(START_O)" $<


#Linking app
$(BUILD_DIR)/$(APP_NAME).elf: $(patsubst %, $(BUILD_DIR)/%,$(OBJS)) $(LINK_FILE)
	@echo "Linking"
	@$(CC) $(COMMON_FLAGS) -Xlinker --gc-sections -Wl,-Map,"$(BUILD_DIR)/$(APP_NAME).map" -T$(LINK_FILE) $(patsubst %, $(BUILD_DIR)/%,$(OBJS)) -o "$(BUILD_DIR)/$(APP_NAME).elf"
#	$(CC) -mcpu=cortex-m4 -mthumb -DSTM32F411xE -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Xlinker --gc-sections -Wl,-Map,"BB1.map" -Tlinker/STM32F411RE_FLASH.ld $(patsubst %, $(BUILD_DIR)/%,$(OBJS)) -o "main.elf"

$(BUILD_DIR)/$(APP_NAME).hex: $(BUILD_DIR)/$(APP_NAME).elf
	@echo "Generating hex file"
	@arm-none-eabi-objcopy -O ihex "$(BUILD_DIR)/$(APP_NAME).elf"  "$(BUILD_DIR)/$(APP_NAME).hex"
	@echo "Binary sizes:"
	@arm-none-eabi-size --format=berkeley "$(BUILD_DIR)/$(APP_NAME).elf"

-include $(patsubst %, $(BUILD_DIR)/%,$(OBJS:.o=.d))

.PHONY: clean sizes

sizes:
	@arm-none-eabi-size --format=berkeley "$(BUILD_DIR)/$(APP_NAME).elf"

clean:
	@echo "Cleaning up ..."
	@rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(BUILD_DIR)/$(APP_NAME) $(BUILD_DIR)/$(APP_NAME).*
