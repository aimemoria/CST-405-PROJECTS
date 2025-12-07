/* Test Security Analysis Features */

int main() {
    int arr[10];
    int x;
    int y;

    /* Test 1: Buffer overflow - out of bounds access */
    arr[15] = 100;  /* Security warning: index 15 out of bounds [0..9] */

    /* Test 2: Division by zero */
    x = 10 / 0;  /* Error: division by zero */

    /* Test 3: Integer overflow */
    y = 2147483647 + 1;  /* Warning: integer overflow */

    /* Test 4: Uninitialized variable use */
    int z;
    print(z);  /* Warning: use before initialization */

    /* Test 5: Infinite loop */
    while (1) {  /* Warning: infinite loop */
        x = x + 1;
    }

    return 0;
}
