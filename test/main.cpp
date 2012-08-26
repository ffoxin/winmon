#include <stdio.h>

#define __ITOS2__(x) #x
#define __ITOS__(x) __ITOS2__(x)
#define __DLINE__ __ITOS__(__LINE__)

#define __TEST__ (__FILE__ ":" __DLINE__)




int main(int argc, char **argv)
{
    printf("%s\n", __FILE__);
    printf("%d\n", __LINE__);
    printf("%s\n", __TEST__);

    return 0;
}
