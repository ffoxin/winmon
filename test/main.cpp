#include <stdio.h>


const char *msg1( )
{
    static char msg[256] = { };
    static int counter = 0;

    sprintf( msg, "value is %d", ++counter );

    return msg;
}

int main(int argc, char **argv)
{
    for( size_t i = 0; i < 10; ++i )
    {
        const char *msg = msg1( );
        printf( "%s\n", msg );
    }

    return 0;
}
