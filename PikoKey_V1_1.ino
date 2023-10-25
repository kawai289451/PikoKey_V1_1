//---------------------------
//   Piko Key v1.1
//   Sample Program
//   ver 0.3
//   2023/4/29
//   by Synth Senpai
//---------------------------

#include <MIDI.h>
USING_NAMESPACE_MIDI

//*********************************
// Variable

const byte OCTKEY = 12;
const byte DEFOFFSET = 48;
const byte MINOFFSET = 36;
const byte MAXOFFSET = 60;

// Pins
const byte LED_DN = 2;
const byte LED_UP = 3;
const byte LED_1  = 13;
const byte SW_DN  = 4;
const byte SW_UP  = 5;
const byte SW15   = 6;
const byte SW14   = 7;
const byte SW13   = 8;
const byte SW12   = 9;
const byte SW11   = 10;
const byte SW10   = 11;
const byte SW09   = 12;
const byte SW08   = 21;
const byte SW07   = 20;
const byte SW06   = 19;
const byte SW05   = 18;
const byte SW04   = 17;
const byte SW03   = 16;
const byte SW02   = 15;
const byte SW01   = 14;
byte sw_map[] = { SW01, SW02, SW03, SW04, SW05, SW06, SW07, SW08, SW09, SW10, SW11, SW12, SW13, SW14};
const byte MAXKEY = 14;

// midi
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

byte note_offset = DEFOFFSET;
byte channel = 1;
int pgno;
int vol = 63;

bool sw_state_old[15];
bool up_sw_state_old;
bool dn_sw_state_old;
bool fn_sw_state_old;

//********************************************
// Accessor

void set_note_offset(int offset) {
  note_offset = offset;
}

void set_channel(int ch) {
  channel = ch;
}

void set_pgno(int no) {
  pgno = no;
}

void set_vol(int v) {
  vol = v;
}

void init_var() {
  note_offset = DEFOFFSET;
  channel = 1;
  pgno = 0;
  vol = 63;

  for (byte i = 0; i < MAXKEY; i++) {
    sw_state_old[i] = true;
  }
  up_sw_state_old = true;
  dn_sw_state_old = true;
  fn_sw_state_old = true;
}

void set_sw_state_old(int i, bool state) {
  sw_state_old[i] = state;
}

void set_up_sw_state_old(bool state) {
  up_sw_state_old = state;
}

void set_dn_sw_state_old(bool state) {
  dn_sw_state_old = state;
}

void set_fn_sw_state_old(bool state) {
  fn_sw_state_old = state;
}

//********************************************
// Arduino

