/** @file logger.cpp
 *  @author WENYUAN YU
 *  @date 07/05/2022
 *  @brief Vehicle Data Logger Function
 */
#include "logger.h"
#include <cinttypes>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include "sensehat.h"
#include "dlgps.h"
#include <cstddef>
#include "loggermqtt.h"

// Global Objects
SenseHat Sh;

/** @brief Vehicle Data Logger DIInitialization function
 *  @author WENYUAN YU
 *  @date 08/05/2022
 *  @param void
 *  @return integer
 */
int DlInitialization(void){
    fprintf(stdout,"Data Logger Initialization, Creator: WENYUAN YU\n");
    return 0;
 }

/** @brief Vehicle Data Logger getSerial function
 *  @author WENYUAN YU
 *  @date 08/05/2022
 *  @param void
 *  @return unit64_t
 */
uint64_t DlGetSerial(void)
{
	static uint64_t serial = 0;
	FILE * fp;
	char buf[SYSINFOBUFSZ];
	char searchstring[] = SEARCHSTR;
	fp = fopen ("/proc/cpuinfo", "r");
	if (fp != NULL)
	{
		while (fgets(buf, sizeof(buf), fp) != NULL)
		{
if (!strncasecmp(searchstring, buf, strlen(searchstring)))

			{
				sscanf(buf+strlen(searchstring), "%Lx", &serial);
			}
		}
		fclose(fp);
	}
     if(serial==0)
     {
         system("uname -a");
         system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
         fp = fopen ("stamp.txt", "r");
         if (fp != NULL)
         {
             while (fgets(buf, sizeof(buf), fp) != NULL)
             {
                sscanf(buf, "%Lx", &serial);
             }
             fclose(fp);
        }
     }
	return serial;
}

/** @brief Vehicle Data Logger getReading function
 *  @author WENYUAN YU
 *  @date 16/05/2022
 *  @param void
 *  @return reading_s
 */
reading_s DlGetLoggerReadings(void)
{
    reading_s creads;
    loc_t gpsdata = {0.0};
    time_t t;

#if SENSEHAT
    creads.rtime = t;
    creads.temperature = Sh.GetTemperature();
    usleep(IMUDELAY);
    creads.humidity = Sh.GetHumidity();
    usleep(IMUDELAY);
    creads.pressure = Sh.GetPressure();
    usleep(IMUDELAY);
    Sh.GetAcceleration(creads.xa, creads.ya,creads.za);
    usleep(IMUDELAY);
    Sh.GetOrientation(creads.pitch,creads.roll,creads.yaw);
    usleep(IMUDELAY);
    Sh.GetMagnetism(creads.xm,creads.ym,creads.zm);
#else
    t=time(NULL);
    creads.temperature = DTEMP;
    creads.humidity = DHUMID;
    creads.pressure = DPRESS;
    creads.xa = DXA;
    creads.ya = DYA;
    creads.za = DZA;
    creads.pitch = DPITCH;
    creads.roll = DROLL;
    creads.yaw = DYAW;
    creads.xm = DXM;
    creads.ym = DYM;
    creads.zm = DZM;
#endif

#if GPSDEVICE
    gpsdata = DlGpsLocation();
    creads.latitude = gpsdata.latitude;
    creads.longitude = gpsdata.longitude;
    creads.altitude = gpsdata.altitude;
    creads.speed = gpsdata.speed;
    creads.heading = gpsdata.course;
#else
    creads.latitude=DLAT;
    creads.longitude = DLONG;
    creads.altitude = DALT;
    creads.speed = DSPEED;
    creads.heading = DHEADING;
#endif //GPSDEVICE

    return creads;
}

/** @brief Vehicle Data Logger Display Logger Reading function
 *  @author WENYUAN YU
 *  @date 08/05/2022
 *  @param lreads of reading_s structure
 *  @return void
 */
void DlDisplayLoggerReadings(reading_s lreads){
    fprintf(stdout, "\nUnit: %" PRIu64 " %s", DlGetSerial(),ctime(&lreads.rtime));
    fprintf(stdout,"T: %fC      H: %f%   P: %fkPa\n",lreads.temperature,lreads.humidity,lreads.pressure);
    fprintf(stdout,"Xa: %fg      Ya:%fg        Za: %fg\n",lreads.xa,lreads.ya,lreads.za);
    fprintf(stdout,"Pitch: %f     Roll: %f          Yaw: %f\n",lreads.pitch,lreads.roll,lreads.yaw);
    fprintf(stdout,"Xm: %f      Ym:%f        Zm:%f\n",lreads.xm,lreads.ym,lreads.zm);
    fprintf(stdout,"Latitude: %f      Longitude:%f     Altitude:%f\n",lreads.latitude,lreads.longitude,lreads.altitude);
    fprintf(stdout,"Speed: %f         Heading:%f\n\n",lreads.speed,lreads.heading);
}

