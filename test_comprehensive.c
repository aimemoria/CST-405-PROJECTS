// Comprehensive test demonstrating all loop types and order of operations
// This program tests: for loops, while loops, do-while loops, and operator precedence

int factorial(int n) {
    int result;
    result = 1;
    int i;

    // Using for loop for factorial calculation
    for (i = 1; i <= n; i = i + 1) {
        result = result * i;
    }

    return result;
}

int main() {
    int sum;
    sum = 0;
    int i;

    // Test 1: For loop with order of operations
    // sum = (1*2) + (2*2) + (3*2) = 2 + 4 + 6 = 12
    for (i = 1; i <= 3; i = i + 1) {
        sum = sum + i * 2;  // Tests precedence: * before +
    }
    print(sum);  // Should print 12

    // Test 2: While loop
    int count;
    count = 0;
    while (count < 3) {
        sum = sum + count;
        count = count + 1;
    }
    print(sum);  // Should print 15 (12 + 0 + 1 + 2)

    // Test 3: Do-while loop
    int value;
    value = 0;
    do {
        value = value + 1;
    } while (value < 5);
    print(value);  // Should print 5

    // Test 4: Order of operations
    int result;
    result = 10 + 5 * 2 - 6 / 3;  // 10 + 10 - 2 = 18
    print(result);  // Should print 18

    // Test 5: Function call with for loop inside
    int fact;
    fact = factorial(5);  // 5! = 120
    print(fact);  // Should print 120

    // Test 6: Nested loops
    int total;
    total = 0;
    int j;
    for (i = 1; i <= 2; i = i + 1) {
        j = 0;
        do {
            total = total + 1;
            j = j + 1;
        } while (j < 3);
    }
    print(total);  // Should print 6 (2 * 3)

    return 0;
}
