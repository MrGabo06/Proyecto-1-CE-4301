#include <stdint.h>

extern uint8_t *chacha20_encrypt(const uint32_t *key, const uint32_t *nonce, uint32_t counter, const uint8_t *plaintext, uint32_t len, uint8_t *out);

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

void print_hex8(uint8_t value)
{
    const char *hex = "0123456789abcdef";
    print_char(hex[(value >> 4) & 0xF]);
    print_char(hex[value & 0xF]);
}

void print_uint32(uint32_t value)
{
    char buffer[10];
    int i = 0;

    if (value == 0)
    {
        print_char('0');
        return;
    }

    while (value > 0)
    {
        buffer[i++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (i > 0)
    {
        print_char(buffer[--i]);
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

uint32_t string_length(const char *str)
{
    uint32_t len = 0;

    while (str[len] != '\0')
    {
        len++;
    }

    return len;
}

void print_buffer_hex(const uint8_t *buffer, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        print_hex8(buffer[i]);
        if (i + 1 < len)
        {
            print_char(' ');
        }
    }
    print_newline();
}

int ask_continue(void)
{
    char buffer[8];

    while (1)
    {
        print_newline();
        print_string("Desea continuar? (y/n): ");
        read_line(buffer, sizeof(buffer));

        if (buffer[0] == 'y' || buffer[0] == 'Y')
        {
            return 1;
        }

        if (buffer[0] == 'n' || buffer[0] == 'N')
        {
            return 0;
        }

        print_string("Entrada invalida. Use y o n.");
        print_newline();
    }
}

void main()
{
    uint32_t key[8];
    uint32_t nonce[3];
    uint32_t counter;

    uint8_t plaintext[256];
    uint8_t ciphertext[256];
    uint8_t decrypted[256];

    uint32_t len;

    while (1)
    {
        print_string("Prueba de chacha20_encrypt");
        print_newline();
        print_string("Ingrese los datos.");
        print_newline();

        read_key(key);
        read_nonce(nonce);
        counter = read_hex32("counter: ");

        print_string("plaintext: ");
        read_line((char *)plaintext, sizeof(plaintext));

        len = string_length((const char *)plaintext);

        chacha20_encrypt(key, nonce, counter, plaintext, len, ciphertext);

        print_newline();
        print_string("Ciphertext (hex):");
        print_newline();
        print_buffer_hex(ciphertext, len);

        chacha20_encrypt(key, nonce, counter, ciphertext, len, decrypted);
        decrypted[len] = '\0';

        print_newline();
        print_string("Texto recuperado:");
        print_newline();
        print_string((const char *)decrypted);
        print_newline();

        if (!ask_continue())
        {
            print_newline();
            print_string("Programa finalizado.");
            print_newline();
            break;
        }

        print_newline();
    }

    while (1)
    {
        __asm__ volatile("nop");
    }
}