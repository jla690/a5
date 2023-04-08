// header for controlling threads
#ifndef THREAD_CONTROL_H
#define THREAD_CONTROL_H

// start thread controller
void initThreads();

// function for thread controller
void* threadControlThread(void* arg);

// stop all threads
void stopThreads();

#endif
