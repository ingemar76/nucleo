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

SRC_BASE = ./src

LINK_FILE := $(SRC_BASE)/linker/STM32F411RE_FLASH.ld

INCLUDE += -I$(SRC_BASE)/ -I$(SRC_BASE)//drivers -I$(SRC_BASE)/drivers/cmsis -I$(SRC_BASE)/board

VPATH = $(SRC_BASE)/ $(SRC_BASE)/drivers $(SRC_BASE)/drivers/cmsis $(SRC_BASE)/board

BUILD_DIR = build
#BUILD_DIR = .

COMMON_FLAGS += -mcpu=cortex-m4 -mthumb -DSTM32F411xE
COMMON_FLAGS += -O0 -g3
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += -fmessage-length=0 -fsigned-char

DEPFLAGS += -MMD -MP -MF"$(@:.o=.d)" -MT"$@"

CFLAGS += $(COMMON_FLAGS) $(INCLUDE) $(DEPFLAGS) -std=gnu11

all: $(BUILD_DIR) $(BUILD_DIR)/$(APP_NAME).hex

$(BUILD_DIR):
	@echo "Creating build directory"
	mkdir -p $(BUILD_DIR)

#Ordinary compile
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Building $<"
	@$(CC) -c  $(CFLAGS) $< -o $@

#Startup asm file
$(BUILD_DIR)/$(START_O): $(START)
	@echo "Building asm $<"
	@$(CC) -c $(COMMON_FLAGS) $(DEPFLAGS) -x assembler-with-cpp -o $@ $<


#Linking app
$(BUILD_DIR)/$(APP_NAME).elf: $(patsubst %, $(BUILD_DIR)/%,$(OBJS)) $(LINK_FILE)
	@echo "Linking"
	@$(CC) $(COMMON_FLAGS) -Xlinker --gc-sections -Wl,-Map,"$(BUILD_DIR)/$(APP_NAME).map" -T$(LINK_FILE) $(patsubst %, $(BUILD_DIR)/%,$(OBJS)) -o "$(BUILD_DIR)/$(APP_NAME).elf"

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
