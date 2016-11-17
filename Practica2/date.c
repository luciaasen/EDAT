#include <stdio.h>
#include <time.h>

int main(){
        time_t tiempo = time(0);
        struct tm *tlocal = localtime(&tiempo);
        char output[128];
        strftime(output,128,"%Y/%m/%d",tlocal);
        printf("%s\n",output);

        return 0;
}
