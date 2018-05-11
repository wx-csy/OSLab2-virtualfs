#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void (*_putc)(char);

/* printf behavior modifier */
#define LEFT_JUSTIFIED   0x1
#define ALWAYS_SIGNED    0x2
#define SPACE_PREPENDED  0x4
#define ALTERNATIVE_FORM 0x8
#define ZERO_PADDING     0x10

typedef struct conv_spec {
  int behavior;
  int width, precision;
} conv_spec;

static inline int _get_behavior_modifier
    (const char *restrict *pfmt) {
  int behavior = 0;
  do {
    switch (**pfmt) {
      case '-': behavior |= LEFT_JUSTIFIED;     break;
      case '+': behavior |= ALWAYS_SIGNED;      break;
      case ' ': behavior |= SPACE_PREPENDED;    break;
      case '#': behavior |= ALTERNATIVE_FORM;   break;
      case '0': behavior |= ZERO_PADDING;       break;
      default:  return behavior;
    }
  } while ((*pfmt)++);
  return 0;
}

static inline int _get_integer(const char *restrict *pfmt) {
  int result = 0;
  while (isdigit(**pfmt)) {
    result = result * 10 + (**pfmt - '0');
    (*pfmt)++;
  }
  return result;
}

static inline void _print_nch(int ch, int n) {
  for (int i = 0; i < n; i++) _putc(ch);
}

static inline int _printf_c(const conv_spec *spec, int ch) {
  if (spec->width) {
    if (spec->behavior & LEFT_JUSTIFIED) {
      _putc(ch);
      _print_nch(' ', spec->width - 1);
    } else {
      _print_nch(' ', spec->width - 1);
      _putc(ch);
    }
    return spec->width;
  } else {
    _putc(ch);
    return 1;
  }
}

static inline int _printf_s(const conv_spec *spec, const char* str) {
  size_t len = strlen(str);
  if (spec->precision != -1 && len > spec->precision) 
     len = spec->precision;
  int nspace = spec->width - len;
  if (nspace < 0) nspace = 0;
  if (spec->behavior && LEFT_JUSTIFIED) {
    for (int i = 0; i < len; i++) _putc(str[i]);
    _print_nch(' ', nspace);
  } else {
    _print_nch(' ', nspace);
    for (int i = 0; i < len; i++) _putc(str[i]);
  }
  return nspace + len;
}

static inline int _printf_d(const conv_spec *spec, int sign,
    unsigned absv) {
  int precision = spec->precision;
  if (precision == -1) precision = 1; // default precision
  char buf[64];
  int bufc = 0;
  
  while (absv) {
    buf[bufc++] = '0' + absv % 10;
    absv /= 10;
  }
  if (bufc == 0 && precision != 0)
    buf[bufc++] = '0';
  
  int signlen = 1;
  char signchar;
  if (sign == -1) {
    signlen = 0;
  } else if (sign == 1) {
    signchar = '-';
  } else if (spec->behavior & ALWAYS_SIGNED) {
    signchar = '+';
  } else if (spec->behavior & SPACE_PREPENDED) {
    signchar = ' ';
  } else {
    signlen = 0;
  }
  
  int width = spec->width;

  if (spec->behavior & ZERO_PADDING) {
    if (precision + signlen < width) precision = width;
    width = 0;
  }

  int numzero = precision - bufc - signlen;
  if (numzero < 0) numzero = 0;

  int numspace = width - numzero - bufc - signlen;
  if (numspace < 0) numspace = 0;

  if (spec->behavior & LEFT_JUSTIFIED) {
    if (signlen) _putc(signchar);
    _print_nch('0', numzero);
    for (int i = bufc - 1; i >= 0; i--)
      _putc(buf[i]);
    _print_nch(' ', numspace);
  } else {
    _print_nch(' ', numspace);
    if (signlen) _putc(signchar);
    _print_nch('0', numzero);
    for (int i = bufc - 1; i >= 0; i--)
      _putc(buf[i]);
  }

  return numspace + signlen + numzero + bufc;
}

static inline int _printf_o(const conv_spec *spec, unsigned value) {
  int precision = spec->precision;
  if (precision == -1) precision = 1; // default precision
  char buf[64];
  int bufc = 0;
  
  while (value) {
    int cur = value & 0x7;
    value >>= 3;
    buf[bufc++] = '0' + cur;
  }

  if ((bufc == 0 && precision != 0) || 
      (spec->behavior & ALTERNATIVE_FORM))
    buf[bufc++] = '0';

  int width = spec->width;

  if (spec->behavior & ZERO_PADDING) {
    if (precision < width) precision = width;
    width = 0;
  }

  int numzero = precision - bufc;
  if (numzero < 0) numzero = 0;

  int numspace = width - numzero - bufc;
  if (numspace < 0) numspace = 0;

  if (spec->behavior & LEFT_JUSTIFIED) {
    _print_nch('0', numzero);
    for (int i = bufc - 1; i >= 0; i--)
      _putc(buf[i]);
    _print_nch(' ', numspace);
  } else {
    _print_nch(' ', numspace);
    _print_nch('0', numzero);
    for (int i = bufc - 1; i >= 0; i--)
      _putc(buf[i]);
  }

  return numspace + numzero + bufc;
}

