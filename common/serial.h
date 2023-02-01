#ifndef COMMON_SERIAL_H
#define COMMON_SERIAL_H

class SerialAdapter {
public:
  static void begin() {
    // Already configured in Setup().
    // Serial.begin(115200);
  }
  static bool Connected() { return !!Serial; }
  static bool AlwaysConnected() { return false; }
  static Stream& stream() { return Serial; }
  static const char* response_header() { return ""; }
  static const char* response_footer() { return ""; }
};

#if defined(TEENSYDUINO) || SERIAL_INTERFACES_COUNT - 0 > 3
class Serial3Adapter {
public:
  static void begin() { Serial3.begin(115200); }
  static bool Connected() { return true; }
  static bool AlwaysConnected() { return true; }
  static Stream& stream() { return Serial3; }
  static const char* response_header() { return "-+=BEGIN_OUTPUT=+-\n"; }
  static const char* response_footer() { return "-+=END_OUTPUT=+-\n"; }
};
#endif

#ifdef USB_CLASS_WEBUSB
class WebUSBSerialAdapter {
public:
  static void begin() { WebUSBSerial.begin(115200); }
  // static bool Connected() { return !!WebUSBSerial; }
  static bool Connected() { return true; }
  static bool AlwaysConnected() { return true; }
  static Stream& stream() { return WebUSBSerial; }
  static const char* response_header() { return "-+=BEGIN_OUTPUT=+-\n"; }
  static const char* response_footer() { return "-+=END_OUTPUT=+-\n"; }
};
#endif

#ifdef RFID_SERIAL
class RFIDParser : public Looper {
public:
  RFIDParser() : Looper() {}
  const char* name() override { return "Parser"; }
  void Setup() override {
    RFID_SERIAL.begin(9600);
  }

#define RFID_READCHAR() do {						\
  state_machine_.sleep_until_ = millis();				\
  while (!RFID_SERIAL.available()) {					\
    if (millis() - state_machine_.sleep_until_ > 200) goto retry;	\
    YIELD();								\
  }									\
  getc();								\
} while (0)

  int c, x;
  uint64_t code;

  void getc() {
    c = RFID_SERIAL.read();
    if (monitor.IsMonitoring(Monitoring::MonitorSerial)) {
      default_output->print("SER: ");
      default_output->println(c, HEX);
    }
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
    retry:
      RFID_READCHAR();
      if (c != 2) goto retry;
      code = 0;
      for (x = 0; x < 10; x++) {
	RFID_READCHAR();
	code <<= 4;
	if (c >= '0' && c <= '9') {
	  code |= c - '0';
	} else if (c >= 'A' && c <= 'F') {
	  code |= c - ('A' - 10);
	} else {
	  goto retry;
	}
      }
      RFID_READCHAR();
      x = code ^ (code >> 24);
      x ^= (x >> 8) ^ (x >> 16);
      x &= 0xff;
      if (c != x) goto retry;
      RFID_READCHAR();
      if (c == 3) {
	default_output->print("RFID: ");
	for (int i = 36; i >= 0; i-=4) {
	  default_output->print((int)((code >> i) & 0xf), HEX);
	}
	default_output->println("");
	for (size_t i = 0; i < NELEM(RFID_Commands); i++) {
	  if (code == RFID_Commands[i].id) {
	    CommandParser::DoParse(RFID_Commands[i].cmd, RFID_Commands[i].arg);
	  }
	}
      }
    }
    STATE_MACHINE_END();
  }

private:
  StateMachineState state_machine_;
};

StaticWrapper<RFIDParser> rfid_parser;
#endif

// Command-line parser. Easiest way to use it is to start the arduino
// serial monitor.
template<class SA> /* SA = Serial Adapter */
class Parser : Looper, StateMachine {
public:
  Parser() : Looper() {}
  const char* name() override { return "Parser"; }

  void Setup() override {
    SA::begin();
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!SA::Connected()) YIELD();
      if (!SA::AlwaysConnected()) {
        STDOUT << "Welcome to ProffieOS " << version << "\n";
        STDOUT << "For available serial commands, see:\n";
        STDOUT << "https://pod.hubbe.net/tools/serial-monitor-commands.html\n";	
      }

