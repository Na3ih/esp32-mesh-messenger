/**
  @brief  Implementation of a mesh network messenger using the ESP-NOW protocol. 
          Messages received via Serial are broadcast across the mesh network. 
          Network configuration is defined in the mesh_config file. 
  
  @note Based on example made by Rui Santos (Random Nerd Tutorial): https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino

  @todo:
  - unit tests
  - check if possible add webserver (chat in js, dns)   
  */

#include <Arduino.h>

#include "painlessMesh.h"
#include "mesh_config.h"
#include "pinout.h"

/*************************************************************************** Defines and Enums ****************************************************************************/
/**
 * @defgroup defines_and_enums Defines and Enums
 * @{
 */
#define LOG_ERROR(...) Serial.printf("ERROR: " __VA_ARGS__); Serial.println()
#define LOG_INFO(...) Serial.printf("INFO: " __VA_ARGS__); Serial.println()

/** @} defines_and_enums */ 

/************************************************************************** Static API declaration ****************************************************************************/
/**
 * @defgroup static_api Static api declaration
 * @{
 */
static void checkSerial(void);
static void sendMessage(void);
static void blinkNodes(void);

static void receivedCallback(uint32_t from, String &msg);
static void newConnectionCallback(uint32_t nodeId);
static void changedConnectionCallback(void);
/** @} static_api */

/*************************************************************************** Global variables ****************************************************************************/
/**
 * @defgroup global_variables Global variables
 * @{
 */
Scheduler userScheduler;
painlessMesh  mesh;

/**
 * @brief Task using internal LED to indicate the number of nodes in the network every 3 seconds
 */
Task taskBlinkNodes(TASK_SECOND * 3, 1, &blinkNodes);

/**
 * @brief Task using to check if there is a data to send available on Serial port
 */
Task taskCheckSerial(TASK_MILLISECOND * 250, 1, &checkSerial);

/**
 * @brief Variable to control built-in LED to indicate the number of nodes in the network 
 */
static volatile bool onFlag = true;

/**
 * @brief Variable to enable/disable additional debug logs on Serial
 */
bool debugEnabled = false;

/**
 * @brief Macro to enable additional debug logs and function
 * @info: Debug is enabled by default. To disable pull down the DEBUG_PIN defined in pinout.h
 */
#define LOG_DEBUG(...) \
  if (debugEnabled) { Serial.printf("DEBUG: " __VA_ARGS__); Serial.println(); }

/** @} global_variables */

/*************************************************************************** Main function ****************************************************************************/
/**
 * @defgroup main_function Main function
 * @{
 */
void setup() {
  Serial.begin(115200);
  
  mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  userScheduler.addTask(taskBlinkNodes);
  taskBlinkNodes.enable();

  userScheduler.addTask(taskCheckSerial);
  taskCheckSerial.enable();

  pinMode(LED_PIN, OUTPUT);
  pinMode(DEBUG_PIN, INPUT_PULLUP);

  if (HIGH == digitalRead(DEBUG_PIN)) {
    debugEnabled = true;
  }

  LOG_DEBUG("Setup completed, debugEnabled = %b", debugEnabled);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  digitalWrite(LED_PIN, onFlag);
}
/** @} main_function */

/*************************************************************************** Static API definitions ****************************************************************************/
/**
 * @defgroup static_api_defs Static APIs
 * @{
 */

 /**
 * @brief Read string from Serial (if available) and send as broadcast message
 * @note  Trigered 4/s
 */
static void checkSerial(void) {
      if (Serial.available() != 0) {
        sendMessage();
      }
      taskCheckSerial.setIterations(1);
      taskCheckSerial.setInterval(TASK_MILLISECOND * 250); 
};

/**
 * @brief Indicates the number of nodes by blinking the built-in LED. (Called every 3 seconds by taskBlinkNodes)
 * 
 */
static void blinkNodes(void) {
      onFlag = !onFlag; 
      taskBlinkNodes.delay(BLINK_DURATION_MS);

      if (taskBlinkNodes.isLastIteration()) {
        taskBlinkNodes.setIterations(((mesh.getNodeList().size() + 1) * 2));
        taskBlinkNodes.setInterval(BLINK_PERIOD_MS); 
      }
};

/**
 * @brief Read string from Serial and send in broadcast message
 * @note  Max text length is set to 260 Bytes
 */
static void sendMessage(void)
{
    char text[260];
    Serial.readBytesUntil('\n', text, 260);
    StaticJsonDocument<300> jsonDoc; // add dynamic size of msg
    jsonDoc["sender"] = NODE_ID;
    jsonDoc["text"] = text;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    int ret = mesh.sendBroadcast(jsonString);
    if (ret != 1) {
      LOG_ERROR("sendMessage: send broadcast msg: size %d, returned %d\n", jsonString.length(), ret);
    }
    Serial.printf("MSG sent [%dB]: ", jsonString.length());
    Serial.println(text);
    LOG_DEBUG("sendMessage: send broadcast msg: size %d, returned %d\n", jsonString.length(), ret);
}

/**
 * @brief Prints the list of currently connected nodes when it changes.
 * @note  Currently for debug purposes only
 */
static void changedConnectionCallback() {
  SimpleList<uint32_t> nodes;
  nodes = mesh.getNodeList();
  
  LOG_DEBUG("changedConnectionCallback: num modes: %d, connecion list:", nodes.size());

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    LOG_DEBUG(" %u", *node);
    node++;
  }
}

/**
 * @brief Function called after receiving a packet
 * 
 * @param from sender MAC address
 * @param msg received packet in String format
 */
static void receivedCallback( uint32_t from, String &msg ) {
  StaticJsonDocument<500> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, msg);

  if (!error) {
    String sender = jsonDoc["sender"];
    String text = jsonDoc["text"];
    Serial.printf("MSG Rx [%dB]: from: %s, text: ", text.length(), sender, text);
    Serial.println(text);
  } else {
    LOG_ERROR("receivedCallback: Failed to parse JSON from: %s, error: %d", from, error);
  }
}

/**
 * @brief Function called after new neighbour node joined a network
 * 
 * @param nodeId new neighbour MAC address
 */
static void newConnectionCallback(uint32_t nodeId) {
    LOG_INFO("newConnectionCallback: New Connection, nodeId = %u", nodeId);
}

/** @} static_api_defs */
