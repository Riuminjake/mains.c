#include <stdio.h>
#include <winsock2.h>
#include <locale.h>
#include <pthread.h>
#include <time.h>
#pragma comment(lib, "Ws2_32.lib")
#define PORT 1000
#define BUFLEN 10000
void *zapis();
char prislo();
char uslo();
BOOL name =FALSE;
BOOL l=TRUE;
int iResult;
char zapiska[BUFLEN];
time_t t;
struct tm* aTm;
HANDLE  hConsole;
FILE *fl;
int main() {
    char servak[16];
    char vibor[3];
    printf("Your name?\n");
    gets(zapiska);
    if ((fl = fopen("servak.txt", "r")) != NULL) {
        if ((fgets(servak, 16, fl)) != NULL) {
            // puts(servak);
            printf("connected to: %s yes or no\n", servak);
        }
    }
    scanf("%s", vibor);
    if ((0 == strncmp(vibor, "no", 2)) || fl == NULL) {
        fl = fopen("servak.txt", "w");
        ZeroMemory (servak, sizeof(servak));
        puts("IP:");
        scanf("%s", servak);
        fputs(servak, fl);
    }
    fclose(fl);
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
    WSADATA proba;
    SOCKET w;
    SOCKET new_w;
    struct sockaddr_in adress, new_adress;
    int new_len = sizeof(new_adress);
    while (1) {
        ZeroMemory (&adress, sizeof(adress));
        ZeroMemory (&new_adress, sizeof(new_adress));
        adress.sin_family = AF_INET;
        adress.sin_addr.S_un.S_addr = inet_addr(servak);
        adress.sin_port = htons(PORT);
        if (0 != (iResult = WSAStartup(MAKEWORD(2, 2), &proba))) {
            printf("WSAStartup failed: %d\\n", iResult);
            return WSAGetLastError();
        }
        if (INVALID_SOCKET == (w = socket(AF_INET, SOCK_STREAM, 0)))
            return WSAGetLastError();
        if (-1 == (connect(w, (struct sockaddr *) &adress, sizeof(adress)))) {
            ZeroMemory (&adress, sizeof(adress));
            closesocket(w);
            if (INVALID_SOCKET == (w = socket(AF_INET, SOCK_STREAM, 0)))
                return WSAGetLastError();
            adress.sin_family = AF_INET;
            adress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
            adress.sin_port = htons(PORT);
            if (SOCKET_ERROR == bind(w, (struct sockaddr *) &adress, sizeof(adress)))
                return E_FAIL;
            if (FAILED(listen(w, 10)))
                return E_FAIL;
            ZeroMemory (&new_adress, sizeof(new_adress));
            if (FAILED(new_w = accept(w, (struct sockaddr *) &new_adress, &new_len))) {
                perror("accept");
            }
            printf("Connected\n");
        } else {
            new_w = w;
            //closesocket(w);
            printf("Connected to server!\n");
        }
        int z = 0;
        pthread_t thread_id;//для ветвления программы
        do {
            if (name)
                zapiska[0] = '\0';
            Sleep(250);
            z = prislo(&new_w);
            if (z == 0)
                break;
            if (zapiska[0] != '\0') {
                z = uslo(&new_w);
                name=TRUE;
            }
            pthread_create(&thread_id, NULL, zapis, NULL);//развлетвление
        } while (z > 0);
        // pthread_join(thread_id, NULL);//конец разветвления программы
        closesocket(w);
        closesocket(new_w);
        WSACleanup();
        if (((strstr(zapiska, "#exit")) > 0) && (strlen(zapiska) == 6))
            return 0;
    }
}
void *zapis(){
    char b[101];
    char* z=NULL;
    do {
        z=fgets(b, 100, stdin);
        strcat(zapiska,z);
    }while(z!=NULL);
}

char prislo(SOCKET *ConnectSocket) {
    SetConsoleTextAttribute(hConsole,7);
    char recvbuf[BUFLEN];
    BOOL odin_raz=FALSE;
    if (SOCKET_ERROR==ioctlsocket(*ConnectSocket,FIONBIO,(unsigned long*)&l))
        return 0;
    do{
    iResult = recv(*ConnectSocket, recvbuf, BUFLEN, 0);
    if ( iResult > 0 ){
        if (odin_raz==FALSE){
            printf("\n");
            t= time(NULL);
            aTm= localtime(&t);
            printf("%s",asctime(aTm));
            printf("&s:\n",name);
            odin_raz=TRUE;
        }
        recvbuf[iResult]='\0';
        for(int i=0;i<iResult;i++){
            SetConsoleTextAttribute(hConsole,6);
            printf("%c",recvbuf[i]);
        }
    }
    else if ( iResult == 0 ) {
        printf("Connection closed\n");
        return 0;
    }
    }while(iResult>0);
    if (odin_raz==TRUE)
        printf("\n");
    return 1;
}

char uslo(SOCKET *ConnectSocket) {
    SetConsoleTextAttribute(hConsole,7);
    if((strstr(zapiska,"#exit"))>0&&(strlen(zapiska)==6))
        return 0;
    iResult = send(*ConnectSocket, zapiska, (int) strlen(zapiska), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(*ConnectSocket);
        WSACleanup();
        return 0;
    }
    return 1;
}
