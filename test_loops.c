/* Test 2: Control Structures - Loops */

int main() {
    int i;
    int sum;
    int n;

    /* While loop */
    i = 0;
    sum = 0;
    while (i < 5) {
        sum = sum + i;
        i = i + 1;
    }
    print(sum);

    /* For loop */
    sum = 0;
    for (i = 0; i < 5; i = i + 1;) {
        sum = sum + i;
    }
    print(sum);

    /* Do-while loop */
    i = 0;
    sum = 0;
    do {
        sum = sum + i;
        i = i + 1;
    } while (i < 5);
    print(sum);

    return 0;
}
