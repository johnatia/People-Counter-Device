# People-Counter-Device
People Counter Device measures the density in the rooms and workplaces using Time of Flight (ToF) technology.

## Setting Configurations using Serial Bluetooth
you can set some configurations using Serial Bluetooth at installation and they will be saved in EEPROM

you can use [Serial Bluetooth Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal) on andriod or [Serial Studio](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal) on PC 

Note: Serial Studio will be useful also for drawing the output signals as shown

After downloading Serial Bluetooth Terminal you can set the following parameters:
Setting People Count = "/4"
Setting Over Density = "/5" in case of display version only
Setting the Wi-Fi = "/6"
Setting the Device ID = "/7"
Setting the Location of installation which could be indoor or outdoor = "/8"
Setting the Threshold = "/9"
All the previous settings switch the device into Config Mode.
So to get back into Active Mode = "/10" , it will start to transfer simple frame consists some numbers which corresponds Front Zone Distance, Back Zone Distance, Threshold, People Count, ToF Signal Rate, the frame will start with /* and end by */ it can be graphed using Serial Studio.
By sending "/10" again That will stop sending the frame but the Active Mode will remain working.
to read the previous data you can send the following string = "/11"

## Over the Air
You can upload new firmware to the device.
You have to connect the People Counter and the PC to the same Wi-Fi network
Send the following string using the Serial Bluetooth = "/12"
then the device will respond you with specific IP address, you can use this IP address in platformio.ini file as shown
'''
upload_port = 192.168.43.127
'''
then build and upload the new firmware and it will load then show success when finish.

https://user-images.githubusercontent.com/55362599/187406659-48200971-6c61-4f46-9c21-a6104f0e11e5.mp4

