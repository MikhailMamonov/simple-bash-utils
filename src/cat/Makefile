# Компилятор и флаги 
CC=gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -c
TARGET = s21_cat

# Директории
TEST_DIR = tests


# Исходные файлы 
SRCS=$(wildcard *.c)

# Объектные файлы (автоматически из SRCS)
OBJS=$(SRCS:.c=.o)

HEADERS = $(wildcard *.h)

# Цель по умолчанию
all: $(TARGET)

# Линковка: создание исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
	@echo "📁 Executable: $(TARGET)"

# Компиляция .c файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "   Compiled: $<"

test: $(TARGET)
	bash $(TEST_DIR)/test_cat.sh

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "🧹 Cleaned:  $(TARGET)"

# Полная пересборка
rebuild: clean all

# Проверка стиля (опционально)
check-style:
	clang-format -n $(SRCS) $(HEADERS)

fix-style:
	clang-format -i $(SRCS) $(HEADERS)

.PHONY: all clean rebuild test check-style fix-style