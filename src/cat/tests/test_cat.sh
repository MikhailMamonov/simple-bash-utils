#!/bin/sh

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' 

CAT_REAL="/usr/bin/cat"
CAT_TEST="./../s21_cat"
TEST_DIR="input"

SUCCESS=0
FAIL=0

assert(){
    local description="$1"
    local expected_result=$2
    shift 2

      # Читаем stdin если он есть (включая here-string)
    local input_data=""
    if [ ! -t 0 ]; then
        # Сохраняем весь stdin
        input_data=$(cat)
    fi

    local real_output test_output real_exit test_exit

    if [ -n "$input_data" ]; then
        real_output=$(echo "$input_data" | "$CAT_REAL" "$@" 2>&1)
        real_exit=$?
        test_output=$(echo "$input_data" | "$CAT_TEST" "$@" 2>&1)
        test_exit=$?
    else
        real_output=$( "$CAT_REAL" "$@" 2>&1 )
        real_exit=$?
        test_output=$( "$CAT_TEST" "$@" 2>&1 )
        test_exit=$?
    fi

    # Заменяем все последовательности пробелов на один пробел
    real_output_normalized=$(echo "$real_output" | tr -s '[:space:]' ' ')
    test_output_normalized=$(echo "$test_output" | tr -s '[:space:]' ' ')

    if [ "$real_output_normalized" = "$test_output_normalized" ] && [ "$real_exit" = "$test_exit" ]; then
        echo -e  "${GREEN}✓${NC} $description"
        SUCCESS=$((SUCCESS + 1))
    else
        echo -e  "${RED}✗${NC} $description"
        echo "   Args: $*"
        echo "   Expected output: $real_output, Got: $test_output"
        echo "   Expected exit: $real_exit, Got: $test_exit"
        FAIL=$((FAIL + 1))
    fi
}

if [ ! -f "$CAT_TEST" ]; then
    echo "Error: $CAT_TEST not found. Run 'make' first."
    exit 1
fi

echo "=== Testing s21_cat vs System Cat ==="

# ---------------------- 1. Без флагов ----------------------
assert "No flags, one file" 0 "$TEST_DIR/1.txt"
assert "No flags, two files" 0 "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "No flags, empty file" 0 "$TEST_DIR/empty.txt"

# ---------------------- 2. Флаг -n (нумерация всех строк) ----------------------
assert "flag -n" 0 -n "$TEST_DIR/1.txt"
assert "flag -n, two files" 0 -n "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "GNU long option --number" 0 --number  "$TEST_DIR/1.txt"

# ---------------------- 2. Флаг -b (нумерация непустых строк) ----------------------
assert "flag -b" 0 -b "$TEST_DIR/1.txt"
assert "Combination -bn (b overrides n)" 0 -bn "$TEST_DIR/1.txt"
assert "GNU long option --number-nonblank" 0 --number-nonblank "$TEST_DIR/1.txt"

# ---------------------- 4. Флаг -s (сжатие пустых строк) ----------------------
assert "Flag -s" 0 -s "$TEST_DIR/1.txt"
assert "GNU long option --squeeze-blank" 0 --squeeze-blank "$TEST_DIR/1.txt"

# ---------------------- 5. Флаги -e и -E (отображение концов строк $) ----------------------
assert "Flag -e (implies -v)" 0 -e "$TEST_DIR/1.txt"
# В GNU: -E делает тоже самое, но без -v (обычно просто $ в конце)
assert "Flag -E (show ends only)" 0 -E "$TEST_DIR/1.txt"

# ---------------------- 6. Флаги -t и -T (отображение табов ^I) ----------------------
assert "Flag -t (implies -v)" 0 -t "$TEST_DIR/1.txt"
assert "Flag -T (show tabs only)" 0 -T "$TEST_DIR/1.txt"

# 7. Флаг -v (отображение непечатаемых символов)
assert "Flag -v" 0 -v "$TEST_DIR/1.txt"
#assert "Flag -v with binary file" 0 -v "$TEST_DIR/binary.bin"

# 8. Флаг --show-nonprinting (GNU only)
assert "GNU long option --show-nonprinting" 0 --show-nonprinting "$TEST_DIR/1.txt"

# ---------------------- 9. Комбинации флагов ----------------------
assert "Flag -v with -n" 0 -v -n "$TEST_DIR/1.txt"
assert "Flag -v with -b" 0 -v -b "$TEST_DIR/1.txt"
assert "Flag -v with -s" 0 -v -s "$TEST_DIR/1.txt"
assert "Flag -v with -E" 0 -v -E "$TEST_DIR/1.txt"
assert "Flag -v with -T" 0 -v -T "$TEST_DIR/1.txt"

