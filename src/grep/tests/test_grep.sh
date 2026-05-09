#!/bin/sh

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' 

GREP_REAL="/usr/bin/grep"
GREP_TEST="./../s21_grep"
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

    # Запускаем реальный grep с перенаправлением, если оно есть
    if [ -n "$input_data" ]; then
        # Передаём данные через pipe
        real_output=$(echo "$input_data" | "$GREP_REAL" "$@" 2>&1)
        real_exit=$?
        test_output=$(echo "$input_data" | "$GREP_TEST" "$@" 2>&1)
        test_exit=$?
    else
        real_output=$( "$GREP_REAL" "$@" 2>&1 )
        real_exit=$?
        test_output=$( "$GREP_TEST" "$@" 2>&1 )
        test_exit=$?
    fi

    # Сравнение
     if [ "$real_output" = "$test_output" ] && [ "$real_exit" = "$test_exit" ]; then
        echo -e  "${GREEN}✓${NC} $description"
        SUCCESS=$((SUCCESS + 1))
    else
        echo -e  "${RED}✗${NC} $description"
        echo "   Args: $*"
        echo "   Expected output :"$real_output" Got :"$test_output""
        echo "   Expected exit :\n $real_exit, Got:\n $test_exit"
        FAIL=$((FAIL + 1))
    fi
}

if [ ! -f "$GREP_TEST" ]; then
    echo "Error: $GREP_TEST not found. Run 'make' first."
    exit 1
fi

echo "=== Testing s21_grep vs System grep ==="

# ---------------------- 1. Без флагов ----------------------
assert "No flags, one file" 0 "." "$TEST_DIR/1.txt"
assert "No flags, two files" 0 "." "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "No flags, empty file" 1 "."  "$TEST_DIR/empty.txt"
# ---------------------- 2. Флаг -i (Игнорировать регистр) ----------------------
assert "flag -i" 0 -i "." "$TEST_DIR/1.txt"
assert "flag -i, two files" 0 -i "." "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "GNU long option --ignore-case"  0 --ignore-case "."  "$TEST_DIR/1.txt"

# ---------------------- 2. Флаг -v (показать все строки НЕ содержащие шаблон) ----------------------
assert "flag -v" 0 -v  "." "$TEST_DIR/1.txt"
assert "Combination -vi " 0 -vi "."  "$TEST_DIR/1.txt"
assert "GNU long option --invert-match" 0 --invert-match "." "$TEST_DIR/1.txt"

# ---------------------- 4. Флаг -n (показать номера строк) ----------------------
assert "Flag -n" 0 -n "."  "$TEST_DIR/1.txt"
assert "GNU long option --line-number" 0 --line-number "." "$TEST_DIR/1.txt"

# ---------------------- 5. Флаги -l (показать только имена файлов (без содержимого)) ----------------------
assert "Flag -l" 0 -l "." "$TEST_DIR/1.txt"
assert "GNU long option --files-with-matches (show ends only)" 0 --files-with-matches "."  "$TEST_DIR/1.txt"

# ---------------------- 6. Флаги -o (показать только совпавшую часть) ----------------------
assert "Flag -o" 0 -o "."  "$TEST_DIR/1.txt"
assert "GNU long option  --only-matching" 0 --only-matching "." "$TEST_DIR/1.txt"

# ---------------------- 7. Флаги -h (Выводит совпадающие строки, не предваряя их именами файлов.) ----------------------
assert "Flag -h" 0 -h "."  "$TEST_DIR/1.txt"
assert "GNU long option --no-filename (show ends only)" 0 --no-filename "."  "$TEST_DIR/1.txt"

# ---------------------- 8. Флаги -s (Подавляет сообщения об ошибках о несуществующих или нечитаемых файлах.) ----------------------
assert "Flag -s" 0 -s "."  "$TEST_DIR/1.txt"
assert "GNU long option  --no-messages" 0 --no-messages "." "$TEST_DIR/1.txt"

# ----------------------9. Флаг -c (подсчет строк) ----------------------
assert "Flag -c" 0 -c "." "$TEST_DIR/1.txt"
assert "Flag -c with no matching" 1 -c "xyz123" "$TEST_DIR/1.txt"
assert "Flag -c with multiple files" 0 -c "." "$TEST_DIR/1.txt"  "$TEST_DIR/2.txt"
assert "GNU long options --count" 0 --count "." "$TEST_DIR/1.txt"

# ----------------------10. Флаг -e (Несколько паттернов) ----------------------
assert "Flag -e single pattern" 0 -e "Hello" "$TEST_DIR/1.txt"
assert "Flag -e multiple pattern" 0 -e "Hello" -e "World" "$TEST_DIR/1.txt"
assert "Flag -e with -i " 0 -e "hello" "$TEST_DIR/1.txt"  "$TEST_DIR/2.txt"
assert "GNU long options --count" 0 --count "." "$TEST_DIR/1.txt"

