void print_int(int i);
void print_string(char* p);
int tmp(int a, int b, int c);
void tmp2(int a, int b, int c);
int dummy_return(int a);
int main(void)
{
  int i, j, *p;
  short s;
  unsigned m;
  char* ptr;

  int v;
  i = 10;
  j = 12;
  v = 10 + 12;
  
  print_int(22+33-44);
  ptr = "Hello World";
  
  p = &i;
  j = i*i + i;
  j = j+*p * i + i;

  /* i = *ptr + 1; */
  i++;
  i = 10;
  j = 5;
  m = i++;
  v = 10;
  i = 10;  j = 2;
  i = (i / j);
  m = (m*=v);
  m = --v;
  print_int(i);

  m = 10;

  i = tmp(1,2,3);

  print_int(m);
  m = 20;

  /* print_int(v); */
  if(v == 0)
    print_string("Found 0\n");
  else
    print_string("Found 1\n");

  print_string("for loop\n");
  for (v = 0; v< 3;v++)
    {
      print_string("value found:");
      print_int(v);
      print_string("\n");
    }

  print_string("do while loop\n");
  while(v>0) {
    print_string("value found:");
    print_int(v);
    print_string("\n");
    v--;
  }

  v = 0;
 Label:  v++;
  print_int(v);
  print_string("\n");
  if(v <2)
    goto Label;

  print_string("do while loop\n");
  v = 0;
  do{
    print_string("value found:");
    print_int(v);
    print_string("\n");
    v++;
  }  while(v < 3);


  print_int(j);
  print_string(ptr);
  print_int(i);
  print_string(" ");
  print_string("\n values are ");
  p = &i;
  print_int(*p);
  print_string(" ");
  print_int(m);
  {
    int* k;
    int a;
    int b[3];
    char ch;
    short x;
    char ch_arr[5];
    short int c[1];

    a =  tmp(1,2);
    a = 1;
    k = &a;
    a = *k;/*+a;*/
    print_string("\n Compound statements values:\n");
    print_int(*k);
    print_string("\n");
    print_int(a+*k+3);
    print_string("\n");

  /*   b[0]+c[0]; */
  /*   k + ch + ch_arr[0]; */
  /*   { */
  /*     int a; */
  /*     b[0] +a; */
  /*   } */
  /*   { */
  /*     int d; */
  /*     b[0] + d; */
  /*   } */
  /*   c[0] + a; */
  }

  print_int(dummy_return(10));
  return 0;
}

int dummy_return(int a)
{
  return a;
}

/*How is it picking this as declarator*/
void tmp2(int a, int b, int c){
  print_string("tmp2\n");
  print_int(a+b+c);
}

int tmp(int a, int b, int c){
  int d;
  char ch;
  short e;
  tmp2(4,5,6);
  print_string("tmp\n");
  print_int(a+b+c);
  return b;
}
