#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define SEM_NAME "/sem"

int main(int argc, char *argv[])
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    int sockfd;
    struct sockaddr_in addr;
    int port;
    socklen_t addr_len = sizeof(addr);

    // Проверки на корректность входных данных
    if (argc != 3) {
        printf("Usage: %s <port> or not count in input\n", argv[0]);
        exit(-1);
    }
    int cnt_proc = strtoll(argv[1], NULL, 10);
    if (cnt_proc <= 0) {
        return 0;
    }

    port = atoi(argv[2]);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sem_t **sem = malloc(cnt_proc * sizeof(*sem));
    pid_t *sons = malloc(cnt_proc * sizeof(sons));
    for (int i = 0; i < cnt_proc; ++i) {
        char *path[100];
        sprintf(path, "%s%d", SEM_NAME, i);
        // Создание именованного семафора
        sem[i] = sem_open(path, O_CREAT, 0644, 0);
        if (sem[i] == SEM_FAILED) {
            printf("failed sem_open");

            // закрываем если поломалось
            for (int j = i - 1; j > -1; --j) {
                sem_close(sem[j]);
            }
            exit(-1);
        }
    }

    // на всякий случай выводим порт и число болтунов
    printf("port: %d, count: %d\n", port, cnt_proc);

    // начинаем создавать сам сервер
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("bind() error");
        exit(-1);
    }

    // ждём клиентов
    struct sockaddr_in client_addrs[cnt_proc];
    for (int i = 0; i < cnt_proc; ++i) {
        int name[2] = {i, 0};
        recvfrom(sockfd, NULL, 0, 0, (struct sockaddr *)&client_addrs[i], &addr_len);
        sendto(sockfd, name, sizeof(name), 0, (struct sockaddr *)&client_addrs[i], addr_len);
    }

    // сам процесс общения
    for (int i = 0; i < cnt_proc; ++i) {
        sons[i] = fork();
        if (sons[i] < 0) {
            printf("Can\'t fork()\n");
            exit(-1);
        } else if (sons[i]) {
            srand(i);
            int state = (i + 1)% 2;
            if (!state) { // проверка на ждущего
                sem_post(sem[i]);
            }
            struct sockaddr_in client_addr = client_addrs[i];
            while (1) {
                if (state) { // проверка на звонящего
                    printf("%d %s", i, "назначен звонящим\n");
                    int want_to_call = rand() % cnt_proc;
                    int check = -1;
                    while (want_to_call == i || check == -1) {
                        want_to_call = rand() % cnt_proc;
                        if (want_to_call != i) {
                            check = sem_trywait(sem[want_to_call]);
                        }
                    }
                    printf("%d %s", i, "звонит\n");
                    int nums[2] = {2, want_to_call};
                    sendto(sockfd, nums, sizeof(nums), 0, (struct sockaddr *)&client_addr, addr_len); // начал разговор
                } else { // else на ждущего
                    printf("%d %s", i, "назначен ждущим\n");
                    int own_value;
                    while (1) { // процесс ожидания
                        sem_getvalue(sem[i], &own_value);
                        if (own_value == 0) {
                            int nums[2] = {1, i};
                            printf("%d %s", i, "принял\n");
                            sendto(sockfd, nums, sizeof(nums), 0, (struct sockaddr *)&client_addr, addr_len); // начал разговор
                            break;
                        }
                    }
                }
                int receivedNumber; // ждём сигнала конца диалога
                recvfrom(sockfd, &receivedNumber, sizeof(receivedNumber), 0, (struct sockaddr *)&client_addr, &addr_len);
                // обновляем роль
                state = rand() % 2;
                if (!state) { // проверка на ждущего
                    sem_post(sem[i]);
                }
            }
        }
    }

    // время всех звонков
    int exec_time = 20;
    sleep(exec_time);

    // очистка
    for (int i = 0; i < cnt_proc; i++) {
        int nums[2] = {-1, -1};
        sendto(sockfd, nums, sizeof(nums), 0, (struct sockaddr *)&client_addrs[i], addr_len);
        kill(sons[i], SIGKILL);
    }
    for (int i = 0; i < cnt_proc; ++i) {
        sem_close(sem[i]);
    }

    free(sem);
    free(sons);

    for (int i = 0; i < cnt_proc; ++i) {
        char *path[100];
        sprintf(path, "%s%d", SEM_NAME, i);
        sem_unlink(path);
    }
    close(sockfd);
    return 0;
}