static inline int _printf_x(const conv_spec *spec, unsigned value,
    int capital) {
  int precision = spec->precision;
  if (precision == -1) precision = 1; // default precision
  char buf[64];
  int bufc = 0;
  
  while (value) {
    int cur = value & 0xf;
    value >>= 4;
    if (cur < 10) buf[bufc++] = '0' + cur;
    else buf[bufc++] = (capital ? 'A' : 'a') + (cur - 10);
  }

  int prefix = 0;  
  if (bufc == 0) {  // value = 0
    if (precision != 0)  buf[bufc++] = '0';
  } else if (spec->behavior & ALTERNATIVE_FORM) {
      prefix = 2;
  }
  
  int width = spec->width;
  if (spec->behavior & ZERO_PADDING) {
    if (precision < width - prefix) precision = width - prefix;
    width = 0;
  }

  int numzero = precision - bufc;
  if (numzero < 0) numzero = 0;

  int numspace = width - numzero - bufc - prefix;
  if (numspace < 0) numspace = 0;

  if (spec->behavior & LEFT_JUSTIFIED) {
    if (prefix) {
      _putc('0');
      _putc(capital ? 'X' : 'x');
    }
    _print_nch('0', numzero);
    for (int i = bufc - 1; i >= 0; i--)
      _putc(buf[i]);
    _print_nch(' ', numspace);
  } else {
    _print_nch(' ', numspace);
    if (prefix) {
      _putc('0');
      _putc(capital ? 'X' : 'x');
    }
    _print_nch('0', numzero);
    for (int i = bufc - 1; i >= 0; i--)
      _putc(buf[i]);
  }

  return numspace + numzero + bufc + prefix;
}

int _vprintf_raw(void (*putc)(char), const char *restrict format,
    va_list args) {
  int written = 0;
  int p_int;
  unsigned p_uint;
  conv_spec spec;
  
  void (*last_putc)(char) = _putc;
  _putc = putc;

  do {
    switch (*format) {
      case 0: goto ret;
      case '%': // parse conversion specification
        format++;

        // behavior modifier
        spec.behavior = _get_behavior_modifier(&format);

        // minimum field width
        if (*format == '*') {
          format++;
          spec.width = va_arg(args, int);
          if (spec.width < 0) {
            spec.width = -spec.width;
            spec.behavior |= LEFT_JUSTIFIED;
          }
        } else {
          spec.width = _get_integer(&format);
        }
        
        // precision
        if (*format == '.') {
          format++;
          if (*format == '-') {
            format++;
            _get_integer(&format);
            spec.precision = -1;
          } else if (*format == '*') {
            format++;
            spec.precision = va_arg(args, int);
            if (spec.precision < 0) {
              spec.precision = -1;
            }
          } else {
            spec.precision = _get_integer(&format);
          }
        } else {
          spec.precision = -1; // default precision
        } 

        // length modifier is not implemented yet
        // conversion format specifier
        switch (*format) {
          /* conversion specifier */
          case 'c':
            /* `char' is promoted to `int'. If use char here,
             * compiler will complain, and program might crash.
             */
            written += _printf_c(&spec, va_arg(args, int));
            break;
          case 's':
            written += _printf_s(&spec, va_arg(args, const char*));
            break;
          case 'd':
          case 'i':
            p_int = va_arg(args, int);
            if (p_int < 0) p_uint = -(unsigned)p_int;
            else p_uint = p_int;
            written += _printf_d(&spec, p_int < 0, p_uint);
            break;
          case 'u':
            written += _printf_d(&spec, -1, va_arg(args, unsigned));
            break;
          case 'o':
            written += _printf_o(&spec, va_arg(args, unsigned));
            break;
          case 'x':
            written += _printf_x(&spec, va_arg(args, unsigned), 0);
            break;
          case 'X':
            written += _printf_x(&spec, va_arg(args, unsigned), 1);
            break;
          case 'p': // this is a temporary implementation
            written += _printf_x(&spec, va_arg(args, unsigned), 1);
            break;
          case '%':
            _putc('%');
            written++;
            break;
          case 'n':
            *va_arg(args, int*) = written;
            break;
          case 0:
            goto ret;
        }
      break;
      default: 
        _putc(*format);
        written++;
    }
  } while (format++);
ret:
  _putc = last_putc;
  return written;
}

