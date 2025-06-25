#include "mbed.h"
#include "Sht31.h"


#define MQTTCLIENT_QOS2 1
 
#include "MQTTmbed.h"
#include "MQTTClientMbedOs.h"
float t;
float h;
int t1;
int h1;
DigitalOut led(D13);
Sht31   temp_sensor(D14, D15);


 
 AnalogIn my_adc(PA_7); //D11 on board

int arrivedcount = 0;
int x;

 
void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}
 
void mqtt_demo(NetworkInterface *net)
{
    float version = 0.6;
    char* topic = "water";
    char* topic1 = "temp";
    char* topic2 = "hum";
    TCPSocket socket;
    MQTTClient client(&socket);
 
    SocketAddress a;
    char* hostname = "dev.rightech.io";
    net->gethostbyname(hostname, &a);
    int port = 1883;
    a.set_port(port);
 
    printf("Connecting to %s:%d\r\n", hostname, port);
 
    socket.open(net);
    printf("Opened socket\n\r");
    int rc = socket.connect(a);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);
    printf("Connected socket\n\r");
 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "alsu4538-kxq54o";
    data.username.cstring = "Aldar";
    data.password.cstring = "123";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);
 
    if ((rc = client.subscribe(topic, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);
 
    MQTT::Message message;
 
    // QoS 0
    //x = round(my_adc.read()*100);
    char buf[100];
    sprintf(buf, "%d", x);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    ThisThread::sleep_for(100);
    //while (arrivedcount < 1)
       // client.yield(200); 

    sprintf(buf, "%d", t1);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic1, message);
    ThisThread::sleep_for(100);
    //while (arrivedcount < 1)
        //client.yield(200);

    sprintf(buf, "%d", h1);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic2, message);
    ThisThread::sleep_for(100);
    while (arrivedcount < 1)
        client.yield(200);
 /*
    // QoS 1
    sprintf(buf, "%d", x);
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (arrivedcount < 2)
        client.yield(200);
 
    while (arrivedcount < 3)
        client.yield(200);
    

 
    // QoS 1
    sprintf(buf, "%d", t1);
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic1, message);
    while (arrivedcount < 2)
        client.yield(200);
 
    while (arrivedcount < 3)
        client.yield(200);


 
    // QoS 1
    sprintf(buf, "%d", h1);
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic2, message);
    while (arrivedcount < 2)
        client.yield(200);
 
    while (arrivedcount < 3)
        client.yield(200);*/
 
    if ((rc = client.unsubscribe(topic)) != 0)
        printf("rc from unsubscribe was %d\r\n", rc);
 
    if ((rc = client.disconnect()) != 0)
        printf("rc from disconnect was %d\r\n", rc);
 
    socket.close();
 
    printf("Version %.2f: finish %d msgs\r\n", version, arrivedcount);
 
    return;
}


WiFiInterface *wifi;

const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    printf("Scan:\n");

    int count = wifi->scan(NULL,0);

    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);

    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\n", count);

    delete[] ap;
    return count;
}

int main()
{ 
    
    printf("\nSTM32 ADC example\n");
 x = round(my_adc.read()*100); 
printf("ADC read = %d\n\r", x);



  t = temp_sensor.readTemperature();
        h = temp_sensor.readHumidity();
        t1=t;
        h1 =h;
        if (x>60) {
            printf ("suxo\n");
            
            led = 1;
        }
        else  {
        printf ("mokro\n");
            
            led = 0;
        }
printf("%d", t1);
printf(" %d\n\r", h1);
ThisThread::sleep_for(500);






    printf("WiFi example\n");

#ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    int count = scan_demo(wifi);
    if (count == 0) {
        printf("No WIFI APs found - can't continue further.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi->get_mac_address());
    printf("IP: %s\n", wifi->get_ip_address());
    printf("Netmask: %s\n", wifi->get_netmask());
    printf("Gateway: %s\n", wifi->get_gateway());
    printf("RSSI: %d\n\n", wifi->get_rssi());

    mqtt_demo(wifi);
    //wifi->disconnect();

    printf("\nDone\n");
    //while (1) {
      //  x = (my_adc.read()*100);

        //ThisThread::sleep_for(1000);

    //}
}