      while (SA::Connected()) {
        while (!SA::stream().available()) YIELD();
        int c = SA::stream().read();
        if (c < 0) { break; }
#if 0
        STDOUT.print("GOT:");
        STDOUT.println(c);
#endif
#if 0
        if (monitor.IsMonitoring(Monitoring::MonitorSerial) &&
            default_output != &SA::stream()) {
          default_output->print("SER: ");
          default_output->println(c, HEX);
        }
#endif
        if (c == '\n' || c == '\r') {
          if (cmd_) ParseLine();
          len_ = 0;
          space_ = 0;
          free(cmd_);
          cmd_ = nullptr;
          continue;
        }
        if (len_ + 1 >= space_) {
          int new_space = space_ * 3 / 2 + 8;
          char* tmp = (char*)realloc(cmd_, new_space);
          if (tmp) {
            space_ = new_space;
            cmd_ = tmp;
          } else {
            STDOUT.println("Line too long.");
            len_ = 0;
            space_ = 0;
            free(cmd_);
            cmd_ = nullptr;
            continue;
          }
        }
        cmd_[len_] = c;
        cmd_[len_ + 1] = 0;
        len_++;
      }
      len_ = 0;
      space_ = 0;
      free(cmd_);
      cmd_ = nullptr;
    }
    STATE_MACHINE_END();
  }

  void ParseLine() {
    if (len_ == 0) return;
    while (len_ > 0 && (cmd_[len_-1] == '\r' || cmd_[len_-1] == ' ')) {
      len_--;
      cmd_[len_] = 0;
    }
    if (cmd_[0] == '#') {
      Serial.println(cmd_);
      return;
    }
    stdout_output = &SA::stream();
    STDOUT.print(SA::response_header());
    char *cmd = cmd_;
    while (*cmd == ' ') cmd++;
    char *e = cmd;
    while (*e != ' ' && *e) e++;
    if (*e) {
      *e = 0;
      e++;  // e is now argument (if any)
    } else {
      e = nullptr;
    }
    if (monitor.IsMonitoring(Monitoring::MonitorSerial) &&
        default_output != &SA::stream()) {
      default_output->print("Received command: ");
      default_output->print(cmd);
      if (e) {
        default_output->print(" arg: ");
        default_output->print(e);
      }
      default_output->print(" HEX ");
      for (size_t i = 0; i < strlen(cmd); i++) {
        default_output->print(cmd[i], HEX);
        default_output->print(" ");
      }
      default_output->println("");
    }
    if (!CommandParser::DoParse(cmd, e)) {
      STDOUT.print("Whut? :");
      STDOUT.println(cmd);
    }
    STDOUT.print(SA::response_footer());
    stdout_output = default_output;
  }

private:
  int len_ = 0;
  char* cmd_ = nullptr;
  int space_ = 0;
};

StaticWrapper<Parser<SerialAdapter>> parser;

#ifdef ENABLE_SERIAL
StaticWrapper<Parser<Serial3Adapter>> serial_parser;
#define ENABLE_SERIAL_COMMANDS
#endif

#ifdef USB_CLASS_WEBUSB
StaticWrapper<Parser<WebUSBSerialAdapter>> webusb_parser;
#endif

#ifdef ENABLE_SERIAL_COMMANDS
class SerialCommands : public CommandParser {
 public:
  void HM1XCmd(const char* cmd) {
    STDOUT.print("Sending: ");
    STDOUT.println(cmd);
    STDOUT.print("Reply: ");
    Serial3.print(cmd);
    uint32_t last_char = millis();
    uint32_t timeout = 300;
    while (millis() - last_char < timeout) {
      if (Serial3.available()) {
        last_char = millis();
        timeout = 100;
        STDOUT.write(Serial3.read());
      }
    }
    STDOUT.println("");
  }
  bool Parse(const char* cmd, const char* e) override {
#if 0
    if (!strcmp(cmd, "hm1Xpin")) {
      // Doesn't work, pine is 4 chars, pinb is 6
      HM1XCmd("AT+AUTH1");
      HM1XCmd("AT+DUAL1");
      Serial3.write("AT+PINE");
      Serial3.println(e);
      Serial3.write("AT+PINB");
      Serial3.println(e);
      return true;
    }
    if (!strcmp(cmd, "hm1Xname")) {
      Serial3.write("AT+NAME");
      Serial3.println(e);
      Serial3.write("AT+NAMB");
      Serial3.println(e);
      return true;
    }
#endif
    if (cmd[0] == 'A' && cmd[1] == 'T') {
      HM1XCmd(cmd);
      return true;
    }
    if (!strcmp(cmd, "send") && e) {
      Serial3.println(e);
      STDOUT.print("Wrote: ");
      STDOUT.println(e);
      return true;
    }
#ifdef BLE_PASSWORD
    if (!strcmp(cmd, "get_ble_config")) {
      PrintQuotedValue("password", BLE_PASSWORD);
#ifndef BLE_NAME
#define BLE_NAME "ProffieOS"
#endif
      PrintQuotedValue("name", BLE_NAME);
#ifdef BLE_SHORTNAME
      PrintQuotedValue("shortname", BLE_SHORTNAME);
#else
      if (sizeof(BLE_NAME) - sizeof("") <= 8) {
        PrintQuotedValue("shortname", BLE_NAME);
      } else {
        PrintQuotedValue("shortname", "Saber");
      }
#endif
      return true;
    }
#endif
    return false;
  }
};

StaticWrapper<SerialCommands> serial_commands;

#endif


#endif  // COMMON_SERIAL_H
