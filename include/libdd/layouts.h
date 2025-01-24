#include <vector>
#include <string>

static std::vector<std::string> keyboard_layout_a75 = {
    "ESC",    "",        "F1",     "F2",     "F3",      "F4",      "F5",
    "F6",     "F7",      "F8",     "F9",     "F10",     "F11",     "F12",
    "KP7",    "KP8",     "KP9",    "u1",     "u2",      "u3",      "u4",
    "SWUNG",  "1",       "2",      "3",      "4",       "5",       "6",
    "7",      "8",       "9",      "0",      "MINUS",   "PLUS",    "BACK",
    "KP4",    "KP5",     "KP6",    "u5",     "u6",      "u7",      "u8",
    "TAB",    "Q",       "W",      "E",      "R",       "T",       "Y",
    "U",      "I",       "O",      "P",      "BRKTS_L", "BRKTS_R", "SLASH_K29",
    "KP1",    "KP2",     "KP3",    "u9",     "u10",     "u11",     "u12",
    "CAPS",   "A",       "S",      "D",      "F",       "G",       "H",
    "J",      "K",       "L",      "COLON",  "QOTATN",  "u13",     "RETURN",
    "u14",    "KP0",     "KP_DEL", "u15",    "u16",     "u17",     "u18",
    "SHF_L",  "EUR_K45", "Z",      "X",      "C",       "V",       "B",
    "N",      "M",       "COMMA",  "PERIOD", "VIRGUE",  "u19",     "SHF_R",
    "ARR_UP", "u20",     "NUMS",   "u21",    "u22",     "u23",     "u24",
    "CTRL_L", "WIN_L",   "ALT_L",  "u25",    "u26",     "u27",     "SPACE",
    "u28",    "u29",     "u30",    "ALT_R",  "FN1",     "APP",     "",
    "ARR_L",  "ARR_DW",  "ARR_R",  "CTRL_R", "u31",     "u32",     "u33",
    "u34"};

static std::vector<std::string> keyboard_layout_g65 = {
    "",       "",        "",      "",       "",        "",        "",
    "",       "",        "",      "",       "",        "",        "",
    "",       "",        "",      "",       "",        "",        "",
    "ESC",    "1",       "2",     "3",      "4",       "5",       "6",
    "7",      "8",       "9",     "0",      "MINUS",   "PLUS",    "BACK",
    "DELETE", "",        "",      "",       "",        "",        "",
    "TAB",    "Q",       "W",     "E",      "R",       "T",       "Y",
    "U",      "I",       "O",     "P",      "BRKTS_L", "BRKTS_R", "SLASH_K29",
    "END",    "",        "",      "",       "",        "",        "",
    "CAPS",   "A",       "S",     "D",      "F",       "G",       "H",
    "J",      "K",       "L",     "COLON",  "QOTATN",  "EUR_K42", "RETURN",
    "PAGEUP", "",        "",      "",       "",        "",        "",
    "SHF_L",  "EUR_K45", "Z",     "X",      "C",       "V",       "B",
    "N",      "M",       "COMMA", "PERIOD", "VIRGUE",  "SHF_R",   "ARR_UP",
    "PAGEDW", "",        "",      "",       "",        "",        "",
    "CTRL_L", "WIN_L",   "ALT_L", "",       "",        "",        "SPACE",
    "",       "",        "ALT_R", "FN1",    "FN2",     "ARR_L",   "ARR_DW",
    "ARR_R",  "",        "",      "",       "",        "",        ""};

static std::vector<std::string> keyboard_layout_g75 = {
    "ESC",    "F1",      "F2",    "F3",     "F4",      "F5",      "F6",
    "F7",     "F8",      "F9",    "F10",    "F11",     "F12",     "PRINT",
    "INSERT", "DELETE",  "",      "",       "",        "",        "",
    "SWUNG",  "1",       "2",     "3",      "4",       "5",       "6",
    "7",      "8",       "9",     "0",      "MINUS",   "PLUS",    "BACK",
    "",       "HOME",    "",      "",       "",        "",        "",
    "TAB",    "Q",       "W",     "E",      "R",       "T",       "Y",
    "U",      "I",       "O",     "P",      "BRKTS_L", "BRKTS_R", "SLASH_K29",
    "",       "PAGEUP",  "",      "",       "",        "",        "",
    "CAPS",   "A",       "S",     "D",      "F",       "G",       "H",
    "J",      "K",       "L",     "COLON",  "QOTATN",  "EUR_K42", "RETURN",
    "",       "PAGEDW",  "",      "",       "",        "",        "",
    "SHF_L",  "EUR_K45", "Z",     "X",      "C",       "V",       "B",
    "N",      "M",       "COMMA", "PERIOD", "VIRGUE",  "SHF_R",   "ARR_UP",
    "",       "END",     "",      "",       "",        "",        "",
    "CTRL_L", "WIN_L",   "ALT_L", "",       "",        "",        "SPACE",
    "",       "",        "ALT_R", "FN1",    "",        "FN2",     "ARR_L",
    "ARR_DW", "ARR_R",   "",      "",       "",        "",        ""};


int ParseIdentifierResult(const unsigned char *buf) 
{
    unsigned char byte5 = buf[0];
    unsigned char byte6 = buf[1];
    unsigned char byte7 = buf[2];
  if (((byte5 == 11) && (byte6 == 1) && (byte7 == 1)) ||
      ((byte5 == 11) && (byte6 == 4) && (byte7 == 1))) {
    return 75;
  } else if ((byte5 == 11) && (byte6 == 4) && (byte7 == 3)) {
    return 750;
  } else if ((byte5 == 11) && (byte6 == 4) && (byte7 == 2)) {
    int current_iso = 751; // Assume UK as default
    if (current_iso == 751) {
      return 751;
    } else if (current_iso == 752) {
      return 752;
    } else if (current_iso == 753) {
      return 753;
    }
  } else if (((byte5 == 11) && (byte6 == 2) && (byte7 == 1)) ||
             ((byte5 == 15) && (byte6 == 1) && (byte7 == 1))) {
    return 65;
  } else if ((byte5 == 11) && (byte6 == 3) && (byte7 == 1)) {
    return 60;
  } else if ((byte5 == 11) && (byte6 == 4) && (byte7 == 1)) {
    return 82;
  } else if ((byte5 == 11) && (byte6 == 4) && (byte7 == 5)) {
    return 754;
  } else {
    return 0;
  }
}

std::string GetKeyboardNameFromId(int keyboard_type) {
  std::string keyboard_name;

  switch (keyboard_type) {

  case 75: {
    keyboard_name = "A75";

    break;
  }
  case 750: {
    keyboard_name = "A75Pro";

    break;
  }
  case 751: {
    keyboard_name = "A75 ISO - UK";

    break;
  }
  case 752:

  {
    keyboard_name = "A75 ISO - FR";

    break;
  }
  case 753:

  {
    keyboard_name = "A75 ISO - DE";

    break;
  }
  case 754: {
    keyboard_name = "G75";

    break;
  }
  case 65: {
    keyboard_name = "G65";

    break;
  }
  case 60:
    keyboard_name = "G60";
    break;
  case 82:
    keyboard_name = "K82";
    break;
  default:
    keyboard_name = "Unknown Keyboard";
    break;
  }

  return keyboard_name;
}