# ----------------------11. Флаг -f (файл с паттернами)  ----------------------
assert "Flag -f single pattern file" 0 -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Flag -f with no matching patterns" 1 -f "$TEST_DIR/patterns2.txt" "$TEST_DIR/1.txt"
assert "Flag -f empty pattern file" 1 -f "$TEST_DIR/patterns_empty.txt" "$TEST_DIR/1.txt"
assert "Flag -f file with only empty lines" 0 -f "$TEST_DIR/patterns_only_empty.txt" "$TEST_DIR/1.txt"

# ---------------------- 12. Комбинации с -o и другими флагами ----------------------
assert "Flag -o with -i" 0 -o -i "hello" "$TEST_DIR/1.txt"
assert "Flag -o with -n" 0 -o -n "Line" "$TEST_DIR/1.txt"
assert "Flag -o with multiple files" 0 -o "line" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "Flag -o with no matches" 1 -o "xyz123" "$TEST_DIR/1.txt"

# ---------------------- 13. Флаг -c с другими флагами ----------------------
assert "Flag -c with -i" 0 -c -i "hello" "$TEST_DIR/1.txt"
assert "Flag -c with -v" 0 -c -v "Hello" "$TEST_DIR/1.txt"
assert "Flag -c with -n (n should be ignored)" 0 -c -n "Hello" "$TEST_DIR/1.txt"
assert "Flag -c with -l (l should override c)" 0 -c -l "Hello" "$TEST_DIR/1.txt"
assert "Flag -c with -o (o should be ignored)" 0 -c -o "Hello" "$TEST_DIR/1.txt"

# ---------------------- 14. Флаг -l с разными комбинациями ----------------------
assert "Flag -l with -i" 0 -l -i "hello" "$TEST_DIR/1.txt"
assert "Flag -l with -v (no match - no output)" 1 -l -v "Hello" "$TEST_DIR/1.txt"
assert "Flag -l with multiple files (one matches)" 0 -l "Hello" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "Flag -l with multiple files (none match)" 1 -l "xyz123" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"

# ---------------------- 15. Флаг -h с разными комбинациями ----------------------
assert "Flag -h with multiple files" 0 -h "Hello" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "Flag -h with -n" 0 -h -n "Line" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"
assert "Flag -h with -c" 0 -h -c "Hello" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"

# ---------------------- 16. Флаг -s (подавление ошибок) ----------------------
# Создаем временный файл без прав доступа
touch "$TEST_DIR/no_permission.txt"
chmod 000 "$TEST_DIR/no_permission.txt"

assert "Flag -s with non-existent file" 2 -s "pattern" "nonexistent.txt"
assert "Flag -s with no permission file" 2 -s "pattern" "$TEST_DIR/no_permission.txt"
assert "Flag -s with both valid and invalid files" 0 -s "Hello" "$TEST_DIR/1.txt" "nonexistent.txt"

# Восстанавливаем права
chmod 644 "$TEST_DIR/no_permission.txt" 2>/dev/null
rm -f "$TEST_DIR/no_permission.txt" 2>/dev/null

# ---------------------- 17. Флаг -f с разными комбинациями ----------------------
assert "Flag -f with -i" 0 -i -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Flag -f with -v" 0 -v -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Flag -f with -c" 0 -c -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Flag -f with -n" 0 -n -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Flag -f with -o" 0 -o -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Flag -f with regex special chars" 0 -f "$TEST_DIR/patterns_regex.txt" "$TEST_DIR/1.txt"

# ---------------------- 18. Флаг -e с разными комбинациями ----------------------
assert "Flag -e with -i" 0 -i -e "Hello" -e "WORLD" "$TEST_DIR/1.txt"
assert "Flag -e with -v" 0 -v -e "Hello" -e "Line" "$TEST_DIR/1.txt"
assert "Flag -e with -c" 0 -c -e "Hello" -e "Line" "$TEST_DIR/1.txt"
assert "Flag -e with -n" 0 -n -e "Hello" -e "Line" "$TEST_DIR/1.txt"
assert "Flag -e with -o" 0 -o -e "Hello" -e "Line" "$TEST_DIR/1.txt"

# ---------------------- 19. Сложные регулярные выражения ----------------------
assert "Regex: start of line anchor ^" 0 "^Hello" "$TEST_DIR/1.txt"
assert "Regex: end of line anchor $" 0 "2$" "$TEST_DIR/1.txt"
assert "Regex: character class []" 0 "[HL]" "$TEST_DIR/1.txt"
assert "Regex: dot ." 0 "L.ne" "$TEST_DIR/1.txt"
assert "Regex: asterisk *" 0 "Lin*e" "$TEST_DIR/1.txt"

