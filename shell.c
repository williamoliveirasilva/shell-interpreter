#include <stdio.h>
#include <stdlib.h>

int main()
{
    char buffer[1024];

    fgets(buffer, sizeof(buffer), stdin);
    system(buffer); //para testar se está funcionando

    return 0;
}
