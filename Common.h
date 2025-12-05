// common.h
#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#define SERVER_QUEUE_NAME   "/chat_server_queue"
#define USER_QUEUE_NAME_FMT "/chat_user_%d"
#define MAX_USERS 64
#define MAX_TEXT 256

// tipos de mensagem enviados ao servidor
typedef enum {
    MSG_REGISTER = 1, // registra usuário (envia seu pid)
    MSG_TEXT     = 2, // mensagem de chat
    MSG_EXIT     = 3  // saída do usuário
} msg_type_t;

// Estrutura da mensagem enviada pelo usuário ao servidor
typedef struct {
    msg_type_t type;
    pid_t sender_pid;          // pid do usuário remetente
    char user_queue_name[64];  // usado no registro: nome da fila do usuário
    char text[MAX_TEXT];       // texto da mensagem
} chat_msg_t;

#endif // COMMON_H