# ---------------------- 20. Пограничные случаи ----------------------
assert "Empty pattern with -e" 0 -e "" "$TEST_DIR/1.txt"
assert "Empty pattern from file" 0 -f "$TEST_DIR/patterns_empty.txt" "$TEST_DIR/1.txt"
assert "Very long pattern" 0 -e "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "$TEST_DIR/1.txt"
assert "Pattern with tabs" 0 -e "	" "$TEST_DIR/1.txt"

# ---------------------- 21. Чтение из stdin ----------------------
cat "$TEST_DIR/1.txt" | assert "Read from stdin" 0 "Hello"
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -n" 0 -n "Line"
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -c" 0 -c "Hello" 
cat "$TEST_DIR/1.txt" | assert "Read from stdin with -o" 0 -o "Line" 

# ---------------------- 22. Комбинация -e и -f ----------------------
assert "Combination -e and -f" 0 -e "Hello" -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Combination -e and -f with -i" 0 -i -e "hello" -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"
assert "Combination -e and -f with -v" 0 -v -e "Hello" -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"

# ---------------------- 23. Несколько -f файлов ----------------------
assert "Multiple -f files" 0 -f "$TEST_DIR/patterns1.txt" -f "$TEST_DIR/patterns2.txt" "$TEST_DIR/1.txt"

# ---------------------- 24. Комбинация -e с -f и -o ----------------------
assert "-e -f with -o" 0 -o -e "Hello" -f "$TEST_DIR/patterns1.txt" "$TEST_DIR/1.txt"

# ---------------------- 25. Комбинация -c с -l (l переопределяет c) ----------------------
assert "-c with -l (l wins)" 0 -c -l "Hello" "$TEST_DIR/1.txt"
assert "-c with -l no match" 1 -c -l "xyz123" "$TEST_DIR/1.txt"

# ---------------------- 26. Комбинация -h с -l (h игнорируется) ----------------------
assert "-h with -l" 0 -h -l "Hello" "$TEST_DIR/1.txt" "$TEST_DIR/2.txt"

# ---------------------- 31. Многострочный ввод через stdin ----------------------
printf "Line1\nLine2\nLine3\n" | assert "Multi-line stdin" 0 "Line2"
printf "Line1\nLine2\nLine3\n" | assert "Multi-line stdin with -n" 0 -n "Line2"

# ---------------------- 32. Комбинация -v с -o (o игнорируется) ----------------------
assert "-v with -o (o ignored)" 0 -v -o "Hello" "$TEST_DIR/1.txt"
# ---------------------- 34. Комбинация -l с -n (n игнорируется) ----------------------
assert "-l with -n (n ignored)" 0 -l -n "Hello" "$TEST_DIR/1.txt"

# ---------------------- 35. Паттерн со спецсимволами без экранирования ----------------------
assert "Pattern with dot" 0 "L.ne" "$TEST_DIR/1.txt"
assert "Pattern with star" 0 "Lin*e" "$TEST_DIR/1.txt"
assert "Pattern with brackets" 0 "[HL]ello" "$TEST_DIR/1.txt"

# ---------------------- 36. Паттерн с экранированными спецсимволами ----------------------
assert "Escaped dot" 0 "L\.ne" "$TEST_DIR/1.txt"
assert "Escaped star" 0 "Lin\*e" "$TEST_DIR/1.txt"
# ---------------------- 9. Негативные тесты ----------------------
#assert "Non-existent file" 1 "non_existent_file.txt"
# Проверка вывода ошибки в stderr (требует расширенной логики в assert, но проверим код возврата)
#assert "File with no permissions" 1 "$TEST_DIR/no_permission.txt"

# ---------------------- 10. Массовый тест (All flags) ----------------------
# Нужно проверить как можно больше комбинаций
for combo in "-i" "-n" "-s" "-v" "-c" "-s" "-iv" "-ic" "-in" "-is" "-ns" "-nv" "-nc" "-sv" "-sc" "-siv" "-sic" "-sin"; do
    assert "Flags combo: $combo" 0 $combo "." "$TEST_DIR/1.txt"
done

# GNU long options
for combo in "--line-number" "--invert-match" "--no-messages"; do
    assert "GNU combo: $combo" 0 $combo "." "$TEST_DIR/1.txt"
done

echo "=== RESULTS ==="
echo -e "Passed: ${GREEN}$SUCCESS${NC}"
echo -e "Failed: ${RED}$FAIL${NC}"

if [ $FAIL -eq 0 ]; then
 exit 0
else
 exit 1
fi
