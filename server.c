// server.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

typedef struct {
    pid_t pid;
    char qname[64];
    mqd_t mqd;
} user_entry_t;

static mqd_t server_mqd = (mqd_t)-1;
static user_entry_t users[MAX_USERS];
static int users_count = 0;

void cleanup_and_exit(int code) {
    if (server_mqd != (mqd_t)-1) {
        mq_close(server_mqd);
        mq_unlink(SERVER_QUEUE_NAME);
    }
    // close user mqd if opened
    for (int i=0;i<users_count;i++){
        if (users[i].mqd != (mqd_t)-1) mq_close(users[i].mqd);
    }
    printf("[SERVER] exiting\n");
    exit(code);
}

void sigint_handler(int sig) {
    (void)sig;
    cleanup_and_exit(0);
}

int find_user_by_pid(pid_t pid) {
    for (int i=0;i<users_count;i++) if (users[i].pid == pid) return i;
    return -1;
}

void remove_user_by_index(int idx) {
    if (idx < 0 || idx >= users_count) return;
    // close its mqd if open
    if (users[idx].mqd != (mqd_t)-1) mq_close(users[idx].mqd);
    // shift array
    for (int i=idx;i<users_count-1;i++) users[i] = users[i+1];
    users_count--;
}

void broadcast_message(const chat_msg_t *m) {
    // forward to all users except sender
    for (int i=0;i<users_count;i++) {
        if (users[i].pid == m->sender_pid) continue;
        mqd_t q = users[i].mqd;
        if (q == (mqd_t)-1) continue;
        // send a copy
        if (mq_send(q, (const char*)m, sizeof(chat_msg_t), 0) == -1) {
            perror("[SERVER] mq_send to user failed");
        }
    }
}

int main(void) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(chat_msg_t);
    attr.mq_curmsgs = 0;

    // cleanup existing server queue if any
    mq_unlink(SERVER_QUEUE_NAME);

    server_mqd = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_mqd == (mqd_t)-1) {
        perror("[SERVER] mq_open");
        exit(1);
    }

    signal(SIGINT, sigint_handler);
    printf("[SERVER] chat server started. Queue: %s\n", SERVER_QUEUE_NAME);

    chat_msg_t msg;
    while (1) {
        ssize_t r = mq_receive(server_mqd, (char*)&msg, sizeof(msg), NULL);
        if (r == -1) {
            perror("[SERVER] mq_receive");
            continue;
        }

        if (msg.type == MSG_REGISTER) {
            if (users_count >= MAX_USERS) {
                printf("[SERVER] max users reached, ignoring registration\n");
                continue;
            }
            // open user's queue for sending
            mqd_t user_q = mq_open(msg.user_queue_name, O_WRONLY);
            if (user_q == (mqd_t)-1) {
                perror("[SERVER] mq_open user queue");
                continue;
            }
            user_entry_t e;
            e.pid = msg.sender_pid;
            strncpy(e.qname, msg.user_queue_name, sizeof(e.qname));
            e.mqd = user_q;
            users[users_count++] = e;
            printf("[SERVER] user registered: pid=%d q=%s (total=%d)\n", e.pid, e.qname, users_count);
            // announce join
            chat_msg_t announce;
            announce.type = MSG_TEXT;
            announce.sender_pid = 0;
            snprintf(announce.text, sizeof(announce.text), "[SERVER] User %d joined the chat", e.pid);
            broadcast_message(&announce);
        }
        else if (msg.type == MSG_TEXT) {
            // print to server console then broadcast
            printf("[SERVER] %d: %s\n", msg.sender_pid, msg.text);
            broadcast_message(&msg);
        }
        else if (msg.type == MSG_EXIT) {
            int idx = find_user_by_pid(msg.sender_pid);
            if (idx != -1) {
                printf("[SERVER] user %d exiting\n", msg.sender_pid);
                // notify others
                chat_msg_t announce;
                announce.type = MSG_TEXT;
                announce.sender_pid = 0;
                snprintf(announce.text, sizeof(announce.text), "[SERVER] User %d left the chat", msg.sender_pid);
                // remove user first then broadcast (or broadcast then remove)
                remove_user_by_index(idx);
                broadcast_message(&announce);
            }
        }
    }

    cleanup_and_exit(0);
    return 0;
}
