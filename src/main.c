#include <stdint.h>

extern void block(const uint32_t *key, const uint32_t *nonce, uint32_t counter, uint32_t *out);

#define UART_BASE 0x10000000UL
#define UART_RBR 0
#define UART_THR 0
#define UART_LSR 5

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
    return -1;
}

int is_separator(char c)
{
    return (c == ' ' || c == '\t' || c == ':' || c == '-' || c == ',');
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
        int hv = hex_value(str[i]);
        if (hv >= 0)
        {
            value = (value << 4) | (uint32_t)hv;
        }
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

int parse_hex_bytes_exact(const char *str, uint8_t *out, int expected_bytes)
{
    int i = 0;
    int out_index = 0;
    int high_nibble = -1;

    while (str[i] != '\0')
    {
        if (is_separator(str[i]))
        {
            i++;
            continue;
        }

        if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
        {
            i += 2;
            continue;
        }

        int hv = hex_value(str[i]);
        if (hv < 0)
        {
            return 0;
        }

        if (high_nibble < 0)
        {
            high_nibble = hv;
        }
        else
        {
            if (out_index >= expected_bytes)
            {
                return 0;
            }

            out[out_index++] = (uint8_t)((high_nibble << 4) | hv);
            high_nibble = -1;
        }

        i++;
    }

    if (high_nibble >= 0)
    {
        return 0;
    }

    return (out_index == expected_bytes);
}

void bytes_to_words_le(const uint8_t *bytes, uint32_t *words, int word_count)
{
    for (int i = 0; i < word_count; i++)
    {
        words[i] =
            ((uint32_t)bytes[i * 4 + 0]) |
            ((uint32_t)bytes[i * 4 + 1] << 8) |
            ((uint32_t)bytes[i * 4 + 2] << 16) |
            ((uint32_t)bytes[i * 4 + 3] << 24);
    }
}

void read_key(uint32_t key[8])
{
    char buffer[128];
    uint8_t key_bytes[32];

    while (1)
    {
        print_string("key: ");
        read_line(buffer, sizeof(buffer));

        if (parse_hex_bytes_exact(buffer, key_bytes, 32))
        {
            bytes_to_words_le(key_bytes, key, 8);
            return;
        }

        print_string("Entrada invalida. Debe tener exactamente 32 bytes en hexadecimal.");
        print_newline();
    }
}

void read_nonce(uint32_t nonce[3])
{
    char buffer[64];
    uint8_t nonce_bytes[12];

    while (1)
    {
        print_string("nonce: ");
        read_line(buffer, sizeof(buffer));

        if (parse_hex_bytes_exact(buffer, nonce_bytes, 12))
        {
            bytes_to_words_le(nonce_bytes, nonce, 3);
            return;
        }

        print_string("Entrada invalida. Debe tener exactamente 12 bytes en hexadecimal.");
        print_newline();
    }
}

void print_keystream(uint32_t out[16])
{
    for (int i = 0; i < 16; i++)
    {
        print_hex32(out[i]);
        print_newline();
    }
}

void main()
{
    uint32_t key[8];
    uint32_t nonce[3];
    uint32_t counter;
    uint32_t keystream[16];

    print_string("Prueba de block");
    print_newline();
    print_string("Ingrese los datos en hexadecimal.");
    print_newline();

    read_key(key);
    read_nonce(nonce);
    counter = read_hex32("counter: ");

    block(key, nonce, counter, keystream);

    print_newline();
    print_string("Keystream completo:");
    print_newline();
    print_keystream(keystream);

    while (1)
    {
        __asm__ volatile("nop");
    }
}