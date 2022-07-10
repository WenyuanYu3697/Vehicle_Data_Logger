#ifndef LOGGERMQTT_H_INCLUDED
#define LOGGERMQTT_H_INCLUDED

 // Constants
 // Default LOGGERMQTT values
#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "VehicleDataLogger"
#define TOPIC       "Logger Data"
#define QOS         1
#define TIMEOUT     10000L

 ///\cond INTERNAL
 //Function Prototypes
int DlPublishLoggerData(const char * mqttdata);
 ///\endcond

#endif // LOGGERMQTT_H_INCLUDED
