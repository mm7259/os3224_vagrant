#include "types.h"
#include "stat.h"
#include "user.h"


char buf[1024];
int count, duplicate, insensitive, capacity; 
char *dynamicbuf;

int 
checkcaseinsensitive(char *buf, int pos1, int pos2)
{
  int char1, char2;
  char1 = (int)(*(buf + pos1));
  char2 = (int)(*(buf + pos2));
  if(char1 < char2){
    char1 += 32; 
  } 
  else {
    char2 += 32; 
  }
  
  if(char1 == char2){
    return 1; 
  }
  return 0; 
}


int
checkmatch(char *buf, int startpos1, int endpos1, int startpos2, int endpos2)
{ 
  int lenword1, lenword2;  
  int i;
  lenword1 = endpos1 - startpos1 + 1; 
  lenword2 = endpos2 - startpos2 + 1; 
  if(lenword1 == lenword2)
  {
    for(i = 0; i < lenword1; i++) 
      if( *(buf + startpos1 + i) != *(buf + startpos2 + i)){
        if(insensitive == 1)
          return checkcaseinsensitive(buf, startpos1 + i, startpos2 + i);
        return 0;  
      }
  }
  else
  { 
    return 0;
  }
  return 1; 
}


void
printsegment(char *buf, int start, int end)
{ 
  char *temp; 
  temp = buf+start;
  while (*temp != '\n')
     printf(1, "%c", *temp++);  
   printf(1, "\n"); 
}

void
reallocate(int newcapacity)
{
  printf(1, "Reallocating\n"); 
  printf(1, dynamicbuf);  
  printf(1, "%c",'\n'); 
  char *newbuf; 
  newbuf = (char*)malloc(newcapacity * sizeof(char)); 
  strcpy(newbuf, dynamicbuf);  
  free(dynamicbuf); 
  dynamicbuf = newbuf; 
  capacity = newcapacity; 
}

void
uniq(int fd, char *name)
{
  int i, n, c, previsdup;
  int startpos1, endpos1, startpos2, endpos2, len;
  startpos1 = endpos1 = startpos2 = endpos2 = len = c = previsdup = 0; 

  while((n = read(fd, dynamicbuf, sizeof(dynamicbuf))) > 0){
    while(n> 0 && n==capacity && dynamicbuf[n-1]!='\n'){
      reallocate(capacity*2); 
      n = read(fd, dynamicbuf, sizeof(dynamicbuf)); 
    }

    startpos1 = 0;
    for(i=0; i<n; i++){
      if(dynamicbuf[i] == '\n')
      {
        if(endpos1 == 0)
        {
          endpos1 = i-1; 
          startpos2 = i + 1;
          
          //print line1
          if(count != 1 && duplicate != 1)
            printsegment(dynamicbuf, startpos1, endpos1);
          else if(count == 1){
            c = 1;
            if((i+1)==n){
              printf(1, "%d ", c); 
              printsegment(dynamicbuf, startpos1, endpos1);
              c = 1;
            }  
          }
        }
        else if(endpos2 == 0)
        {
          endpos2 = i -1;
          if (checkmatch(dynamicbuf, startpos1, endpos1, startpos2, endpos2) == 1)
          {
             startpos2 = endpos2 + 2; 
             endpos2 = 0;

             //support for 'c'
             if(count == 1){
               c += 1; 
               if((i+1) == n){
                 printf(1, "%d ", c); 
                 printsegment(dynamicbuf, startpos1, endpos1);
                 c = 1;
               }  
             }
             
             //support for 'd'
             if(duplicate == 1){
               previsdup = 1;
               if((i+1) == n){
                 printsegment(dynamicbuf, startpos1, endpos1);
               }
             } 
             
          }
          else 
          {  
             //support for 'c'
             if(count == 1){
               if((i+1) == n){
                 printf(1, "%d ", c); 
                 printsegment(dynamicbuf, startpos2, endpos2);
                 c = 1; 
               }
               printf(1, "%d ", c); 
               printsegment(dynamicbuf, startpos1, endpos1);
               c = 1;  
             } 
             else if(duplicate == 1){
               if(previsdup == 1){
                printsegment(dynamicbuf, startpos1, endpos1);
                previsdup = 0;  
               }
             }
             else { 
               len = endpos2 - startpos2;
               printsegment(dynamicbuf, startpos2, endpos2); 
             }

             startpos1 = startpos2; 
             endpos1 = endpos2; 
             startpos2 = endpos2 + 2; 
             endpos2 = 0; 
          }
        }
      } 
    }
  }
  if(n < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  count = duplicate = insensitive = 0; 
  
   
  capacity = 512; 

  dynamicbuf = (char*)malloc(capacity* sizeof(char)); 

  if(argc <= 1){
    uniq(0, "");
    exit();
  }

  for(i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      switch(argv[i][1]){
        case 'c':
          count = 1; 
          break; 
        case 'd':
          duplicate = 1; 
          break;  
        case 'i':
          insensitive = 1; 
          break;  
      }
    }
    else {
      if((fd = open(argv[i], 0)) < 0){
        printf(1, "uniq: cannot open %s\n", argv[i]);
        exit();
      }
      uniq(fd, argv[i]);
      close(fd);
    } 
  }
  exit();
}