/** @brief Vehicle Data Logger Save Logger Data function
 *  @author WENYUAN YU
 *  @date 12/06/2022
 *  @param lreads of reading_s structure
 *  @return void
 */
int DlSaveLoggerData(reading_s creads){
    FILE *fp;
    char ltime[TIMESTARSZ];
    char jsondata[PAYLOADSTRSZ];
    int rc;



    if((fp=fopen("loggerdata.csv","a"))==NULL)
    {
        fprintf(stdout,"Error for opening the file!");
        return 0;
    }
    strcpy(ltime,ctime(&creads.rtime));
    ltime[3]=',';
    ltime[7]=',';
    ltime[10]=',';
    ltime[19]=',';
    fprintf(fp,"%.24s,%3.1f,%3.0f,%3.1f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",ltime,creads.temperature,creads.humidity,creads.pressure,
    creads.roll,creads.pitch,creads.yaw,creads.xa,creads.ya,creads.za,creads.xm,creads.ym,creads.zm,creads.latitude,creads.longitude,creads.altitude
    ,creads.speed,creads.heading);
    fclose(fp);

    sprintf(jsondata,"{\"temperature\":%-3.1f,\"humidity\":%-3.0f,\"pressure\":%-3.1f,\"xa\":%-f,\"ya\":%-f,\"za\":%-f,\
    \"pitch\":%-f,\"roll\":%-f,\"yaw\":%-f,\"xm\":%-f,\"ym\":%-f,\"zm\":%-f,\"latitude\":%-f,\"longitude\":%-f,\"altitude\":%-f,\"speed\":%-f,\"heading\":%-f,\"active\": true}"
    ,creads.temperature,creads.humidity,creads.pressure,creads.xa,creads.ya,creads.za,creads.pitch,creads.roll,creads.yaw,creads.xm,creads.ym,creads.zm,creads.latitude,creads.longitude,creads.altitude
    ,creads.speed,creads.heading);
    if((fp=fopen("loggerdata.json","w"))==NULL)
    {
        fprintf(stdout,"Error for opening the file!");
        return -1;
    }
    fprintf(fp,"%s",jsondata);
    fclose(fp);

    rc = DlPublishLoggerData(jsondata);

    return rc;
}

/** @brief Vehicle Data Logger Display Logo function
 *  @author WENYUAN YU
 *  @date 29/05/2022
 *  @param void
 *  @return void
 */
void DlDisplayLogo(void){
    uint16_t logo[8][8] = {
            HB,HB,HB,HB,HB,HB,HB,HB,
            HB,HB,HW,HB,HB,HW,HB,HY,
            HB,HB,HW,HB,HB,HW,HY,HY,
            HB,HB,HW,HB,HB,HW,HY,HY,
            HB,HB,HW,HW,HW,HW,HY,HY,
            HB,HB,HW,HY,HY,HW,HY,HY,
            HB,HY,HW,HY,HY,HW,HY,HY,
            HY,HY,HY,HY,HY,HY,HY,HY,
            };
    Sh.WipeScreen();
    Sh.ViewPattern(logo);
}

/** @brief Vehicle Data Logger Update Level function
 *  @author WENYUAN YU
 *  @date 29/05/2022
 *  @param float xa
 *  @param float ya
 *  @return void
 */
void DlUpdateLevel(float xa, float ya){
    int x, y;

    Sh.WipeScreen();

    y=(int)(xa * -30.0 + 4);
    x=(int)(ya * -30.0 + 4);
    if(x<0){
        x=0;
    }else if(x>6){
        x=6;
    }

    if(y<0){
        y=0;
    }else if(y>6){
        y=6;
    }

    Sh.LightPixel(x,y,HY);
    Sh.LightPixel(x+1,y,HY);
    Sh.LightPixel(x,y+1,HY);
    Sh.LightPixel(x+1,y+1,HY);
}
