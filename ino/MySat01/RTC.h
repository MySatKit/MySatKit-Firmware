#pragma once

#include <Wire.h> 
#include <RtcDS3231.h>
#include "I2C.h"

static RtcDS3231<TwoWire> Rtc(getI2C());

int initRTC(void)
{
    Rtc.Begin();
}

RtcDateTime rtcGetTime()
{
    return Rtc.GetDateTime();
}

RtcTemperature rtcGetTemperature()
{
    return Rtc.GetTemperature();
}