#include <stdlib.h>
#include <stddef.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include "eventloop.h"
#include "../utils.h"

static void set_loop_interval(EventLoop* eventloop) {
    struct timeval* tv = malloc(sizeof(struct timeval));
    tv->tv_sec = 1;
    tv->tv_usec = 0;
    eventloop->loop_interval = tv;
}

EventLoop* create_eventloop(void) {
    EventLoop* eventloop = malloc(sizeof(EventLoop));
    eventloop->event_head = NULL;
    set_loop_interval(eventloop);
    return eventloop;
}

Event* create_event(int fd, int flags, EventHandler* handler) {
    Event* ev = malloc(sizeof(Event));
    ev->fd      = fd;
    ev->flags   = flags;
    ev->next    = NULL;
    ev->handler = handler;
    return ev;
}

void free_event(Event* ev) {
    free(ev);
}

void append_event(EventLoop* eventloop, Event* ev) {
    ev->next = eventloop->event_head;
    eventloop->event_head = ev;
}

void process_events(EventLoop* eventloop) {
    fd_set rfds, wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    int max_fd = 0;
    Event* ev = eventloop->event_head;
    while (ev != NULL) {
        if (ev->flags & READ_EVENT ) FD_SET(ev->fd, &rfds);
        if (ev->flags & WRITE_EVENT) FD_SET(ev->fd, &wfds);

        if (ev->fd > max_fd) max_fd = ev->fd;
        ev = ev->next;
    }

    int ret_val = select(max_fd + 1, &rfds, &wfds, NULL, eventloop->loop_interval);
    logger("DEBUG", "select ret_val=%d", ret_val);
    if (ret_val > 0) {
        ev = eventloop->event_head;
        while (ev != NULL) {
            if (
                ev->flags & READ_EVENT  && FD_ISSET(ev->fd, &rfds) ||
                ev->flags & WRITE_EVENT && FD_ISSET(ev->fd, &wfds)
            ) {
                ev->handler(eventloop, ev->fd);
            }
            ev = ev->next;
        }
    }
    set_loop_interval(eventloop);
}

void start_eventloop(EventLoop* eventloop) {
    while (1) {
        logger("INFO", "loop start");
        process_events(eventloop);
        logger("INFO", "loop end");
    }
}
