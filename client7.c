#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in echoServAddr;
    int nums[2];
    char servIP[20];
    int echoServPort;
    socklen_t addr_len = sizeof(echoServAddr);

    // Проверки на корректность входных данных
    if (argc != 3) {
        printf("Usage: %s <servIP> <echoServPort>\n", argv[0]);
        exit(-1);
    }
    strncpy(servIP, argv[1], sizeof(servIP));
    if (sscanf(argv[2], "%d", &echoServPort) != 1) {
        printf("Invalid port.\n");
        exit(-1);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // подключение
    sendto(sock, "Connect", 8, 0, (struct sockaddr *)&echoServAddr, addr_len);

    // задаём номер болтуна/логера
    int name;
    recvfrom(sock, nums, sizeof(nums), 0, (struct sockaddr *)&echoServAddr, &addr_len);
    name = nums[0];
    if (name != -1) {
        printf("меня зовут %d\n", name);
        char log_inf[256];
        snprintf(log_inf, sizeof(log_inf), "меня зовут %d\n", name);
        sendto(sock, log_inf, sizeof(log_inf), 0, (struct sockaddr *)&echoServAddr, addr_len);
        // говорим
        while (1) {
            memset(nums, 0, sizeof(nums));
            int numToSend = 3;
            recvfrom(sock, nums, sizeof(nums), 0, (struct sockaddr *)&echoServAddr, &addr_len);
            if (nums[0] == 2) { // роль звонящего
                int want_to_call = nums[1];
                char log_inf1[256];
                printf("%d %s %d %s", name, "позвонил", want_to_call, "\n");
                snprintf(log_inf1, sizeof(log_inf1), "%d %s %d %s", name, "позвонил", want_to_call, "\n");
                sendto(sock, log_inf1, sizeof(log_inf1), 0, (struct sockaddr *)&echoServAddr, addr_len);
                // TALKING
                sleep(10);
                printf("%d %s %d %s", name, "завершился", want_to_call, "\n");
                // call finished
                char log_inf2[256];
                snprintf(log_inf2, sizeof(log_inf2), "%d %s %d %s", name, "завершился", want_to_call, "\n");
                sendto(sock, log_inf2, sizeof(log_inf2), 0, (struct sockaddr *)&echoServAddr, addr_len);
            } else if (nums[0] == 1) { // роль ждущего
                printf("%d %s", name, "болтает\n");
                char log_inf1[256];
                snprintf(log_inf1, sizeof(log_inf1), "%d %s", name, "болтает\n");
                sendto(sock, log_inf1, sizeof(log_inf1), 0, (struct sockaddr *)&echoServAddr, addr_len);
                // TALKING
                sleep(10);
                printf("%d %s %s", name, "завершился", "\n");
                // call finished
                char log_inf2[256];
                snprintf(log_inf2, sizeof(log_inf2), "%d %s %s", name, "завершился", "\n");
                sendto(sock, log_inf2, sizeof(log_inf2), 0, (struct sockaddr *)&echoServAddr, addr_len);
            } else {
                break;
            }
            nums[0] = -1;
        }
    } else {
        printf("меня зовут логер\n");
        while (1) {
            char log_inf[256];
            recvfrom(sock, log_inf, sizeof(log_inf), 0, (struct sockaddr *)&echoServAddr, &addr_len);
            if (log_inf[0] == 'a') {
                break;
            }
            printf("%s\n", log_inf);
        }
    }
    close(sock);
    return 0;
}
