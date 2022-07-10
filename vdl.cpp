/** @file vdl.cpp
 *  @author WENYUAN YU
 *  @date 02/05/2022
 *  @brief Vehicle Data Logger Main Function
 */
#include <cstdio>
#include <unistd.h>
#include "vdl.h"
#include "logger.h"
#include "sensehat.h"
#include "dlgps.h"

/** @brief Vehicle Data Logger main function
 *  @author WENYUAN YU
 *  @date 08/05/2022
 *  @param void
 *  @return int program status
 */
int main(void)
{
    reading_s creads={0};
    int tc = 0;

    DlGpsInit();
    DlInitialization();
    DlDisplayLogo();
    sleep(3);
    while(1)
    {
        creads = DlGetLoggerReadings();
        DlUpdateLevel(creads.xa,creads.ya);
        if(tc==LOGCOUNT)
        {
            DlDisplayLoggerReadings(creads);
            DlSaveLoggerData(creads);
            tc=0;
        }else
        {
            usleep(SLEEPTIME);
            tc++;
        }
    }

    return 1;
}

