int read_int(void);
void read_string(char *s, int n);
void print_int(int i);
void print_string(char *s);
int atoi(char *s);
void strcpy(char *dest, char *src);
void print_int_hex(int i);

int fib(int n) {
  int val;
  val = fib(n - 1) + fib(n - 2);
  return (n < 2 ? n : val);
}

int main(int argc, char *argv[]) {
  print_int(fib(atoi(argv[1])));
  print_string("\\n");
  goto label;
 label:print_string("Reach here");
  return 0;
}
