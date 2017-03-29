/* void tmp(void); */
void print_int(int);
void print_string(char*);
int main(void)
{
  /* int a; */
  /* char ch; */
  /* char* str; */
  int arr[3];
  int *ptr,**p;
  int tmp;
  short sh;
  int arr[3+3];

  char ch_arr[3];
  char *ptr1, *ptr2;
  char ch;
  str = "Temp String";
  ch = *str++;

  ptr = arr;
  p = ptr;
  ptr = &tmp;
  ptr = & tmp;
  p = &ptr;
  **p = 3;
  ptr = p-1;
  *(ptr + 1) = 1;
  *(ptr+1) = 2;
  tmp = arr[1] + 2;
  arr[2] = 3;
  tmp = tmp+2;
  arr[2] = 3;

  arr[0] = 1;
  sh = 255;
  ptr1 = ch_arr;
  ch = sh;
  ch_arr[2] = 255 + 10 + 11;
  print_int(ch_arr[2]);
  print_int(ch);
  ptr1 = ch_arr;
  ptr2 = ptr2;
  *ptr++ = *ptr2;

 label:
  ptr = ptr+1;
  a = *(arr + 2);/*ptr*/
  a = *ptr++;
  print_int(arr[2]);
  print_int(tmp);
  print_int(*ptr++);
  print_string(" ");
  print_int(*ptr--);
  print_string(" ");
  print_int(*ptr);
  print_int(**p);
  print_string("\n");
  print_int(*(ptr+1));
  print_string("\n");
  print_int(*(p));

  /* a = tmp(); */

  
  tmp();
  {
    int b;
    int c;
    b = 1;

  }
}

void tmp(void)
{
  char ch;
  print_string("tmp called");
}
