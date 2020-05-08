

#include "painlessMesh.h"
#include "DHT.h"


#define DHTPIN 0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

bool cold=0;
bool hot=0;

#define MESHNAME "Wi-Fi"
#define MESHPWD "01234567"
#define MESHPORT 5555

Scheduler userScheduler;
painlessMesh mesh;
void sendMessage();
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage()
{
  float t = dht.readTemperature();
  if(t<30){
    cold=1;
    hot=0;
    }
   else {hot=1;
   cold=0;}
  // Serializing in JSON Format
  DynamicJsonDocument doc(1024);  
  doc["RELAY1"]=cold;
  doc["RELAY2"]=hot; 
 
  String msg ;
  serializeJson(doc, msg);
  mesh.sendBroadcast( msg );
  Serial.println("From node1");
  Serial.println(msg);
  taskSendMessage.setInterval((TASK_SECOND * 10));
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}




void setup() {
  Serial.begin(115200);
  Serial.println("Node 1");
  dht.begin();

  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESHNAME, MESHPWD, &userScheduler, MESHPORT );  
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  }

void loop() {
   mesh.update();  

}
