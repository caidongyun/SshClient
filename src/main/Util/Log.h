/*
 * Log.h
 *
 *  Created on: Feb 5, 2016
 *      Author: ecejnj
 */

#ifndef MAIN_LOG_H_
#define MAIN_LOG_H_


#include <cstdio>
#include <time.h>
#include <string.h>

static const char *getTime(){
    // not thread safe, but anyway
    static char buf[255];
    time_t secsEpoch = time(NULL);

    snprintf( buf, 255, "%s", ctime( &secsEpoch));
    buf[strlen(buf)-1]=0; //Remove '\n'

    return buf;
}

#define TRACE( fmt, s ... ) fprintf( stdout, "%s %s:%s:" fmt "\n"\
        ,getTime(), __FILE__, __PRETTY_FUNCTION__, ## s )
#define DEBUG( fmt, s ... ) fprintf( stdout, "%s %s:%d:" fmt "\n"\
        ,getTime(), __FILE__, __LINE__, ## s )
#define INFO( fmt, s ... ) fprintf( stdout, "%s %s:%d:" fmt "\n"\
        ,getTime(), __FILE__, __LINE__, ## s )
#define ERROR( fmt, s ... ) fprintf( stdout, "%s %s:%d:" fmt "\n"\
        ,getTime(), __FILE__, __LINE__, ## s )


#endif /* MAIN_LOG_H_ */
