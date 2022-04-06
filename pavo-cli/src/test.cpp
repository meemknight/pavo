#include <cstdio>

int main()
{
        while(true)
        {
                int x = scanf("%d", &x);
                if(x == 0)
                {
                        break;
                }

                printf("received: %d\n", x);
        }
}
