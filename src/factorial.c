/* Built-in syscalls */
void print_int(int integer);
void print_string(char *string);
int read_int(void);
void read_string(char *buffer, int length);
void exit(void);

void optimization_demo(int n);
void auto_casting_demo(void);

char *strcpy(char *dest, char *src);
int factorial(int n);

int main(void) {
  char prompt[80];
  int n, f;

  strcpy(prompt, "Factorial of ");
  print_string(prompt);

  n = read_int();
  strcpy(prompt, " equals ");
  print_string(prompt);

  f = factorial(n);

  print_int(f);  

  strcpy(prompt, "\n");
  print_string(prompt);

  auto_casting_demo();

  return 0;
}

char *strcpy(char *dest, char *src) {
  char *initial_dest;
  initial_dest = dest;
  do {
    *dest++ = *src;
  } while(*src++ != 0);
  return initial_dest;
}

int factorial(int n) {
  if(n <= 1)
    return 1;
  else
    return n*factorial(n-1);
}

void optimization_demo(int n)
{
  int m;
  m = 10+20-40/50;
  30+50+60;  
}

void auto_casting_demo(void)
{
  int p;
  short m;
  char k;
  p = 10;
  m = p;
  k = m;

  print_string("\n Int value stored:");
  print_int(p);

  print_string("\n short value stored:");
  print_int(m);

  print_string("\n Char value stored: ");
  print_int(k);
}
