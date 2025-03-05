# PROYECTO1_DIGITAL2
Red de Sensores

	PINOUT
     Las siguientes figuras contienen la manera en la que los pines se conectan entre sí, Incluyendo los pines de los 3 Arduino, el ESP32, el convertidor bidireccional y los sensores. Para los Arduino esclavos aparece su dirección en hexadecimal para la comunicación I2C a la par de su respectivo nombre.

 
Figura 1. Pinout del Arduino maestro.







 
Figura 2. Pinout de Arduino esclavo con sensor de lluvia.

 }
Figura 2. Pinout de Arduino esclavo con sensor de humo.

 
Figura 3. Pinout de ESP32.

 
Figura 4. Pinout de sensores

 
Figura 5. Pinout de conversor bidireccional.

	MÓDULOS
	Arduino Maestro
Para el Arduino maestro se implementaron los siguientes módulos:
	UART 9600: Para este módulo se implementó la comunicación con 9600 de baudrate hacia el ESP32, en este modulo se enviaron cada uno de los datos recibidos de los sensores y el estado de cada motor hacia el mismo. Al tener cada dato ya calculado y definido cuando se encendía o se apagaba cada motor los datos se enviaban utilizando los comandos “T” para temperatura, “B” para sensor de humo, “w:rain” o “w:day-sunny” según el clima con “S” como comando de aviso. “V” para el servo de la ventana y “D” para el DC del ventilador.
	I2C: Para el módulo I2C se creó una función para cada uno de los sensores, este pide los datos de cada sensor, y al convertir cada valor con sus fórmulas respectivas en el main envía los comandos (en minúsculas para apagar y mayúsculas para encender) ”H” para el buzzer de alarma de humo, “V” para el DC de ventilador y “S” para el servo de la ventana.
	LCD: Para el módulo de la LCD Se convertían los datos del sensor de humo y temperatura a valores string y poder enseñarlos en sus espacios correspondientes, por otro lado para el sensor de lluvia se crearon caracteres especiales para que al determinar si esta lloviendo o no se mostrar un sol o gotas de lluvia de manera correspondiente.
	ESP32
Para el ESP32 se implementaron los siguientes módulos:
	UART2 9600: Para el módulo de UART con RX2 y TX2 se recibió cada uno de los valores que se recibieron del Arduino maestro según los comandos mencionados anteriormente, convirtiendo los valores de humo y temperatura a float.
	WI-FI: En este módulo se conecta el ESP32 a wifi con el config.h y al usuario de Adafruit con sus respectivas credenciales. Al tener Adafruit correctamente conectado con los feeds respectivos, se envían los valores ya listos por el módulo de UART.
	UART1 11500: Usando la comunicación UART conectada al cable de alimentación con un baudrate de 11500 se monitorea que toda la información recibida pr medio del UART2 sea correcta para enviarla posteriormente con el módulo de Wi-Fi.
	Arduino con sensor de Humo
Para este Arduino esclavo se implementaron los siguientes módulos:
	I2C: En este Arduino se recibieron los valores correspondientes de comandos enviados por el Arduino maestro acerca del DC para el ventilador y el buzzer y encender sus salidas digitales de manera correspondiente al ser el comando una mayúscula. También con este módulo se enviaron los valores del sensor de humo cuando se requerían según el maestro.
	ADC: Con el ADC se convirtieron los valores analógicos recibidos por el sensor de humo a valores digitales para poder ser luego enviados por la comunicación I2C.
	Arduino con sensor de Lluvia
Para este Arduino esclavo se implementaron los mismos módulos que el anterior, pero también se utilizó el siguiente módulo:
	PWM0: Se utilizo el módulo PWM0 para poder enviar la señal correspondiente al servo de 0 a 90 grados utilizando una división por 6 para que los valores de 0 a 255 fuera mapeado de manera correcta según la señal funcional de un servo. En este módulo no se utilizó un DC ni un buzzer.

	DATASHEETS
	Sensor de Humo: 605-00008-MQ-2-Datasheet-370464.pdf
	Sensor de Temperatura: Aosong_AHT10_en_draft_0c.pdf

	CÁLCULOS
	Temperatura
Temperatura=((ValorAnalógico*200)/1048576.0)-50
	Concentración de Humo
Concentración(kppm)=  (50/(5/ValorAnalógico))^2/1000

	GITHUB
