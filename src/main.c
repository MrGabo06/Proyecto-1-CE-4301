#include <stdint.h>

extern void quarter_round(void);
extern uint32_t *block(const uint32_t *key, const uint32_t *nonce, uint32_t counter, uint32_t *out);
extern uint8_t *chacha20_encrypt(const uint32_t *key, const uint32_t *nonce, uint32_t counter,
                                 const uint8_t *plaintext, uint32_t len, uint8_t *out);

#define UART_BASE 0x10000000UL
#define UART_THR 0
#define UART_LSR 5

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

void print_string(const char *str)
{
    while (*str)
    {
        print_char(*str++);
    }
}

void print_newline(void)
{
    print_char('\r');
    print_char('\n');
}

void print_hex8(uint8_t value)
{
    static const char hex[] = "0123456789abcdef";
    print_char(hex[(value >> 4) & 0xF]);
    print_char(hex[value & 0xF]);
}

void print_hex32(uint32_t value)
{
    static const char hex[] = "0123456789abcdef";

    for (int shift = 28; shift >= 0; shift -= 4)
    {
        print_char(hex[(value >> shift) & 0xF]);
    }
}

void print_words(const uint32_t *words, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        print_hex32(words[i]);

        if ((i % 4) == 3 || i + 1 == count)
        {
            print_newline();
        }
        else
        {
            print_char(' ');
        }
    }
}

void print_bytes_hex(const uint8_t *buffer, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        print_hex8(buffer[i]);

        if ((i % 16) == 15 || i + 1 == len)
        {
            print_newline();
        }
        else
        {
            print_char(' ');
        }
    }
}

void print_named_word(const char *label, uint32_t value)
{
    print_string(label);
    print_string(": ");
    print_hex32(value);
    print_newline();
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

void print_repeat_char(char c, int count)
{
    for (int i = 0; i < count; i++)
    {
        print_char(c);
    }
}

void print_separator(void)
{
    print_repeat_char('-', 50);
    print_newline();
}

void print_big_separator(void)
{
    print_repeat_char('=', 50);
    print_newline();
}

void print_section_title(const char *title)
{
    print_big_separator();
    print_string(title);
    print_newline();
    print_big_separator();
    print_newline();
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

void print_test_title(uint32_t test_number)
{
    print_separator();
    print_string("PRUEBA ");
    print_uint32(test_number);
    print_newline();
    print_separator();
}

int words_equal(const uint32_t *a, const uint32_t *b, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (a[i] != b[i])
        {
            return 0;
        }
    }

    return 1;
}

int bytes_equal(const uint8_t *a, const uint8_t *b, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (a[i] != b[i])
        {
            return 0;
        }
    }

    return 1;
}

typedef struct
{
    const char *name;
    const uint32_t *key;
    const uint32_t *nonce;
    uint32_t counter;
    const uint32_t *expected;
} block_test_vector_t;

typedef struct
{
    const char *name;
    const uint32_t *key;
    const uint32_t *nonce;
    uint32_t counter;
    const uint8_t *plaintext;
    uint32_t plaintext_len;
    const uint8_t *expected;
} encrypt_test_vector_t;

static const uint32_t quarter_input[4] = {
    0x11111111,
    0x01020304,
    0x9b8d6f43,
    0x01234567};

static const uint32_t quarter_expected[4] = {
    0xea2a92f4,
    0xcb1cf8ce,
    0x4581472e,
    0x5881c4bb};

static const uint32_t quarter_state_input[16] = {
    0x879531e0, 0xc5ecf37d, 0x516461b1, 0xc9a62f8a,
    0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0x2a5f714c,
    0x53372767, 0xb00a5631, 0x974c541a, 0x359e9963,
    0x5c971061, 0x3d631689, 0x2098d9d6, 0x91dbd320};

static const uint32_t quarter_state_expected[16] = {
    0x879531e0, 0xc5ecf37d, 0xbdb886dc, 0xc9a62f8a,
    0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0xcfacafd2,
    0xe46bea80, 0xb00a5631, 0x974c541a, 0x359e9963,
    0x5c971061, 0xccc07c79, 0x2098d9d6, 0x91dbd320};

static const uint32_t key_all_zero[8] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

static const uint32_t key_seq_00_1f[8] = {
    0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
    0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};

