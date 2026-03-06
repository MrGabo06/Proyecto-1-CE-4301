// Simple C program that calls assembly functions
// This demonstrates C+assembly integration in RISC-V

extern int sum_to_n(int n);
extern int subtract_two_numbers(int a, int b);

#define UART_BASE 0x10000000UL

// 16550-like UART registers
#define UART_RBR 0 // Receiver Buffer Register
#define UART_THR 0 // Transmit Holding Register
#define UART_LSR 5 // Line Status Register

#define LSR_DATA_READY 0x01
#define LSR_THR_EMPTY 0x20

static volatile unsigned char *const uart =
    (volatile unsigned char *)UART_BASE;

void print_char(char c)
{
    while ((uart[UART_LSR] & LSR_THR_EMPTY) == 0)
    {
    }

    uart[UART_THR] = (unsigned char)c;
}

char read_char()
{
    while ((uart[UART_LSR] & LSR_DATA_READY) == 0)
    {
    }

    return (char)uart[UART_RBR];
}

void print_string(const char *str)
{
    while (*str)
    {
        print_char(*str++);
    }
}

void print_number(int num)
{
    if (num == 0)
    {
        print_char('0');
        return;
    }

    if (num < 0)
    {
        print_char('-');
        num = -num;
    }

    char buffer[12];
    int i = 0;

    while (num > 0)
    {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0)
    {
        print_char(buffer[--i]);
    }
}

void print_newline()
{
    print_char('\r');
    print_char('\n');
}

void read_line(char *buffer, int max_len)
{
    int i = 0;

    while (1)
    {
        char c = read_char();

        // Enter
        if (c == '\r' || c == '\n')
        {
            print_newline();
            break;
        }

        // Backspace handling
        if ((c == '\b' || c == 127) && i > 0)
        {
            i--;
            print_char('\b');
            print_char(' ');
            print_char('\b');
            continue;
        }

        if (i < max_len - 1)
        {
            buffer[i++] = c;
            print_char(c); // echo
        }
    }

    buffer[i] = '\0';
}

int parse_int(const char *str)
{
    int i = 0;
    int sign = 1;
    int value = 0;

    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }
    else if (str[0] == '+')
    {
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9')
    {
        value = value * 10 + (str[i] - '0');
        i++;
    }

    return sign * value;
}

int read_number()
{
    char buffer[32];
    read_line(buffer, sizeof(buffer));
    return parse_int(buffer);
}

void main()
{
    print_string("Testing sum_to_n assembly function");
    print_newline();
    print_newline();

    print_string("How many test values do you want to enter? ");
    int num_tests = read_number();

    if (num_tests <= 0)
    {
        print_string("Invalid amount. Nothing to test.");
        print_newline();
    }
    else
    {
        for (int i = 0; i < num_tests; i++)
        {
            print_string("Enter value #");
            print_number(i + 1);
            print_string(": ");

            int n = read_number();
            int result = sum_to_n(n);

            print_string("sum_to_n(");
            print_number(n);
            print_string(") = ");
            print_number(result);
            print_newline();
        }
    }

    print_newline();
    print_string("Now testing subtraction function");
    print_newline();
    print_newline();

    print_string("Enter first number: ");
    int a = read_number();

    print_string("Enter second number: ");
    int b = read_number();

    int subtraction_result = subtract_two_numbers(a, b);

    print_string("subtract_two_numbers(");
    print_number(a);
    print_string(", ");
    print_number(b);
    print_string(") = ");
    print_number(subtraction_result);
    print_newline();

    print_newline();
    print_string("Tests completed.");
    print_newline();

    while (1)
    {
        __asm__ volatile("nop");
    }
}