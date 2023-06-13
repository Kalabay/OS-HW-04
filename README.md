# Отчёт, Калабай Михаил Иванович БПИ219
## Работа претендует на 7 баллов, Вариант 19

19. Задача о болтунах. N болтунов имеют телефоны. Они либо ждут
звонков, либо звонят друг другу, чтобы побеседовать. Выбор этого
состояния осуществляется случайно и равновероятно. Если телефон случайного абонента занят, болтун будет звонить другому абоненту, пока ему кто-нибудь не ответит. Побеседовав некоторое время, болтун равновероятно или ждет звонка, или звонит на другой
случайный номер. Создать приложение, моделирующее поведение болтунов. Каждый болтун — отдельный клиент. Сервер
получает число болтунов при запуске и является коммутатором.
Запуск клиентов можно осуществлять скриптом или вручную.

## Решение
Для проверок свободности болтунов я использую именованые семафоры (и процессы). Внутри кода есть параметр exec_time, 
который означает сколько секунд длится симуляция. По истечении времени сразу конец всего. 
Сервер-коммутатор главный, он решает, кто я является звонящим, а кто ждёт звонка и т.д.
А также сервер посылает информацию клиентам-болтунам, кому из свободных болтун должен позвонить.
Далее в зависимости от сценария клиент звонит или слушает звонящего (сделал через sleep(10))

## Про решение на 4-5 
[код-сервер](https://github.com/Kalabay/OS-HW-04/blob/main/client.c)
[код-клиент](https://github.com/Kalabay/OS-HW-04/blob/main/server.c)
Передаю через ввод в терминале серверу аргументы = количество болтунов и порт.
Передаю через ввод в терминале клиенту аргументы = адресс и порт

## Про решение на 6-7
добавил клиент для логирования и обновил сервер. Логер = первый подключенный клиент, следующие уже опять болтуны
[код-сервер](https://github.com/Kalabay/OS-HW-04/blob/main/client7.c)
[код-клиент](https://github.com/Kalabay/OS-HW-04/blob/main/server7.c)


### Примеры
Обычный вывод

![2023-06-13 23 55 05](https://github.com/Kalabay/OS-HW-04/assets/90344366/de77c275-575c-45fd-9092-cf98fdbbd812)

Краткий пример логера

![2023-06-13 23 55 11](https://github.com/Kalabay/OS-HW-04/assets/90344366/5f31fbd1-7fe5-4063-a9d7-b7e73891e415)
