#include <stdint.h>

extern void quarter_round(void);

#define UART_BASE 0x10000000UL
#define UART_RBR 0
#define UART_THR 0
#define UART_LSR 5

#define LSR_DATA_READY 0x01
#define LSR_THR_EMPTY 0x20

static volatile unsigned char *const uart =
    (volatile unsigned char *)UART_BASE;

typedef struct
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
} quarter_round_result_t;

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

        if (c == '\r' || c == '\n')
        {
            print_newline();
            break;
        }

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
            print_char(c);
        }
    }

    buffer[i] = '\0';
}

int hex_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    return 0;
}

uint32_t parse_hex32(const char *str)
{
    uint32_t value = 0;
    int i = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        i = 2;
    }

    while (str[i] != '\0')
    {
        value = (value << 4) | (uint32_t)hex_value(str[i]);
        i++;
    }

    return value;
}

uint32_t read_hex32(const char *prompt)
{
    char buffer[32];
    print_string(prompt);
    read_line(buffer, sizeof(buffer));
    return parse_hex32(buffer);
}

void print_hex32(uint32_t value)
{
    const char *hex = "0123456789abcdef";

    for (int i = 7; i >= 0; i--)
    {
        uint32_t nibble = (value >> (i * 4)) & 0xF;
        print_char(hex[nibble]);
    }
}

quarter_round_result_t call_quarter_round(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    register uint32_t reg_a0 asm("a0") = a;
    register uint32_t reg_a1 asm("a1") = b;
    register uint32_t reg_a2 asm("a2") = c;
    register uint32_t reg_a3 asm("a3") = d;

    asm volatile(
        "call quarter_round"
        : "+r"(reg_a0), "+r"(reg_a1), "+r"(reg_a2), "+r"(reg_a3)
        :
        : "ra", "memory");

    quarter_round_result_t out;
    out.a = reg_a0;
    out.b = reg_a1;
    out.c = reg_a2;
    out.d = reg_a3;

    return out;
}

void main()
{
    uint32_t a = read_hex32("a: ");
    uint32_t b = read_hex32("b: ");
    uint32_t c = read_hex32("c: ");
    uint32_t d = read_hex32("d: ");

    quarter_round_result_t qr = call_quarter_round(a, b, c, d);
    print_newline();
    print_string("Resultado:------------------------------------------------- ");
    print_newline();

    print_string("a: ");
    print_hex32(qr.a);
    print_newline();

    print_string("b: ");
    print_hex32(qr.b);
    print_newline();

    print_string("c: ");
    print_hex32(qr.c);
    print_newline();

    print_string("d: ");
    print_hex32(qr.d);
    print_newline();

    while (1)
    {
        __asm__ volatile("nop");
    }
}