static const uint32_t key_last_byte_01[8] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01000000};

static const uint32_t key_00ff0000[8] = {
    0x0000ff00, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

static const uint32_t key_a2_v3[8] = {
    0xa540921c, 0x8ad355eb, 0x868833f3, 0xf0b5f604,
    0xc1173947, 0x09802b40, 0xbc5cca9d, 0xc0757020};

static const uint32_t nonce_all_zero[3] = {
    0x00000000, 0x00000000, 0x00000000};

static const uint32_t nonce_00000009_0000004a_00000000[3] = {
    0x09000000, 0x4a000000, 0x00000000};

static const uint32_t nonce_00000000_0000004a_00000000[3] = {
    0x00000000, 0x4a000000, 0x00000000};

static const uint32_t nonce_last_byte_02[3] = {
    0x00000000, 0x00000000, 0x02000000};

static const uint32_t block_main_expected[16] = {
    0xe4e7f110, 0x15593bd1, 0x1fdd0f50, 0xc47120a3,
    0xc7f4d1c7, 0x0368c033, 0x9aaa2204, 0x4e6cd4c3,
    0x466482d2, 0x09aa9f07, 0x05d7c214, 0xa2028bd9,
    0xd19c12b5, 0xb94e16de, 0xe883d0cb, 0x4e3c50a2};

static const uint32_t block_a1_v1_expected[16] = {
    0xade0b876, 0x903df1a0, 0xe56a5d40, 0x28bd8653,
    0xb819d2bd, 0x1aed8da0, 0xccef36a8, 0xc70d778b,
    0x7c5941da, 0x8d485751, 0x3fe02477, 0x374ad8b8,
    0xf4b8436a, 0x1ca11815, 0x69b687c3, 0x8665eeb2};

static const uint32_t block_a1_v2_expected[16] = {
    0xbee7079f, 0x7a385155, 0x7c97ba98, 0x0d082d73,
    0xa0290fcb, 0x6965e348, 0x3e53c612, 0xed7aee32,
    0x7621b729, 0x434ee69c, 0xb03371d5, 0xd539d874,
    0x281fed31, 0x45fb0a51, 0x1f0ae1ac, 0x6f4d794b};

static const uint32_t block_a1_v3_expected[16] = {
    0x2452eb3a, 0x9249f8ec, 0x8d829d9b, 0xddd4ceb1,
    0xe8252083, 0x60818b01, 0xf38422b8, 0x5aaa49c9,
    0xbb00ca8e, 0xda3ba7b4, 0xc4b592d1, 0xfdf2732f,
    0x4436274e, 0x2561b3c8, 0xebdd4aa6, 0xa0136c00};

static const uint32_t block_a1_v4_expected[16] = {
    0xfb4dd572, 0x4bc42ef1, 0xdf922636, 0x327f1394,
    0xa78dea8f, 0x5e269039, 0xa1bebbc1, 0xcaf09aae,
    0xa25ab213, 0x48a6b46c, 0x1b9d9bcb, 0x092c5be6,
    0x546ca624, 0x1bec45d5, 0x87f47473, 0x96f0992e};

static const uint32_t block_a1_v5_expected[16] = {
    0x374dc6c2, 0x3736d58c, 0xb904e24a, 0xcd3f93ef,
    0x88228b1a, 0x96a4dfb3, 0x5b76ab72, 0xc727ee54,
    0x0e0e978a, 0xf3145c95, 0x1b748ea8, 0xf786c297,
    0x99c28f5f, 0x628314e8, 0x398a19fa, 0x6ded1b53};

static const block_test_vector_t block_vectors[] = {
    {"RFC 2.3.2", key_seq_00_1f, nonce_00000009_0000004a_00000000, 1, block_main_expected},
    {"RFC A.1 #1", key_all_zero, nonce_all_zero, 0, block_a1_v1_expected},
    {"RFC A.1 #2", key_all_zero, nonce_all_zero, 1, block_a1_v2_expected},
    {"RFC A.1 #3", key_last_byte_01, nonce_all_zero, 1, block_a1_v3_expected},
    {"RFC A.1 #4", key_00ff0000, nonce_all_zero, 2, block_a1_v4_expected},
    {"RFC A.1 #5", key_all_zero, nonce_last_byte_02, 0, block_a1_v5_expected},
};

static const uint8_t encrypt_main_plaintext[] =
    "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.";

static const uint8_t encrypt_main_expected[] = {
    0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80,
    0x41, 0xba, 0x07, 0x28, 0xdd, 0x0d, 0x69, 0x81,
    0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2,
    0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b,
    0xf9, 0x1b, 0x65, 0xc5, 0x52, 0x47, 0x33, 0xab,
    0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57,
    0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab,
    0x8f, 0x53, 0x0c, 0x35, 0x9f, 0x08, 0x61, 0xd8,
    0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61,
    0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e,
    0x52, 0xbc, 0x51, 0x4d, 0x16, 0xcc, 0xf8, 0x06,
    0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36,
    0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6,
    0xb4, 0x0b, 0x8e, 0xed, 0xf2, 0x78, 0x5e, 0x42,
    0x87, 0x4d};

static const uint8_t encrypt_a2_v1_plaintext[64] = {0};

static const uint8_t encrypt_a2_v1_expected[64] = {
    0x76, 0xb8, 0xe0, 0xad, 0xa0, 0xf1, 0x3d, 0x90,
    0x40, 0x5d, 0x6a, 0xe5, 0x53, 0x86, 0xbd, 0x28,
    0xbd, 0xd2, 0x19, 0xb8, 0xa0, 0x8d, 0xed, 0x1a,
    0xa8, 0x36, 0xef, 0xcc, 0x8b, 0x77, 0x0d, 0xc7,
    0xda, 0x41, 0x59, 0x7c, 0x51, 0x57, 0x48, 0x8d,
    0x77, 0x24, 0xe0, 0x3f, 0xb8, 0xd8, 0x4a, 0x37,
    0x6a, 0x43, 0xb8, 0xf4, 0x15, 0x18, 0xa1, 0x1c,
    0xc3, 0x87, 0xb6, 0x69, 0xb2, 0xee, 0x65, 0x86};

static const uint8_t encrypt_a2_v2_plaintext[] =
    "Any submission to the IETF intended by the Contributor for publication as all or part of an IETF "
    "Internet-Draft or RFC and any statement made within the context of an IETF activity is considered "
    "an \"IETF Contribution\". Such statements include oral statements in IETF sessions, as well as "
    "written and electronic communications made at any time or place, which are addressed to";

static const uint8_t encrypt_a2_v2_expected[] = {
    0xa3, 0xfb, 0xf0, 0x7d, 0xf3, 0xfa, 0x2f, 0xde, 0x4f, 0x37, 0x6c, 0xa2, 0x3e, 0x82, 0x73, 0x70,
    0x41, 0x60, 0x5d, 0x9f, 0x4f, 0x4f, 0x57, 0xbd, 0x8c, 0xff, 0x2c, 0x1d, 0x4b, 0x79, 0x55, 0xec,
    0x2a, 0x97, 0x94, 0x8b, 0xd3, 0x72, 0x29, 0x15, 0xc8, 0xf3, 0xd3, 0x37, 0xf7, 0xd3, 0x70, 0x05,
    0x0e, 0x9e, 0x96, 0xd6, 0x47, 0xb7, 0xc3, 0x9f, 0x56, 0xe0, 0x31, 0xca, 0x5e, 0xb6, 0x25, 0x0d,
    0x40, 0x42, 0xe0, 0x27, 0x85, 0xec, 0xec, 0xfa, 0x4b, 0x4b, 0xb5, 0xe8, 0xea, 0xd0, 0x44, 0x0e,
    0x20, 0xb6, 0xe8, 0xdb, 0x09, 0xd8, 0x81, 0xa7, 0xc6, 0x13, 0x2f, 0x42, 0x0e, 0x52, 0x79, 0x50,
    0x42, 0xbd, 0xfa, 0x77, 0x73, 0xd8, 0xa9, 0x05, 0x14, 0x47, 0xb3, 0x29, 0x1c, 0xe1, 0x41, 0x1c,
    0x68, 0x04, 0x65, 0x55, 0x2a, 0xa6, 0xc4, 0x05, 0xb7, 0x76, 0x4d, 0x5e, 0x87, 0xbe, 0xa8, 0x5a,
    0xd0, 0x0f, 0x84, 0x49, 0xed, 0x8f, 0x72, 0xd0, 0xd6, 0x62, 0xab, 0x05, 0x26, 0x91, 0xca, 0x66,
    0x42, 0x4b, 0xc8, 0x6d, 0x2d, 0xf8, 0x0e, 0xa4, 0x1f, 0x43, 0xab, 0xf9, 0x37, 0xd3, 0x25, 0x9d,
    0xc4, 0xb2, 0xd0, 0xdf, 0xb4, 0x8a, 0x6c, 0x91, 0x39, 0xdd, 0xd7, 0xf7, 0x69, 0x66, 0xe9, 0x28,
    0xe6, 0x35, 0x55, 0x3b, 0xa7, 0x6c, 0x5c, 0x87, 0x9d, 0x7b, 0x35, 0xd4, 0x9e, 0xb2, 0xe6, 0x2b,
    0x08, 0x71, 0xcd, 0xac, 0x63, 0x89, 0x39, 0xe2, 0x5e, 0x8a, 0x1e, 0x0e, 0xf9, 0xd5, 0x28, 0x0f,
    0xa8, 0xca, 0x32, 0x8b, 0x35, 0x1c, 0x3c, 0x76, 0x59, 0x89, 0xcb, 0xcf, 0x3d, 0xaa, 0x8b, 0x6c,
    0xcc, 0x3a, 0xaf, 0x9f, 0x39, 0x79, 0xc9, 0x2b, 0x37, 0x20, 0xfc, 0x88, 0xdc, 0x95, 0xed, 0x84,
    0xa1, 0xbe, 0x05, 0x9c, 0x64, 0x99, 0xb9, 0xfd, 0xa2, 0x36, 0xe7, 0xe8, 0x18, 0xb0, 0x4b, 0x0b,
    0xc3, 0x9c, 0x1e, 0x87, 0x6b, 0x19, 0x3b, 0xfe, 0x55, 0x69, 0x75, 0x3f, 0x88, 0x12, 0x8c, 0xc0,
    0x8a, 0xaa, 0x9b, 0x63, 0xd1, 0xa1, 0x6f, 0x80, 0xef, 0x25, 0x54, 0xd7, 0x18, 0x9c, 0x41, 0x1f,
    0x58, 0x69, 0xca, 0x52, 0xc5, 0xb8, 0x3f, 0xa3, 0x6f, 0xf2, 0x16, 0xb9, 0xc1, 0xd3, 0x00, 0x62,
    0xbe, 0xbc, 0xfd, 0x2d, 0xc5, 0xbc, 0xe0, 0x91, 0x19, 0x34, 0xfd, 0xa7, 0x9a, 0x86, 0xf6, 0xe6,
    0x98, 0xce, 0xd7, 0x59, 0xc3, 0xff, 0x9b, 0x64, 0x77, 0x33, 0x8f, 0x3d, 0xa4, 0xf9, 0xcd, 0x85,
    0x14, 0xea, 0x99, 0x82, 0xcc, 0xaf, 0xb3, 0x41, 0xb2, 0x38, 0x4d, 0xd9, 0x02, 0xf3, 0xd1, 0xab,
    0x7a, 0xc6, 0x1d, 0xd2, 0x9c, 0x6f, 0x21, 0xba, 0x5b, 0x86, 0x2f, 0x37, 0x30, 0xe3, 0x7c, 0xfd,
    0xc4, 0xfd, 0x80, 0x6c, 0x22, 0xf2, 0x21};

static const uint8_t encrypt_a2_v3_plaintext[] =
    "'Twas brillig, and the slithy toves\n"
    "Did gyre and gimble in the wabe:\n"
    "All mimsy were the borogoves,\n"
    "And the mome raths outgrabe.";

static const uint8_t encrypt_a2_v3_expected[] = {
    0x62, 0xe6, 0x34, 0x7f, 0x95, 0xed, 0x87, 0xa4, 0x5f, 0xfa, 0xe7, 0x42, 0x6f, 0x27, 0xa1, 0xdf,
    0x5f, 0xb6, 0x91, 0x10, 0x04, 0x4c, 0x0d, 0x73, 0x11, 0x8e, 0xff, 0xa9, 0x5b, 0x01, 0xe5, 0xcf,
    0x16, 0x6d, 0x3d, 0xf2, 0xd7, 0x21, 0xca, 0xf9, 0xb2, 0x1e, 0x5f, 0xb1, 0x4c, 0x61, 0x68, 0x71,
    0xfd, 0x84, 0xc5, 0x4f, 0x9d, 0x65, 0xb2, 0x83, 0x19, 0x6c, 0x7f, 0xe4, 0xf6, 0x05, 0x53, 0xeb,
    0xf3, 0x9c, 0x64, 0x02, 0xc4, 0x22, 0x34, 0xe3, 0x2a, 0x35, 0x6b, 0x3e, 0x76, 0x43, 0x12, 0xa6,
    0x1a, 0x55, 0x32, 0x05, 0x57, 0x16, 0xea, 0xd6, 0x96, 0x25, 0x68, 0xf8, 0x7d, 0x3f, 0x3f, 0x77,
    0x04, 0xc6, 0xa8, 0xd1, 0xbc, 0xd1, 0xbf, 0x4d, 0x50, 0xd6, 0x15, 0x4b, 0x6d, 0xa7, 0x31, 0xb1,
    0x87, 0xb5, 0x8d, 0xfd, 0x72, 0x8a, 0xfa, 0x36, 0x75, 0x7a, 0x79, 0x7a, 0xc1, 0x88, 0xd1};

static const encrypt_test_vector_t encrypt_vectors[] = {
    {"RFC 2.4.2", key_seq_00_1f, nonce_00000000_0000004a_00000000, 1,
     encrypt_main_plaintext, sizeof(encrypt_main_plaintext) - 1, encrypt_main_expected},

    {"RFC A.2 #1", key_all_zero, nonce_all_zero, 0,
     encrypt_a2_v1_plaintext, sizeof(encrypt_a2_v1_plaintext), encrypt_a2_v1_expected},

    {"RFC A.2 #2", key_last_byte_01, nonce_last_byte_02, 1,
     encrypt_a2_v2_plaintext, sizeof(encrypt_a2_v2_plaintext) - 1, encrypt_a2_v2_expected},

    {"RFC A.2 #3", key_a2_v3, nonce_last_byte_02, 42,
     encrypt_a2_v3_plaintext, sizeof(encrypt_a2_v3_plaintext) - 1, encrypt_a2_v3_expected},
};

void quarter_round_call(uint32_t in0, uint32_t in1, uint32_t in2, uint32_t in3,
                        uint32_t *out0, uint32_t *out1, uint32_t *out2, uint32_t *out3)
{
    register uint32_t a0_reg __asm__("a0") = in0;
    register uint32_t a1_reg __asm__("a1") = in1;
    register uint32_t a2_reg __asm__("a2") = in2;
    register uint32_t a3_reg __asm__("a3") = in3;

    __asm__ volatile(
        "call quarter_round"
        : "+r"(a0_reg), "+r"(a1_reg), "+r"(a2_reg), "+r"(a3_reg)
        :
        : "ra",
          "t0", "t1", "t2", "t3", "t4", "t5", "t6",
          "a4", "a5", "a6", "a7",
          "memory");

    *out0 = a0_reg;
    *out1 = a1_reg;
    *out2 = a2_reg;
    *out3 = a3_reg;
}

int is_printable_text(const uint8_t *buffer, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        uint8_t c = buffer[i];

        if (c == '\n' || c == '\r' || c == '\t')
        {
            continue;
        }

        if (c < 32 || c > 126)
        {
            return 0;
        }
    }

    return 1;
}

