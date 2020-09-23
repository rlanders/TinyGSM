/*
  Geneva.cpp - Base class that provides access to Geneva modules
  Copyright (c) 2020 Rodney Landers.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include Geneva.h

USBSerial pc(true); // This allows us to have a non-blocking USB serial thingy
FileHandle* mbed::mbed_override_console(int)
{   
    return &pc;
}

UARTSerial modem(MDMTXD, MDMRXD, 115200);

DigitalInOut cellRst(MDMRST);
DigitalInOut cellPwrKey(MDMPWRON);
DigitalOut cellVcc(PC_9, 0);
DigitalOut cellRts(MDMRTS, 0);  // Flow control not supported by 410, but RTS must be low
DigitalIn userBtn(USER_BUTTON);


DigitalOut greenLed(LED_GREEN, 1);
DigitalOut blueLed(LED_BLUE, 1);
DigitalOut redLed(LED_RED, 1);

void doLED(uint8_t mask){
    redLed = ((mask & 4)>>2)^1;
    greenLed = ((mask & 2)>>1)^1;
    blueLed = ((mask & 1))^1; 
}

static void press_power_button(int time_ms)
{
    float time = (float) time_ms / 1000;
    // logger.trace("holding power key for %d mS", time_ms);
    cellPwrKey = 0;
    wait(time);
    cellPwrKey = 1;
}

void initModem(void)
{
    // Power up the modem!
    cellVcc = 1;
    pc.printf("Modem VCC UP\r\n");
    wait(1);

    press_power_button(900);
    wait(1);
    pc.printf("Modem Ready\r\n");
}     

void pdwnModem(void)
{
    if(cellVcc){
        pc.printf("commanding power down\r\n");
        press_power_button(2000);
        wait(1);
        cellVcc = 0;
        pc.printf("Modem off");
    }
}

void initGeneva(void)
{
    userBtn.mode(PullUp);
    cellRst.mode(OpenDrainPullUp);
    cellRst.output();
    cellRst = 1;

    cellPwrKey.mode(OpenDrainPullUp);
    cellPwrKey.output();
    cellPwrKey = 1;
    wait_us(1);
    initModem();
}  