#ifndef NTP_H_   /* Include guard */
#define NTP_H_

#include <sys/time.h>
static void obtain_time(void);
static void initialize_sntp(void);
void time_sync_notification_cb(struct timeval*);
void ntp_sync_task();

#endif
