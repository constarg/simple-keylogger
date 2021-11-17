#include <kb_decoder.h>
#include <linux/input-event-codes.h>
#include <stddef.h>

#define KEY_NUMBERS 52

#define KEY_ALT 56
#define KEY_SUPER 125

static struct kb_dec_key kb_dec_keys[KEY_NUMBERS] = {
        {.kb_key_code = KEY_1,          .kb_key_name = "1"          },
        {.kb_key_code = KEY_2,          .kb_key_name = "2"          },
        {.kb_key_code = KEY_3,          .kb_key_name = "3"          },
        {.kb_key_code = KEY_4,          .kb_key_name = "4"          },
        {.kb_key_code = KEY_5,          .kb_key_name = "5"          },
        {.kb_key_code = KEY_6,          .kb_key_name = "6"          },
        {.kb_key_code = KEY_7,          .kb_key_name = "7"          },
        {.kb_key_code = KEY_8,          .kb_key_name = "8"          },
        {.kb_key_code = KEY_9,          .kb_key_name = "9"          },
        {.kb_key_code = KEY_0,          .kb_key_name = "0"          },
        {.kb_key_code = KEY_ESC,        .kb_key_name = "ESC"        },
        {.kb_key_code = KEY_MINUS,      .kb_key_name = "-"          },
        {.kb_key_code = KEY_EQUAL,      .kb_key_name = "="          },
        {.kb_key_code = KEY_BACKSPACE,  .kb_key_name = "{BACKSPACE}"},
        {.kb_key_code = KEY_TAB,        .kb_key_name = "{TAB}"      },
        {.kb_key_code = KEY_A,          .kb_key_name = "A"          },
        {.kb_key_code = KEY_B,          .kb_key_name = "B"          },
        {.kb_key_code = KEY_C,          .kb_key_name = "C"          },
        {.kb_key_code = KEY_D,          .kb_key_name = "D"          },
        {.kb_key_code = KEY_E,          .kb_key_name = "E"          },
        {.kb_key_code = KEY_F,          .kb_key_name = "F"          },
        {.kb_key_code = KEY_G,          .kb_key_name = "G"          },
        {.kb_key_code = KEY_H,          .kb_key_name = "H"          },
        {.kb_key_code = KEY_I,          .kb_key_name = "I"          },
        {.kb_key_code = KEY_J,          .kb_key_name = "J"          },
        {.kb_key_code = KEY_K,          .kb_key_name = "K"          },
        {.kb_key_code = KEY_L,          .kb_key_name = "L"          },
        {.kb_key_code = KEY_M,          .kb_key_name = "M"          },
        {.kb_key_code = KEY_N,          .kb_key_name = "N"          },
        {.kb_key_code = KEY_O,          .kb_key_name = "O"          },
        {.kb_key_code = KEY_P,          .kb_key_name = "P"          },
        {.kb_key_code = KEY_Q,          .kb_key_name = "Q"          },
        {.kb_key_code = KEY_R,          .kb_key_name = "R"          },
        {.kb_key_code = KEY_S,          .kb_key_name = "S"          },
        {.kb_key_code = KEY_T,          .kb_key_name = "T"          },
        {.kb_key_code = KEY_U,          .kb_key_name = "U"          },
        {.kb_key_code = KEY_V,          .kb_key_name = "V"          },
        {.kb_key_code = KEY_W,          .kb_key_name = "W"          },
        {.kb_key_code = KEY_X,          .kb_key_name = "X"          },
        {.kb_key_code = KEY_Y,          .kb_key_name = "Y"          },
        {.kb_key_code = KEY_Z,          .kb_key_name = "Z"          },
        {.kb_key_code = KEY_ENTER,      .kb_key_name = "{ENTER}"    },
        {.kb_key_code = KEY_LEFTCTRL,   .kb_key_name = "{CTRL}"     },
        {.kb_key_code = KEY_LEFTSHIFT,  .kb_key_name = "{SHIFT}"    },
        {.kb_key_code = KEY_COMMA,      .kb_key_name = ","          },
        {.kb_key_code = KEY_DOT,        .kb_key_name = "."          },
        {.kb_key_code = KEY_RIGHTSHIFT, .kb_key_name = "{SHIFT}"    },
        {.kb_key_code = KEY_SPACE,      .kb_key_name = "{SPACE}"    },
        {.kb_key_code = KEY_CAPSLOCK,   .kb_key_name = "{CAPTSLOCK}"},
        {.kb_key_code = KEY_ALT,        .kb_key_name = "{ALT}"      },
        {.kb_key_code = KEY_SUPER,      .kb_key_name = "{SUPER}"    }
};


struct kb_dec_key *decode(int key_code) {
    for (int key = 0; key < KEY_NUMBERS; key++) if (kb_dec_keys[key].kb_key_code == key_code) return &kb_dec_keys[key];
    return NULL;
}

