#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <time.h>

typedef void EventCallback(int fd);

typedef struct Event {
    int fd;
    EventCallback* callback;
    struct Event* next;
} Event;

typedef struct EventLoop {
    Event* event_head;
    timeval loop_interval;
} EventLoop;

EventLoop* create_eventloop();
void start_eventloop(EventLoop* eventloop);
Event* create_event(EventCallback* callback);
void append_event(Event* head, Event* ev);

#endif
