//===================================================================================

#include "main.h"
#include "sql.h"
#include "http.h"
#include "calendar.h"
#include "pi_2_dht_read.h"

const char *sql1 =
"CREATE TABLE IF NOT EXISTS Meas ("
	"year INTEGER,"
	"month INTEGER,"
	"day INTEGER,"
	"hour INTEGER,"
	"minute INTEGER,"
	"week INTEGER,"
	"sensor_id INTEGER,"
	"value REAL,"
	"meas_id INTEGER PRIMARY KEY"
");";


//===================================================================================

typedef struct {
	float value;
	int sensor_id;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int week;
} meas_t;


meas_t meas[10];
int n_meas = 0;


void flush()
{
char str[256] = {0};
int i;
    if(!n_meas) return;
    if(!dbx_query("begin transaction;")) {
	printf("flush - dbx_query failed\n");
    }
    for(i=0; i < n_meas; i++) {
    	snprintf(str,sizeof(str),"INSERT INTO Meas (year,month,day,hour,minute,week,sensor_id,value) VALUES "
		"(%d,%d,%d, %d,%d,%d, %d,%3.1f);",
		meas[i].year,meas[i].month,meas[i].day,
		meas[i].hour,meas[i].minute,meas[i].week,
		meas[i].sensor_id,meas[i].value);
	if(!dbx_query(str)) {
		printf("flush - dbx_query failed2\n");
	}
    }
    if(!dbx_query("commit;")) {
    	printf("flush - dbx_query failed3\n");
    }
    n_meas = 0;
}


void push(float val,int sid,int hour,int minute,int day,int month,int year)
{
int week = weekNumber(day,month,year);
    printf("sid: %d val: %3.1f %02d:%02d %02d-%02d-%04d wk %d\n",sid,val,hour,minute,day,month,year,week);

    meas[n_meas].value = val;
    meas[n_meas].sensor_id = sid;
    meas[n_meas].year = year;
    meas[n_meas].month = month;		//1..12
    meas[n_meas].day = day;		//1..31
    meas[n_meas].hour = hour;		//0..23
    meas[n_meas].minute = minute;	//0..59
    meas[n_meas].week = week;

    if(++n_meas >= NELEM(meas)) {
	flush();
    }
}


void test_data()
{
int d,m,h;
//    for(m=1; m <= 12; m++) {
	m = 11;
        for(d=14; d <= 20; d++) {
	    for(h=0; h < 24; h++) {
    		float temp = m+d+h;
		push(temp,0, h,30, d,m,2016);
	    }
	}
//    }
}

//===================================================================================

int cnt = 0;

void get_meas()
{
float temp = 0;
float hum = 0;

    if(++cnt < 60) return;
    cnt = 0;
#if 0
    if(pi_2_dht_read(DHT22, 4, &hum, &temp) != DHT_SUCCESS) {
        printf("can\'t read DHT22\n");
	return;
    }
#else
    hum = 20;
    temp = 30;
#endif

    time_t t = time (NULL);
    struct tm *timeinfo = localtime (&t);

    push(temp,0,timeinfo->tm_hour,timeinfo->tm_min, timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900);
    push(hum, 1,timeinfo->tm_hour,timeinfo->tm_min, timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900);
    flush();
}

//===================================================================================

int main(int argc, char* argv[])
{
	dbx_init();
	if(!dbx_query(sql1)) {
		printf("dbx_query failed\n");
		goto exit;
	}
//	test_data();
	start_server();

	for( ; ; ) {
	    get_meas();
    	    usleep(1000*1000);
	}
exit:
	stop_server();
	dbx_close();
	return 0;
}

//===================================================================================
