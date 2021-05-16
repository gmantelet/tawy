TARGET   = tawy

BIN      = bin
INC      = inc
LIB      = lib
OBJ      = obj
SRC      = src

CC       = gcc
RM       = rm -rf
MKDIR    = mkdir -p

CFLAGS   = -c -Wall -Werror -O3 -I./$(INC)
LFLAGS   = -lm -lglfw -lGL -lX11 -lpthread -lXi -lXrandr -ldl -lassimp

SOURCES  = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c)
INCLUDES = $(wildcard $(INC)/*.h)
OBJECTS  = $(SOURCES:$(SRC)/%.c=$(OBJ)/%.o)

.PHONY: all
all: $(OBJECTS) $(BIN)/$(TARGET)

$(OBJECTS): $(OBJ)/%.o : $(SRC)/%.c
	@$(MKDIR) $(@D)
	@echo $(CC) $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/$(TARGET): $(OBJECTS)
	@$(MKDIR) $(@D)
	@echo $(CC) $(OBJECTS) $(LFLAGS) -o $@
	@$(CC) $(OBJECTS) $(LFLAGS) -o $@
	@echo "===>" $(BIN)/$(TARGET)
	@echo "Done."

.PHONY: clean
clean:
	@echo $(RM) $(OBJ)/
	@$(RM) $(OBJ)/
	@echo $(RM) $(BIN)/
	@$(RM) $(BIN)/