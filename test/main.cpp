#include <iostream>
#include <stdio.h>


const char * log( const char * msg )
{
    std::cout << msg << std::endl;

    return msg;
}

int main(int argc, char **argv)
{
    const char * msg;
    
    {
        msg = log( "hello" );
        std::cout << log( "hi" ) << std::endl;
    }

    std::cout << msg << std::endl;

    const char * msg2 = "hello";

    printf( "%p\n", msg );
    printf( "%p\n", msg2 );

    return 0;
}
