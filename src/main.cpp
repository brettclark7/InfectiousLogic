#include <unistd.h>
#include <iostream>
#include <exception>
#include <fstream>
#include <string.h>

#include "ublox6.h"
#include "lsm303.h"
#include "INIReader.h"

using namespace std;


/* Constants */
enum magnitude {INFO, WARN, CRIT};
const static char* CURR_DIRECTORY = "/home/phr34k/C_Testing/project/bin/";
const static char* LOG_FILENAME = "log.txt";
const static char* CONFIG_FILENAME = "config.ini";
const static char* DATA_FILENAME = ".txt";
const static char* startTime;

/* Globals */
static fstream log_fh;
static fstream data_fh;
static upm::Ublox6* gps_sh;
static upm::LSM303* lsm_sh;
static char dateTime[18];
static INIReader* reader;

/* Skeletons */
int init_log();
int init_config();
int init_dataFile();
int updateDateTime();
char* getDateTime();

int init_gps();
int init_lsm();

int recordDataLine();

int log(char* message, magnitude mode);
int shutdown();

/* Make it hap'n cap'n */
int main()
{
	/*----------------------------------------------------
	             Prep the FileSystem for duty
	-----------------------------------------------------*/
    int rc = 0;

    rc = updateDateTime();
    if (rc == 0)
    {
    	char msg[] = "Failed to get DateTime";
        cerr << msg << rc << endl;
        return rc;
    }
    else
     	startTime = dateTime;

    rc = init_log();
    if (rc > 0)
    {
    	char msg[] = "Failed to initialize log file";
        cerr << msg << endl;
        return rc;
    }

    rc = init_config();
    if (rc > 0)
    {
    	char msg[] = "Failed to initialize config file";
        cerr << msg << endl;
        return rc;
    }

    rc = init_dataFile();
    if (rc > 0)
    {
    	char msg[] = "Failed to initialize data file";
        cerr << msg << endl;
        return rc;
    }

    rc = init_gps();
    if (rc != 0)
    {
    	char msg[] = "Failed to initialize GPS";
        cerr << msg << endl;
        return rc;
    }

    rc = init_lsm();
    if (rc != 0)
    {
    	char msg[] = "Failed to initialize Accelerometer";
        cerr << msg << endl;
        return rc;
    }

    // Log while device is marked as active
    while (reader->GetBoolean("device", "active", true))
    {
    	recordDataLine();
    	// Manual reset of config containing active flag until UI is written
    	init_config();
    }

    //TODO: Sleep when not active rather than shut down
    shutdown();
    return 0;
}

/*
 * Open the log file in append mode.
 * Eventually will need to handle this better by splitting logs
 * into smaller partitions such as by date or journey
 */
int init_log()
{
	char log_file_abs[strlen(CURR_DIRECTORY)+strlen(LOG_FILENAME)] = "";
	strcpy(log_file_abs, CURR_DIRECTORY);
	strcat(log_file_abs, LOG_FILENAME);
	log_fh.open(log_file_abs, std::fstream::app);

	if (!log_fh)
		return 1;

	char msg[] = "Log file initialized";
	log(msg, INFO);
	return 0;
}

/*
 * Read in the configuration file using the INIReader library.
 * File is opened and closed once during instantiation
 */
int init_config()
{
	char config_file_abs[strlen(CURR_DIRECTORY)+strlen(CONFIG_FILENAME)] = "";
	strcpy(config_file_abs, CURR_DIRECTORY);
	strcat(config_file_abs, CONFIG_FILENAME);

	reader = new INIReader(config_file_abs);

	if (reader->ParseError() < 0)
	{
		char msg[] = "Failed to load configuration file";
		log(msg, CRIT);
		return 1;
	}

	//char msg[] = "Config file initialized";
	//log(msg, INFO);
	return 0;
}

/*
 * Open the data file in append mode.
 * Filename = YYYY-MM-DD_HHMMSS.txt
 */
int init_dataFile()
{
	char data_file_abs[strlen(CURR_DIRECTORY)+strlen(DATA_FILENAME)+strlen(startTime)] = "";
	strcpy(data_file_abs, CURR_DIRECTORY);
	strcat(data_file_abs, startTime);
	strcat(data_file_abs, DATA_FILENAME);
	data_fh.open(data_file_abs, std::fstream::app);

	if (!data_fh)
		return 1;

	char msg[] = "Data file initialized";
	log(msg, INFO);
	return 0;
}

