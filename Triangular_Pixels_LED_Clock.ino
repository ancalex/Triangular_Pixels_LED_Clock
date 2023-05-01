/*
 * * ESP8266 template with phone config web page
 * based on ESP 8266 Arduino IDE WebConfig by John Lassen and
 * BVB_WebConfig_OTA_V7 from Andreas Spiess https://github.com/SensorsIot/Internet-of-Things-with-ESP8266
 *
 */
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define LED_PIN 2
#define NUM_LEDS  96
CRGB leds[NUM_LEDS];
uint8 BRIGHTNESS = 200;
int led_list[NUM_LEDS];
int temp_minute = 0;
int temp_second = 0;

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <EEPROM.h>
#include "global.h"
#include "NTP.h"
#include "Color_Palettes.h"
#include "Display_Functions.h"

// Include STYLE and Script "Pages"
#include "Page_Script.js.h"
#include "Page_Style.css.h"

// Include HTML "Pages"
#include "Page_Admin.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_NetworkConfiguration.h"
#include "Page_SetTime.h"

extern "C" {
#include "user_interface.h"

#define UPDATES_PER_SECOND 50
}

void setup() {
	Serial.begin(115200);
	//**** Network Config load
	EEPROM.begin(512); // define an EEPROM space of 512Bytes to store data
	CFG_saved = ReadConfig();

	//  Connect to WiFi acess point or start as Acess point
	if (CFG_saved)  //if no configuration yet saved, load defaults
	{
		// Connect the ESP8266 to local WIFI network in Station mode
		Serial.println("Booting");
		//printConfig();
		WiFi.mode(WIFI_STA);
		WiFi.begin(config.ssid.c_str(), config.password.c_str());
		WIFI_connected = WiFi.waitForConnectResult();
		if (WIFI_connected != WL_CONNECTED)
			Serial.println("Connection Failed! activating the AP mode...");

		Serial.print("Wifi ip:");
		Serial.println(WiFi.localIP());
	}

	if ((WIFI_connected != WL_CONNECTED) or !CFG_saved) {
		// DEFAULT CONFIG
		Serial.println("Setting AP mode default parameters");
		config.ssid = "TPLedClock"; // SSID of access point
		config.password = "asdfg12345";
		config.dhcp = true;
		config.IP[0] = 192;
		config.IP[1] = 168;
		config.IP[2] = 1;
		config.IP[3] = 100;
		config.Netmask[0] = 255;
		config.Netmask[1] = 255;
		config.Netmask[2] = 255;
		config.Netmask[3] = 0;
		config.Gateway[0] = 192;
		config.Gateway[1] = 168;
		config.Gateway[2] = 1;
		config.Gateway[3] = 254;
		config.DeviceName = "Triangular Pixels LED Clock";
		config.ntpServerName = "0.europe.pool.ntp.org"; // to be adjusted to PT ntp.ist.utl.pt
		config.Update_Time_Via_NTP_Every = 3;
		config.timeZone = 20;
		config.isDayLightSaving = true;
		WriteConfig();
		WiFi.mode(WIFI_AP);
		WiFi.softAP(config.ssid.c_str(),"admin1234");
		Serial.print("Wifi ip:");
		Serial.println(WiFi.softAPIP());
	}

	// Start HTTP Server for configuration
	server.on("/", []() {
		Serial.println("admin.html");
		server.send_P ( 200, "text/html", PAGE_AdminMainPage); // const char top of page
	});

	server.on("/favicon.ico", []() {
		Serial.println("favicon.ico");
		server.send( 200, "text/html", "" );
	});
	// Network config
	server.on("/config.html", send_network_configuration_html);
	// Info Page
	server.on("/info.html", []() {
		Serial.println("info.html");
		server.send_P ( 200, "text/html", PAGE_Information );
	});
	server.on("/ntp.html", send_NTP_configuration_html);
	server.on("/time.html", send_Time_Set_html);
	server.on("/style.css", []() {
		Serial.println("style.css");
		server.send_P ( 200, "text/plain", PAGE_Style_css );
	});
	server.on("/microajax.js", []() {
		Serial.println("microajax.js");
		server.send_P ( 200, "text/plain", PAGE_microajax_js );
	});
	server.on("/admin/values", send_network_configuration_values_html);
	server.on("/admin/connectionstate", send_connection_state_values_html);
	server.on("/admin/infovalues", send_information_values_html);
	server.on("/admin/ntpvalues", send_NTP_configuration_values_html);
	server.on("/admin/timevalues", send_Time_Set_values_html);
	server.onNotFound([]() {
		Serial.println("Page Not Found");
		server.send ( 400, "text/html", "Page not Found" );
	});
	server.begin();
	Serial.println("HTTP server started");

	printConfig();

	// start internal time update ISR
	tkSecond.attach(1, ISRsecondTick);

	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
	FastLED.setBrightness(BRIGHTNESS);
	Serial.println("FastLed Setup done");

	// start internal time update ISR
	tkSecond.attach(1, ISRsecondTick);
}

// the loop function runs over and over again forever
void loop() {
	server.handleClient();
	if (config.Update_Time_Via_NTP_Every > 0) {
		if (cNTP_Update > 5 && firstStart) {
			getNTPtime();
			delay(1500); //wait for DateTime
			cNTP_Update = 0;
			firstStart = false;
		}
		else if (cNTP_Update > (config.Update_Time_Via_NTP_Every * 60)) {
			getNTPtime();
			cNTP_Update = 0;
		}
	}
	//  feed de DOG :)
	customWatchdog = millis();

	//============================
	if (WIFI_connected != WL_CONNECTED and manual_time_set == false) {
		config.Update_Time_Via_NTP_Every = 0;
		//display_led_no_wifi
		softtwinkles();
		FastLED.show();
	} else if (ntp_response_ok == false and manual_time_set == false) {
		config.Update_Time_Via_NTP_Every = 1;
		//display_animation_no_ntp
		pride();
		FastLED.show();
	} else if (ntp_response_ok == true or manual_time_set == true) {
		if (temp_minute != DateTime.minute or temp_minute == 0) {
			temp_minute = DateTime.minute;
			random16_add_entropy(analogRead(0));
			int BgIndex = random8(BgGradientPaletteCount);
			int TimeIndex = random8(TimeGradientPaletteCount);
			targetPaletteBg = BgGradientPalettes[BgIndex];
			targetPaletteTime = TimeGradientPalettes[TimeIndex];
			Serial.println("minute");
			Serial.print(BgIndex);Serial.print("-");Serial.println(BgPaletteList[BgIndex]);
			Serial.print(TimeIndex);Serial.print("-");Serial.println(TimePaletteList[TimeIndex]);
			TimeInit();
		}
		uint8_t maxChanges = 24;
		nblendPaletteTowardPalette( currentPaletteBg, targetPaletteBg, maxChanges);
		nblendPaletteTowardPalette( currentPaletteTime, targetPaletteTime, maxChanges);

		static uint8_t startIndex = 0;
		startIndex = startIndex + 1; /* motion speed */
		TimeDisplay( startIndex );

		FastLED.show();
		FastLED.delay(1000 / UPDATES_PER_SECOND);
	}
}