void print_text_or_hex(const uint8_t *buffer, uint32_t len)
{
    if (is_printable_text(buffer, len))
    {
        for (uint32_t i = 0; i < len; i++)
        {
            print_char((char)buffer[i]);
        }
        print_newline();
    }
    else
    {
        print_bytes_hex(buffer, len);
    }
}

void quarter_test(void)
{
    uint32_t result[4];
    uint32_t state_result[16];

    print_test_title(1);

    quarter_round_call(
        quarter_input[0],
        quarter_input[1],
        quarter_input[2],
        quarter_input[3],
        &result[0], &result[1], &result[2], &result[3]);

    print_string("datos ingresados");
    print_newline();
    print_named_word("a", quarter_input[0]);
    print_named_word("b", quarter_input[1]);
    print_named_word("c", quarter_input[2]);
    print_named_word("d", quarter_input[3]);

    print_string("resultado obtenido");
    print_newline();
    print_words(result, 4);

    print_string("resultado esperado");
    print_newline();
    print_words(quarter_expected, 4);

    print_string(words_equal(result, quarter_expected, 4) ? "OK" : "FALLO");
    print_newline();
    print_newline();

    print_test_title(2);

    for (uint32_t i = 0; i < 16; i++)
    {
        state_result[i] = quarter_state_input[i];
    }

    quarter_round_call(
        state_result[2],
        state_result[7],
        state_result[8],
        state_result[13],
        &state_result[2],
        &state_result[7],
        &state_result[8],
        &state_result[13]);

    print_string("datos ingresados:");
    print_newline();
    print_string("estado inicial:");
    print_newline();
    print_words(quarter_state_input, 16);

    print_string("quarter round aplicado:");
    print_newline();
    print_string("(2, 7, 8, 13)");
    print_newline();

    print_string("resultado obtenido:");
    print_newline();
    print_words(state_result, 16);

    print_string("resultado esperado:");
    print_newline();
    print_words(quarter_state_expected, 16);

    print_string(words_equal(state_result, quarter_state_expected, 16) ? "OK" : "FALLO");
    print_newline();
}