void init_pin_mode() {
  pinMode(LED_DN, OUTPUT);
  pinMode(LED_UP, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(SW_UP, INPUT_PULLUP);
  pinMode(SW_DN, INPUT_PULLUP);
  pinMode(SW15, INPUT_PULLUP);

  for (byte i = 0; i < MAXKEY; i++) {
    pinMode(sw_map[i], INPUT_PULLUP);
  }
}

bool read_up_sw_state() {
  return digitalRead(SW_UP);
}

bool read_dn_sw_state() {
  return digitalRead(SW_DN);
}

bool read_sw_state(int i) {
  return digitalRead(sw_map[i]);
}

bool read_fn_sw_state() {
  return digitalRead(SW15);
}

void write_dn(bool state) {
  digitalWrite(LED_DN, state ? HIGH : LOW);
}

void write_up(bool state) {
  digitalWrite(LED_UP, state ? HIGH : LOW);
}

void write_1(bool state) {
  digitalWrite(LED_1, state ? HIGH : LOW);
}

void write_note_on() {
  digitalWrite(LED_1, HIGH);
}

void write_note_off() {
  digitalWrite(LED_1, LOW);
}

//********************************************
// MIDI

void begin_midi() {
  MIDI.begin();
}

void send_note_on(byte note, byte channel) {
  byte velocity = 127;
  MIDI.sendNoteOn(note, velocity, channel);
}

void send_note_off(byte note, byte channel) {
  byte velocity = 0;
  MIDI.sendNoteOn(note, velocity, channel);
}

void send_program_change(int pgno, byte channel) {
  MIDI.sendProgramChange(pgno, channel);
}

//*********************************************
// setup

void setup() {
  init_pin_mode();
  init_var();

  // midi
  begin_midi();

  int delay_time = 200
  start_up_animation(delay_time);

  // for debug
  // Serial.begin(115200);
  // Serial.println("setup done");

}

//*********************************************
// Main

void loop() {
  // Key scan
  scan_key();

  // Function (Oct up/down, MIDI ch, Program Change)
  scan_oct_up();
  scan_oct_down();
  scan_function();
}

//********************************************
// SW Scan

// Key scan
void scan_key() {
  for (byte i = 0; i < MAXKEY; i++) {
    bool sw_state = read_sw_state(i);
    if (is_sw_pressed(sw_state_old[i], sw_state)) {
      press_key(i);
    }

    if (is_sw_released(sw_state_old[i], sw_state)) {
      release_key(i);
    }
    
    set_sw_state_old(i, sw_state);
  }
}

// Oct up/down
void scan_oct_up() {
  bool up_sw_state = read_up_sw_state();
  if (!is_sw_pressed(up_sw_state_old, up_sw_state)) return;

  oct_up();

  set_up_sw_state_old(up_sw_state);
}

void scan_oct_down() {
  bool dn_sw_state = read_dn_sw_state();
  if (is_sw_pressed(dn_sw_state_old, dn_sw_state)) return;

  oct_down();

  set_dn_sw_state_old(dn_sw_state);
}

// Function
void scan_function() {
  bool fn_sw_state = read_fn_sw_state();
  if (!is_sw_pressed(fn_sw_state_old, fn_sw_state)) return;

  byte funcno = read_funcno();
  press_fn_sw(funcno);
  
  set_fn_sw_state_old(fn_sw_state);
}

byte read_funcno() {
  for ( byte i = 0; i < MAXKEY; i++ ) {
    bool sw_state = read_sw_state(i);
    if (!sw_state) {
      return i;
    }
  }
  return -1;
}

bool is_sw_pressed(bool state_old, bool state_new) {
  return state_old && !state_new;
}

bool is_sw_released(bool state_old, bool state_new) {
  return !state_old && state_new;
}

//********************************************
// UseCase

void start_up_animation(int delay_time) {
  write_dn(true);
  delay(delay_time);
  write_up(true);
  delay(delay_time);
  write_1(true);
  delay(delay_time);
  write_dn(false);
  delay(delay_time);
  write_up(false);
  delay(delay_time);
  write_1(false);
}

void press_key(byte keyno) {
  write_note_on();

  byte note = keyno + note_offset;
  send_note_on(note, channel);
}

void release_key(byte keyno) {
  write_note_off();

  byte note = keyno + note_offset;
  send_note_off(note, channel);
}

void oct_up() {
  byte offset = min(note_offset + OCTKEY, MAXOFFSET);
  change_note_offset(offset);
}

void oct_down() {
  byte offset = max(note_offset - OCTKEY, MINOFFSET);
  change_note_offset(offset);
}

void change_note_offset(byte note_offset) {
  set_note_offset(note_offset);

  bool is_write_dn = note_offset == MINOFFSET;
  write_dn(is_write_dn);

  bool is_write_up = note_offset == MAXOFFSET;
  write_up(is_write_up);
}

void change_pgno(int pgno, byte channel) {
  set_pgno(pgno);
  send_program_change(pgno, channel);
}

void decrement_channel() {
  int channel_new = max(channel - 1, 1);
  set_channel(channel_new);
}

void increment_channel() {
  int channel_new = min(channel + 1, 16);
  set_channel(channel_new);
}

void decrement_vol() {
  int vol_new = max(vol - 10, 0);
  set_vol(vol_new);
}

void increment_vol() {
  int vol_new = min(vol + 10, 127);
  set_vol(vol_new);
}

void decrement_pgno() {
  int pgno_new = max(pgno - 1, 0);
  change_pgno(pgno_new, channel);
}

void increment_pgno() {
  int pgno_new = min(pgno + 1, 127);
  change_pgno(pgno_new, channel);
}

void press_fn_sw(byte funcno) {
  switch (funcno) {
    case 0: change_pgno(0, channel); break;
    case 1: change_pgno(20, channel); break;
    case 2: set_channel(1); break;
    case 3: change_pgno(40, channel); break;
    case 4: set_channel(10); break;
    case 5: change_pgno(60, channel); break;
    case 6: change_pgno(80, channel); break;
    case 7: decrement_channel(); break;
    case 8: decrement_vol(); break;
    case 9: break;
    case 10: increment_vol(); break;
    case 11: increment_channel(); break;
    case 12: decrement_pgno(); break;
    case 13: increment_pgno(); break;
    default: break;
  }
}