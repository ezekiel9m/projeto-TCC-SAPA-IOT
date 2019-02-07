/*
  DS1307.cpp - DS1307 Real-Time Clock library
  Copyright (c) 2011 Álvaro Justen aka Turicas.  All right reserved.
  Modified and updated by Fellipe Couto M. Arruda
  
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

extern "C" {
    #include <inttypes.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
}

#include "DS1307.h"
#include "Wire.h"
int zero = 0;

char *fromNumberToWeekDay(int dayOfWeek, int language) {
	//language = 1 (ENGLISH)  /  language = 2 (PT-BRAZIL)
	if (language == 1){
		switch (dayOfWeek) {
			case 1:
				return "Sunday";
				break;
			case 2:
				return "Monday";
				break;
			case 3:
				return "Tuesday";
				break;
			case 4:
				return "Wednesday";
				break;
			case 5:
				return "Thursday";
				break;
			case 6:
				return "Friday";
				break;
			case 7:
				return "Saturday";
				break;
			default:
				return "";
		}
	}else if (language == 2){
		switch (dayOfWeek) {
			case 1:
				return "Domingo";
				break;
			case 2:
				return "Segunda-feira";
				break;
			case 3:
				return "Terca-feira";
				break;
			case 4:
				return "Quarta-feira";
				break;
			case 5:
				return "Quinta-feira";
				break;
			case 6:
				return "Sexta-feira";
				break;
			case 7:
				return "Sabado";
				break;
			default:
				return "";
		}
	}else{
		return "";
	}
}

char *fromNumberToMonth(int Month, int language) {
	//language = 1 (ENGLISH)  /  language = 2 (PT-BRAZIL)
	if (language == 1){
		switch (Month) {
			case 1:
				return "January";
				break;
			case 2:
				return "February";
				break;
			case 3:
				return "March";
				break;
			case 4:
				return "April";
				break;
			case 5:
				return "May";
				break;
			case 6:
				return "June";
				break;
			case 7:
				return "July";
				break;
			case 8:
				return "August";
				break;
			case 9:
				return "September";
				break;
			case 10:
				return "October";
				break;
			case 11:
				return "November";
				break;
			case 12:
				return "December";
				break;
			default:
				return "";
		}
	}else if (language == 2){
		switch (Month) {
			case 1:
				return "Janeiro";
				break;
			case 2:
				return "Fevereiro";
				break;
			case 3:
				return "Marco";
				break;
			case 4:
				return "Abril";
				break;
			case 5:
				return "Maio";
				break;
			case 6:
				return "Junho";
				break;
			case 7:
				return "Julho";
				break;
			case 8:
				return "Agosto";
				break;
			case 9:
				return "Setembro";
				break;
			case 10:
				return "Outubro";
				break;
			case 11:
				return "Novembro";
				break;
			case 12:
				return "Dezembro";
				break;
			default:
				return "";
		}
	}else{
		return "";
	}
}

uint8_t fromDecimalToBCD(uint8_t decimalValue) {
    return ((decimalValue / 10) * 16) + (decimalValue % 10);
}

uint8_t fromBCDToDecimal(uint8_t BCDValue) {
    return ((BCDValue / 16) * 10) + (BCDValue % 16);
}

void DS1307Class::begin() {
    Wire.begin();
}

void DS1307Class::setDate(uint8_t year, uint8_t month, uint8_t dayOfMonth, uint8_t dayOfWeek, uint8_t hour, uint8_t minute, uint8_t second){
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero); 

    //Start sending the new values
	Wire.write(fromDecimalToBCD(second));     //RTCValues[0]
    Wire.write(fromDecimalToBCD(minute));     //RTCValues[1]
    Wire.write(fromDecimalToBCD(hour));       //RTCValues[2]
    Wire.write(fromDecimalToBCD(dayOfWeek));  //RTCValues[3]
    Wire.write(fromDecimalToBCD(dayOfMonth)); //RTCValues[4]
    Wire.write(fromDecimalToBCD(month));      //RTCValues[5]
    Wire.write(fromDecimalToBCD(year));       //RTCValues[6]

    Wire.write(zero);
    Wire.endTransmission();
}

void DS1307Class::getDate(int *values){
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 7);

    for (int i = 6; i >= 0; i--) {
        values[i] = fromBCDToDecimal(Wire.read());
    }

}

DS1307Class DS1307;
