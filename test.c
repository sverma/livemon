#include <stdio.h>
#include <stdlib.h>

struct test {
	int a;
	int b; 
};
int main(int argc , char *argvp[] ) {
	struct test * p  = (struct test * )malloc(sizeof(struct test));
	char * str = "12334343        343";
	int a;
	int b; 
	sscanf(str,"%d %d",&a,&b); 
	printf("%d\n", b ); 

}
