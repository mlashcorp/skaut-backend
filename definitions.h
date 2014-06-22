/* 
 * File:   definitions.h
 * Author: cortereal
 *
 * Created on July 28, 2013, 1:05 PM
 */

#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#define OUTPUT_PERFORMANCE_METRICS false //For time measurements
#define OUTPUT_SENSOR_DATA false //for algorithm business logic
#define DEBUG_VERBOSE_FLAG true //main debug flag override
#define MAX_DEBUG_SIZE 200 //Max char number per line for debug
#define ERROR -1
#define SUCCESS 0

#define ALARM_THRESHOLD 3000 //Science was used to calculate this number ...
#define ALARM_COUNT_THRESHOLD 25

#define ALARM_INHIBIT_TIMER 120
enum alarm_type
{
    ALL_CLEAR   = 0,
    ALARM_UNKNOWN,
    ALARM_TRIGGERED,
    ALARM_INHIBITED
};

#define ASSERT(test)  if ((test)); \
    else HSio::debug("Assert(%s) failed in file %s, line  %d\n", #test, __FILE__, __LINE__ )
#define ASSERT_RETURN(test))  if ((test)); \
    else { HSio::debug("Assert(%s) failed in file %s, line  %d\n", #test, __FILE__, __LINE__ ); \
    return; }
#define ASSERT_RETURN_NULL(test)  if ((test)); \
    else { HSio::debug("Assert(%s) failed in file %s, line  %d\n", #test, __FILE__, __LINE__ ); \
    return NULL_POINTER; }
#define ASSERT_RETURN_INT(test, retval)  if ((test)); \
    else { HSio::debug("Assert(%s) failed in file %s, line  %d\n", #test, __FILE__, __LINE__ ); \
    return retval; }

#endif	/* DEFINITIONS_H */

