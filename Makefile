NAME      := uno
CC        := gcc
CFLAGS    := -Wall -Wextra -Werror -Iincs
DEBUGFLAGS:= -g -O0
SRC_DIR   := src
INC_DIR   := incs
BUILD_DIR := build

SRCS      := $(wildcard $(SRC_DIR)/*.c)
OBJS      := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET    := $(BUILD_DIR)/$(NAME)

IP ?= 127.0.0.1

# Reglas principales
all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

unoh: $(TARGET)
	./$(TARGET) host

unoc: $(TARGET)
	./$(TARGET) client $(IP)

debug: CFLAGS += $(DEBUGFLAGS)
debug: fclean all

clean:
	rm -f $(BUILD_DIR)/*.o

fclean: clean
	rm -f $(TARGET)

re: fclean all

help:
	@echo "Opciones disponibles:"
	@echo "  make        -> compila el proyecto"
	@echo "  make run    -> compila y ejecuta"
	@echo "  make debug  -> recompila con flags de depuración"
	@echo "  make clean  -> elimina archivos objeto"
	@echo "  make fclean -> elimina objetos y ejecutable"
	@echo "  make re     -> recompila desde cero"
	@echo "  make help   -> muestra esta ayuda"

.PHONY: all run unoh unoc debug clean fclean re help