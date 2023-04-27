//This example demonstrates the ESP RainMaker with a standard Switch device.
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"

#define DEFAULT_POWER_MODE true
const char *service_name = "PROV_1234";
const char *pop = "abcd1234";

//GPIO for push button
//GPIO for virtual device
static int gpio_0 = 0;
static int switch_1 = 2;
static int switch_2 = 23;
static int switch_3 = 22;
static int switch_4 = 21;
static int ps1 = 34;  //pysical switch
static int ps2 = 35;
static int ps3 = 32;


/* Variable for reading pin status*/
bool switch_state = true;
bool ini1 = false;
bool ini2 = false;
bool ini3 = true;
bool ini4 = false;



//The framework provides some standard device types like switch, lightbulb, fan, temperaturesensor.
static Switch my_switch1;
static Switch my_switch2;
static Switch my_switch3;
static Switch my_switch4;

//wifi reconnection function
void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    default:
      break;
  }
}

void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32S2
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
      printQR(service_name, pop, "softap");
#else
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
#endif
      break;
    default:;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();


  if (strcmp(device_name, "switch1"))
    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state = val.val.b;
      (switch_state == false) ? digitalWrite(switch_1, LOW) : digitalWrite(switch_1, HIGH);
      param->updateAndReport(val);
    }

    else if (strcmp(device_name, "switch2"))
      if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        switch_state = val.val.b;
        (switch_state == false) ? digitalWrite(switch_2, LOW) : digitalWrite(switch_2, HIGH);
        param->updateAndReport(val);
      }

      else if (strcmp(device_name, "switch3"))
        if (strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
          switch_state = val.val.b;
          (switch_state == false) ? digitalWrite(switch_3, LOW) : digitalWrite(switch_3, HIGH);
          param->updateAndReport(val);
        }

        else if (strcmp(device_name, "switch4"))
          if (strcmp(param_name, "Power") == 0) {
            Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
            switch_state = val.val.b;
            (switch_state == false) ? digitalWrite(switch_4, LOW) : digitalWrite(switch_4, HIGH);
            param->updateAndReport(val);
          }
}

void enableee() {
  Node my_node;
  my_node = RMaker.initNode("ESP RainMaker Node");

  //Initialize switch device
  my_switch1 = Switch("Switch1", &switch_1);
  my_switch2 = Switch("Switch2", &switch_2);
  my_switch3 = Switch("Switch3", &switch_3);
  my_switch4 = Switch("Switch4", &switch_4);
  //Standard switch device
  my_switch1.addCb(write_callback);
  my_switch2.addCb(write_callback);
  my_switch3.addCb(write_callback);
  my_switch4.addCb(write_callback);
  //Add switch device to the node
  my_node.addDevice(my_switch1);
  my_node.addDevice(my_switch2);
  if (ini3)
    my_node.addDevice(my_switch3);
  if (ini4)
    my_node.addDevice(my_switch4);
}

void setup() {
  Serial.begin(115200);
  pinMode(gpio_0, INPUT);
  pinMode(ps1, INPUT);
  pinMode(ps2, INPUT);
  pinMode(ps3, INPUT);

  pinMode(switch_1, OUTPUT);
  pinMode(switch_2, OUTPUT);
  pinMode(switch_3, OUTPUT);
  pinMode(switch_4, OUTPUT);
  WiFi.onEvent(WiFiEvent);

  digitalWrite(switch_1, DEFAULT_POWER_MODE);

  enableee();

  //This is optional
  RMaker.enableOTA(OTA_USING_PARAMS);
  //If you want to enable scheduling, set time zone for your region using setTimeZone().
  //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
  // RMaker.setTimeZone("Asia/Shanghai");
  // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
  RMaker.enableTZService();
  RMaker.setTimeZone();

  RMaker.enableSchedule();

  RMaker.start();

  WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32S2
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#else
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#endif
}

void loop() {

  if (digitalRead(gpio_0) == LOW) {  //Push button pressed

    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_0) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    } else {
      // Toggle device state
      switch_state = !switch_state;
      Serial.printf("Toggle State to %s.\n", switch_state ? "true" : "false");
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state);
      (switch_state == false) ? digitalWrite(switch_1, LOW) : digitalWrite(switch_1, HIGH);
    }
  }
  delay(100);
}
