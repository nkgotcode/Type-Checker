// buried type errors

int a = 5;

if(a < 6){
    int b = 0;
    if(b == 0)
        a = "no";
    else
        b = 0;
}
else{
    int b = 1;
    int c = "yes";
}

while(a > 0){
  int c = a;
  a = c + 1;
  d = d - 1;
}