assert "Flag -ne" 0 -ne "$TEST_DIR/1.txt"
assert "Flag -nE" 0 -nE "$TEST_DIR/1.txt"
assert "Flag -nb" 0 -nb "$TEST_DIR/1.txt"
assert "Flag -nT" 0 -nT "$TEST_DIR/1.txt"
assert "Flag -bE" 0 -bE "$TEST_DIR/1.txt"
assert "Flag -bT" 0 -bT "$TEST_DIR/1.txt"
assert "Flag -sE" 0 -sE "$TEST_DIR/1.txt"
assert "Flag -sT" 0 -sT "$TEST_DIR/1.txt"

# ---------------------- 10. Флаг -e и -E с другими флагами ----------------------
assert "Flag -e with -n" 0 -e -n "$TEST_DIR/1.txt"
assert "Flag -e with -b" 0 -e -b "$TEST_DIR/1.txt"
assert "Flag -e with -s" 0 -e -s "$TEST_DIR/1.txt"
assert "Flag -E with -n" 0 -E -n "$TEST_DIR/1.txt"
assert "Flag -E with -b" 0 -E -b "$TEST_DIR/1.txt"
assert "Flag -E with -s" 0 -E -s "$TEST_DIR/1.txt"

# ---------------------- 11. Флаг -t и -T с другими флагами ----------------------
assert "Flag -t with -n" 0 -t -n "$TEST_DIR/1.txt"
assert "Flag -t with -b" 0 -t -b "$TEST_DIR/1.txt"
assert "Flag -t with -s" 0 -t -s "$TEST_DIR/1.txt"
assert "Flag -T with -n" 0 -T -n "$TEST_DIR/1.txt"
assert "Flag -T with -b" 0 -T -b "$TEST_DIR/1.txt"
assert "Flag -T with -s" 0 -T -s "$TEST_DIR/1.txt"

# ---------------------- 12. Чтение из stdin ----------------------
cat "$TEST_DIR/1.txt" | assert "Read from stdin" 0 < "$TEST_DIR/1.txt"
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -n" 0 -n
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -b" 0 -b
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -s" 0 -s
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -E" 0 -E
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -T" 0 -T
# ---------------------- 13. Флаг -b с пустыми строками ----------------------
assert "Flag -b with empty lines" 0 -b "$TEST_DIR/empty.txt"
assert "Flag -b with multiple empty lines" 0 -b "$TEST_DIR/multiple_empty.txt"

# ---------------------- 14. Флаг -s с разными файлами ----------------------
assert "Flag -s with single empty line" 0 -s "$TEST_DIR/one_empty.txt"
assert "Flag -s with no empty lines" 0 -s "$TEST_DIR/no_empty.txt"
assert "Flag -s with -n" 0 -s -n "$TEST_DIR/1.txt"
assert "Flag -s with -b" 0 -s -b "$TEST_DIR/1.txt"

# ---------------------- 15. Комбинация всех флагов ----------------------
assert "All flags combo: -vETns" 0 -vETns "$TEST_DIR/1.txt"
assert "All flags combo: -vETnb" 0 -vETnb "$TEST_DIR/1.txt"

# ---------------------- 16. Порядок флагов ----------------------
assert "Flags order: -n -b (b overrides n)" 0 -n -b "$TEST_DIR/1.txt"
assert "Flags order: -b -n (b overrides n)" 0 -b -n "$TEST_DIR/1.txt"
assert "Flags order: -s -n" 0 -s -n "$TEST_DIR/1.txt"
assert "Flags order: -n -s" 0 -n -s "$TEST_DIR/1.txt"

# ---------------------- 17. Специальные символы в начале/конце строки ----------------------
printf "\tLeading tab\n" > "$TEST_DIR/leading_tab.txt"
printf "Trailing tab\t\n" > "$TEST_DIR/trailing_tab.txt"
printf " \n" > "$TEST_DIR/space_only.txt"

assert "Leading tab -T" 0 -T "$TEST_DIR/leading_tab.txt"
assert "Trailing tab -T" 0 -T "$TEST_DIR/trailing_tab.txt"
assert "Space only line" 0 "$TEST_DIR/space_only.txt"
assert "Space only line -b" 0 -b "$TEST_DIR/space_only.txt"

# ---------------------- 18. Массовый тест (All flags) ----------------------
# Нужно проверить как можно больше комбинаций
for combo in "-b" "-n" "-s" "-e" "-t" "-v" "-bn" "-sb" "-ne" "-nv" "-bnst"; do
    assert "Flags combo: $combo" 0 $combo "$TEST_DIR/1.txt"
done

# GNU long options
for combo in "--number" "--number-nonblank" "--squeeze-blank"; do
    assert "GNU combo: $combo" 0 $combo "$TEST_DIR/1.txt"
done

echo "=== RESULTS ==="
echo -e "Passed: ${GREEN}$SUCCESS${NC}"
echo -e "Failed: ${RED}$FAIL${NC}"

if [ $FAIL -eq 0 ]; then
 exit 0
else
 exit 1
fi
