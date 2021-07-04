/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
// -------------------- NOTES ----------------------
// WHATS A STRUCT ?
// HOW DOES ADDRESSING WORK ?

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

/** Prints error message and exits the program */
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawBashMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

/**
 * Enables Terminal Raw Mode
 */
void enableRawBashMode() {
  // Read parameters to terminal
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawBashMode); // Callback firing when on program exit

  struct termios raw = orig_termios;

  // ~ - NOT OPERATOR
  // IXON - disable software flow control (CTRL-Q and CTRL-S)
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  // OPOST - disable post processing of output (newline etc.)
  raw.c_oflag &= ~(OPOST);

  raw.c_cflag |= (CS8);

  // ECHO - disable echo
  // ICANON - disable canonical mode
  // ISIG - disable CTRL-C and CTRL-Z Signals (SIGINT SIGTSTP)
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

  // Sets minimum numver of bytes input needs before read() can return.
  raw.c_cc[VMIN] = 0;

  // Sets maxiumum amout of time to wait before read() returns.
  raw.c_cc[VTIME] = 1;

  // Apply params to terminal
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/*** init ***/

int main() {
  enableRawBashMode();

  char c = '\0';
  while (1) {
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
      die("read");
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c') \r\n", c, c);
    }
    if (c == 'q')
      break;
  };
  // reading 1 byte from standard input to c
  // this loop will continue reading bytes untill it hits EOF

  return 0;
}