void block_test(void)
{
    uint32_t result[16];
    uint32_t total = sizeof(block_vectors) / sizeof(block_vectors[0]);

    for (uint32_t i = 0; i < total; i++)
    {
        const block_test_vector_t *v = &block_vectors[i];

        print_test_title(i + 1);

        block(v->key, v->nonce, v->counter, result);

        print_string("datos ingresados:");
        print_newline();

        print_string("key:");
        print_newline();
        print_words(v->key, 8);

        print_string("nonce:");
        print_newline();
        print_words(v->nonce, 3);

        print_named_word("counter", v->counter);

        print_string("resultado obtenido:");
        print_newline();
        print_words(result, 16);

        print_string("resultado esperado:");
        print_newline();
        print_words(v->expected, 16);

        print_string(words_equal(result, v->expected, 16) ? "OK" : "FALLO");
        print_newline();
        print_newline();
    }
}

void encrypt_test(void)
{
    uint8_t result[512];
    uint8_t recovered[512];
    uint32_t total = sizeof(encrypt_vectors) / sizeof(encrypt_vectors[0]);

    for (uint32_t i = 0; i < total; i++)
    {
        const encrypt_test_vector_t *v = &encrypt_vectors[i];
        int cipher_ok;
        int recover_ok;

        print_test_title(i + 1);

        chacha20_encrypt(v->key, v->nonce, v->counter, v->plaintext, v->plaintext_len, result);
        chacha20_encrypt(v->key, v->nonce, v->counter, result, v->plaintext_len, recovered);

        cipher_ok = bytes_equal(result, v->expected, v->plaintext_len);
        recover_ok = bytes_equal(recovered, v->plaintext, v->plaintext_len);

        print_string("texto ingresado:");
        print_newline();
        print_text_or_hex(v->plaintext, v->plaintext_len);

        print_string("key:");
        print_newline();
        print_words(v->key, 8);

        print_string("nonce:");
        print_newline();
        print_words(v->nonce, 3);

        print_named_word("counter", v->counter);

        print_string("resultado obtenido:");
        print_newline();
        print_bytes_hex(result, v->plaintext_len);

        print_string("resultado esperado:");
        print_newline();
        print_bytes_hex(v->expected, v->plaintext_len);

        print_string("texto recuperado:");
        print_newline();
        print_text_or_hex(recovered, v->plaintext_len);

        print_string((cipher_ok && recover_ok) ? "OK" : "FALLO");
        print_newline();
        print_newline();
    }
}

void main(void)
{
    print_section_title("PRUEBAS DE QUARTER ROUND");
    quarter_test();
    print_newline();
    print_newline();

    print_section_title("PRUEBAS DE BLOCK");
    block_test();

    print_section_title("PRUEBAS DE ENCRYPT");
    encrypt_test();

    while (1)
    {
        __asm__ volatile("nop");
    }
}