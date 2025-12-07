// Test program for DO-WHILE loop
// This program demonstrates the do-while loop functionality

int main() {
    int count;
    count = 0;
    int total;
    total = 0;

    // Do-while loop: executes at least once
    do {
        total = total + count;
        count = count + 1;
    } while (count < 5);

    print(total);  // Should print 10 (0+1+2+3+4)
    print(count);  // Should print 5
    return 0;
}
