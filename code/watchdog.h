#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#define clear_wdt() do { WDTCTL = WDTPW | WDTCNTCL; } while (0)
#endif
