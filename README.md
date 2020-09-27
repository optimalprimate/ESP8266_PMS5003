# ESP8266_PMS5003
ESP node for reading air quality data and temperature to MQTT server

Uses a PMS5003 for air quality and a Dallas 18b20 for temperature

Constantly reads the PMS5003 to keep the serial comms alive and then pushes the last value along with temp reading to mqtt every minute or so

In the image, the connection with a resistor on top goes to the temp sensor and the bare copper connector goes to the Air Sensor
