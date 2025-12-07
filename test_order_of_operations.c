// Test program for order of operations
// Demonstrates operator precedence: *, /, % have higher precedence than +, -

int main() {
    int result;

    // Test 1: 2 + 3 * 4 = 2 + 12 = 14 (not 20)
    result = 2 + 3 * 4;
    print(result);  // Should print 14

    // Test 2: 10 - 6 / 2 = 10 - 3 = 7 (not 2)
    result = 10 - 6 / 2;
    print(result);  // Should print 7

    // Test 3: 5 * 2 + 3 = 10 + 3 = 13
    result = 5 * 2 + 3;
    print(result);  // Should print 13

    // Test 4: 15 / 3 - 2 = 5 - 2 = 3
    result = 15 / 3 - 2;
    print(result);  // Should print 3

    // Test 5: 10 % 3 + 2 = 1 + 2 = 3
    result = 10 % 3 + 2;
    print(result);  // Should print 3

    // Test 6: (2 + 3) * 4 = 5 * 4 = 20 (with parentheses)
    result = (2 + 3) * 4;
    print(result);  // Should print 20

    // Test 7: Complex expression: 2 + 3 * 4 - 6 / 2 = 2 + 12 - 3 = 11
    result = 2 + 3 * 4 - 6 / 2;
    print(result);  // Should print 11

    return 0;
}
