#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUFFER 1024

//definicja plików
#define SYSFS_FILE_WE1 "/sys/kernel/sykt/raba1"
#define SYSFS_FILE_WE2 "/sys/kernel/sykt/raba2"
#define SYSFS_FILE_RES "/sys/kernel/sykt/rabw"
#define SYSFS_FILE_STATUS "/sys/kernel/sykt/rabb"
#define SYSFS_FILE_ONES "/sys/kernel/sykt/rabl"


// oby nie było błędów z kompilacją i z kolejnoscią, definiuje metody na początku
unsigned int read_from_file(char *);  
void write_to_file(char *, unsigned int);
struct multiplication_result multiply(unsigned int, unsigned int);
int test_module();




int main(void){
int test = test_module();


/*test polega na tym że  daje testowe znaczenia
i już  mam wynik mnożenia
gdyż wynik oczekiwany różni się od output-u dodaje 1
Te
*/
if(test > 0){
printf(" ERROR at %d values\n",test);
}
else{
printf(" OK\n");
}
return 0;
}


unsigned int read_from_file(char *filePath){
char buffer[MAX_BUFFER];

int file=open(filePath, O_RDONLY);
if(file<0){
printf("Open %s - error number %d\n", filePath, errno);
exit(1);
}
int n=read(file, buffer, MAX_BUFFER);
if(n>0){   
        buffer[n]='\0';
        printf("%s", buffer); 
        close(file);
        return strtoul(buffer, NULL, 16);  // 16 znaczy HEX
    }else{
        printf("Open %s - error %d\n", filePath, errno); 
        close(file);
        return 404;
    }
}




void write_to_file(char *filePath, unsigned int input){
	char buffer[MAX_BUFFER];
	int fd_in=open(filePath, O_RDWR); 
	if(fd_in < 0){  
		 printf("Open %s - error number %d\n", filePath, errno);
		 exit(2);
	}
	snprintf(buffer, MAX_BUFFER, "%x", input);
	int n=write(fd_in, buffer, strlen(buffer));
    if(n!=strlen(buffer)){
        printf("Open %s - error number %d\n", filePath, errno);
        close(fd_in);
        exit(3);
    }
	close(fd_in);
}

/*operacja mnożenia -  to wczytywanie danych od użytkownika
i wczytywanie outputów*/

struct multiplication_result {
  unsigned int w;
  unsigned int l;
  unsigned int b;
};

struct multiplication_result multiply(unsigned int arg1, unsigned int arg2){

write_to_file(SYSFS_FILE_WE1,arg1);
write_to_file(SYSFS_FILE_WE2,arg2);
unsigned int read = 0;
unsigned int readw = 0;
unsigned int readl = 0;
unsigned int readb = 0;
/*
    read = read_from_file(SYSFS_FILE_STATUS);
    read = read_from_file(SYSFS_FILE_STATUS);

    readw = read_from_file(SYSFS_FILE_RES);
    readl = read_from_file(SYSFS_FILE_ONES);
    readb = read_from_file(SYSFS_FILE_STATUS);
while (read != 3 && readw == 0 && readl == 0){

    read = read_from_file(SYSFS_FILE_STATUS);
    readw = read_from_file(SYSFS_FILE_RES);
    readl = read_from_file(SYSFS_FILE_ONES);
    readb = read_from_file(SYSFS_FILE_STATUS);
}*/

 while (1) {
        unsigned int read = read_from_file(SYSFS_FILE_STATUS);
        unsigned int readw = read_from_file(SYSFS_FILE_RES);
        unsigned int  readl = read_from_file(SYSFS_FILE_ONES);
        unsigned int  readb = read_from_file(SYSFS_FILE_STATUS);
        if (read == 3 && readw != 0 && readl != 0) break;
       /* readw = read_from_file(SYSFS_FILE_RES);
        readl = read_from_file(SYSFS_FILE_ONES);
        readb = read_from_file(SYSFS_FILE_STATUS);*/
    }
     readw = read_from_file(SYSFS_FILE_RES);
        readl = read_from_file(SYSFS_FILE_ONES);
        readb = read_from_file(SYSFS_FILE_STATUS);


//} 

struct multiplication_result result;
  result.w = readw;
  result.l = readl;
  result.b = readb;

//printf("A1=0x%x, A2=0x%x, W=0x%x, L=0x%x, B =0x%x", arg1, arg2, readw, readl,readb);
//printf("A1=0x%x, A2=0x%x", arg1, arg2);
return result;
}

int random_in_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

int count_ones(unsigned int num) {
    int count = 0;
    char binary[33]; 
    sprintf(binary, "%032lX", num);
   
    for (int i = 0; i < 32; i++) {
        if (binary[i] == '1') {
            count++;
        }
    }
    return count;
}

int test_module(){

    typedef struct {
    unsigned int a1;
    unsigned int a2;
    unsigned int w;
    int num_ones;
} MyStruct;



 MyStruct values[500];

 for (int i = 0; i < 500; i++) {
        values[i].a1 = random_in_range(0, 1048575); // 20 
        values[i].a2 = random_in_range(0, 1048575); // 20 
        values[i].w = values[i].a1 * values[i].a2;
        values[i].num_ones = count_ones(values[i].w);}

/*unsigned int args1[3] = { 3, 0xc, 8};
unsigned int args2[3] = { 4, 3, 3};
unsigned int results[3] = { 0xc,24,18 };
unsigned int ones[3] ={3,2,2};*/
int k=0;
for(int i=0; i<500; i++){
struct multiplication_result result = multiply(values[i].a1,values[i].a2);
if( result.w != values[i].w && result.l != values[i].num_ones)
printf("ERROR: a1 = %u, a2 = %u, expected w = %u, expected num_ones = %u, resultw = %u,resultw = %u\n", values[i].a1, values[i].a2, values[i].w, values[i].num_ones, result.w,result.l);
k++;
}
return k;
}



