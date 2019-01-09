/*
 Name:		Xinstruments.ino
 Created:	3/30/2018 6:41:46 PM
 Author:	Frank
*/
// ------------------------------------------------------------------------------------------------------------------ -
//	 Basic Instrument Control
//   Franks Flightsim Intruments project
//	by Frank van Ierland
//
//	This code is in the public domain.
//
//	VERSION HISTORY:
//
//
//-------------------------------------------------------------------------------------------------------------------
// TODO: move setup to eeprom
// TODO: OTA software update

#undef CONFIG_AUTOSTART_ARDUINO
static const char *TAG = "Instruments";
#define LOG_LOCAL_LEVEL 5

#include <stdlib.h>
#include "Xinstruments.h"

#include "Instrument.h"
#include "esp_event_loop.h"

//#include "ICanInstrument.h"
//#include <ICanBase.h>

//generic includes for used libs
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <QList.h>
#include <esp32_can.h>
#include "EEPROM.h"
#include "esp_log.h"

Instrument*	 myInstrument = NULL;
EventGroupHandle_t s_connection_event_group;
QueueHandle_t xQueueRREF = NULL;	// queu for results of data items from Xplane
QueueHandle_t xQueueDREF = NULL;	// queue for data items to send to XPlane
QueueHandle_t xQueueDataSet = NULL;  // queue to  request of stop dataitems in XPlane interface
//===================================================================================================================
// MAIN SETUP PROC
//===================================================================================================================

void mySetup()
{
	// define all needed inter task queues;
	xQueueRREF = xQueueCreate(20, sizeof(queueDataItem));
	assert(xQueueRREF != NULL);
	xQueueDREF = xQueueCreate(20, sizeof(queueDataItem));
	assert(xQueueDREF != NULL);
	xQueueDataSet = xQueueCreate(50, sizeof(queueDataSetItem));
	assert(xQueueDataSet != NULL);
	s_connection_event_group = xEventGroupCreate();

	// start communication with master
	ESP_EARLY_LOGI(TAG, "Starting");
	myInstrument = new Instrument("eeprom");
	ESP_EARLY_LOGI(TAG, "Instrument initiated");

	// set up can bus communication
	//myInstrument->_initiateCommunication();
}

//===================================================================================================================
// MAIN SETUP PROC
//===================================================================================================================

extern "C" void app_main()
{
	// start up serial interface
	Serial.begin(115200);
	//Serial.setDebugOutput(true);
	delay(1000); // make sure Serial is initialized
	ESP_EARLY_LOGE(TAG, "Loglevel=%d", LOG_LOCAL_LEVEL);
	//vprintf("TEST OUTPUT %s", "test");

	esp_log_level_set("*", ESP_LOG_VERBOSE);
	esp_log_level_set("ICanService", ESP_LOG_INFO);
	esp_log_level_set("ICanBase", ESP_LOG_INFO);
	esp_log_level_set("ICanInstrument", ESP_LOG_INFO);
	esp_log_level_set("ICanDriver", ESP_LOG_INFO);
	esp_log_level_set("ICanBaseSrv", ESP_LOG_INFO);

	//esp_task_wdt_init(254, false);
	esp_task_wdt_deinit();

	mySetup();

#ifdef DEBUG_CLI
	commandLine.update();
#endif

#ifdef USE_PWR_FLAG_SERVO
	_UpdateFlagStatus();
#endif
	ESP_EARLY_LOGI(TAG, "Starting Instrument");
	assert(myInstrument != NULL);
	myInstrument->start(1);

	ESP_EARLY_LOGI(TAG, "Run State entered");

	for (;;)
	{
		esp_task_wdt_reset(); delay(1000);
	}
}