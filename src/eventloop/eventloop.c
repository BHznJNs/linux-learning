#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include "eventloop.h"

EventLoop* create_eventloop() {
    EventLoop* eventloop = malloc(sizeof(EventLoop));
    eventloop->event_head = NULL;

    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    eventloop->loop_interval = ;
}

Event* create_event(EventCallback* callback) {
    Event* ev = malloc(sizeof(Event));
    ev->callback = callback;
    ev->next = NULL;
    return ev;
}

void append_event(Event* head, Event* ev) {
    ev->next = head;
    *head = *ev;
}

void start_eventloop(EventLoop* eventloop) {
    // 
}
