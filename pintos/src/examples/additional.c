#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
    if(argc != 5) return EXIT_FAILURE;

    int fib, max_int;
    fib = fibonacci( atoi(argv[1]) ) ;
    printf("%d ", fib);
    max_int =  max_of_four_int( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]) );
    printf("%d\n", max_int);
    
    return EXIT_SUCCESS;      
}
