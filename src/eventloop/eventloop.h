#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <sys/time.h>
#include <sys/types.h>

#define READ_EVENT  1 << 0
#define WRITE_EVENT 1 << 1

typedef struct Event Event;
typedef struct EventLoop EventLoop;

typedef void EventHandler(EventLoop* eventloop, int fd);

typedef struct Event {
    int fd;
    int flags;
    EventHandler* handler;
    Event* next;
} Event;

typedef struct EventLoop {
    Event* event_head;
    struct timeval* loop_interval;
} EventLoop;

EventLoop* create_eventloop(void);
void start_eventloop(EventLoop* eventloop);
void process_events(EventLoop* eventloop);

Event* create_event(int fd, int flags, EventHandler* handler);
void free_event(Event* ev);
void append_event(EventLoop* eventloop, Event* ev);

#endif
