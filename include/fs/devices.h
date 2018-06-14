#ifndef __FS__DEVICES_H__
#define __FS__DEVICES_H__

// null device

int dev_null_getch();

int dev_null_putch(int ch);

// zero device

int dev_zero_getch();

// random device

int dev_random_getch();

// stdin

int dev_stdin_getch();

// stdout

int dev_stdout_putch(int ch);

#endif 
