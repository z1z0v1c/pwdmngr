#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

int choose_option(void);

int main(void)
{
    int option = choose_option();
}

int choose_option(void)
{
    int option;
    char should_continue[3];

    while (option != 1 && option != 2)
    {
        printf("Choose an option (1/2): \n");
        printf("1. Login \n");
        printf("2. Register \n");

        scanf("%d", &option);

        // Flush the buffer
        char line[100];
        fgets(line, sizeof(line), stdin);
    }
    
    return option;
}