/*
 * Populate the dateTime string with the current UTC system time.
 */
int updateDateTime()
{
	time_t t = time(NULL);
	return strftime(dateTime, sizeof(dateTime), "%Y-%m-%d_%H%M%S", localtime(&t));
}

/*
 * Updates the time and returns it all in one.
 */
char* getDateTime()
{
	int rc = 0;
	rc = updateDateTime();
    if (rc == 0)
    {
    	char msg[] = "Failed to update dateTime";
        log(msg, WARN);
    }
	return dateTime;
}

/*
 * Tell the gps to start streaming data to UART.
 */
int init_gps()
{
	gps_sh = new upm::Ublox6(0);

	if (!gps_sh->setupTty(9600))
	{
		char msg[] = "Failed to setup tty port parameters";
		cerr << msg << endl;
		log(msg, CRIT);
		return 1;
	}

	return 0;
}

/*
 * Tell the accelerometer to start streaming data to I2C.
 * Do an initial coordinate call to initialize all values.
 */
int init_lsm()
{
	lsm_sh = new upm::LSM303(0);
	lsm_sh->getCoordinates();
	return 0;
}

/*
 * Write message to log file with given criticality (INFO/WARN/CRIT)
 * CRIT = Failed Hardware (Sensor is unresponsive)
 * WARN = Non-optimal environment (Lack of space causing overwrite)
 * INFO = Helpful debugging (Expected creation/destruction of data files)
 * Function takes care of appending newline before writing.
 */
int log(char* message, magnitude mode)
{
	switch (mode)
	{
		case INFO:
			log_fh << "info: ";
			break;
		case WARN:
			log_fh << "warn: ";
			break;
		case CRIT:
			log_fh << "crit: ";
			break;
	}

	log_fh << getDateTime();
	log_fh << " ";
	log_fh << message;
	log_fh << endl;
	return 0;
}

/*
 * Purge any buffers and close any open files to prevent corruption.
 */
int shutdown()
{
	char msg[] = "Shutting Down";
	log(msg, INFO);
	log_fh.close();
	data_fh.close();
	delete reader;
	return 0;
}

/*
 * Testing Method to generate data immediately
 * Grabs all possible values and writes to data handler
 */
int recordDataLine()
{
	//Update Sensor registers with latest data
	lsm_sh->getAcceleration();
	lsm_sh->getCoordinates();
	lsm_sh->getHeading();

	// Nice to have a line that tells us what we're looking at while debugging
	//data_fh << "YYYY-MM-DD_HHMMSS,Coord_X,Coord_Y,Coord_Z,Heading,Accel_X,Accel_Y,Accel_Z" <<
    //               ",GPS_Feed" <<endl;
	data_fh << getDateTime()
            << "," << lsm_sh->getCoorX()
            << "," << lsm_sh->getCoorY()
			<< "," << lsm_sh->getCoorZ()
			<< "," << lsm_sh->getHeading()
			<< "," << lsm_sh->getAccelX()
			<< "," << lsm_sh->getAccelY()
			<< "," << lsm_sh->getAccelZ()
			<< "," << endl;

    // The buffer is normally 245b (GGA,GSA,RMC,VTG)
    // Sometimes it jumps to ~440b due to receiving 3 GSV sentences
    int bufferLength = 500;
    int testCount = 0;
    bool keepRunning = true;
    char GPGGA[] = "$GPGGA";

    char nmeaBuffer[bufferLength];

    while (keepRunning)
    {
        if (gps_sh->dataAvailable())
        {
        	int num_bytes_read = gps_sh->readData(nmeaBuffer, bufferLength);

        	if (num_bytes_read > 0)
        	{
        		//weak sauce validity check
        		if (strncmp(GPGGA, nmeaBuffer,3) == 0)
        		{
        			keepRunning = false;
				data_fh.write(nmeaBuffer,num_bytes_read);
        		}
        	}

        	else if (num_bytes_read < 0)
            	{
            		char msg[] = "GPS is not reading";
            		log(msg, WARN);
            	}

            	usleep(100000); // 100ms

            	// Attempt for 2s or fail
            	testCount++;
		if (testCount >=20)
			keepRunning = false;
        }
    }
    return 0;
}
