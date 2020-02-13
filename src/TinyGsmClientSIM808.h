/**
 * @file     TinyGsmClientSIM808.h
 * @author   Volodymyr Shymanskyy
 * @license  LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date     Nov 2016
 */

#ifndef SRC_TINYGSMCLIENTSIM808_H_
#define SRC_TINYGSMCLIENTSIM808_H_
// #pragma message("TinyGSM:  TinyGsmClientSIM808")

#include "TinyGsmClientSIM800.h"
#include "TinyGsmGPS.tpp"

class TinyGsmSim808 : public TinyGsmSim800, public TinyGsmGPS<TinyGsmSim808> {
  friend class TinyGsmGPS<TinyGsmSim808>;

 public:
  explicit TinyGsmSim808(Stream& stream) : TinyGsmSim800(stream) {}


  /*
   * GPS location functions
   */
 protected:
  // enable GPS
  bool enableGPSImpl() {
    // uint16_t state;

    sendAT(GF("+CGNSPWR=1"));
    if (waitResponse() != 1) { return false; }

    return true;
  }

  bool disableGPSImpl() {
    // uint16_t state;

    sendAT(GF("+CGNSPWR=0"));
    if (waitResponse() != 1) { return false; }

    return true;
  }

  // get the RAW GPS output
  // works only with ans SIM808 V2
  String getGPSrawImpl() {
    sendAT(GF("+CGNSINF"));
    if (waitResponse(GF(GSM_NL "+CGNSINF:")) != 1) { return ""; }
    String res = stream.readStringUntil('\n');
    waitResponse();
    res.trim();
    return res;
  }

  // get GPS informations
  // works only with ans SIM808 V2
  bool getGPSImpl(float* lat, float* lon, float* speed = 0, int* alt = 0,
                  int* vsat = 0, int* usat = 0, int* year = 0, int* month = 0,
                  int* day = 0, int* hour = 0, int* minute = 0,
                  int* second = 0) {
    bool fix = false;

    sendAT(GF("+CGNSINF"));
    if (waitResponse(GF(GSM_NL "+CGNSINF:")) != 1) { return false; }

    streamSkipUntil(',');                    // GNSS run status
    if (streamGetInt(',') == 1) fix = true;  // fix status

    // UTC date & Time
    char dtSBuff[7] = {'\0'};
    stream.readBytes(dtSBuff, 4);             // Four digit year
    dtSBuff[4] = '\0';                        // null terminate buffer
    if (year != NULL) *year = atoi(dtSBuff);  // Convert to int

    stream.readBytes(dtSBuff, 2);  // Two digit month
    dtSBuff[2] = '\0';
    if (month != NULL) *month = atoi(dtSBuff);

    stream.readBytes(dtSBuff, 2);  // Two digit day
    dtSBuff[2] = '\0';
    if (day != NULL) *day = atoi(dtSBuff);

    stream.readBytes(dtSBuff, 2);  // Two digit hour
    dtSBuff[2] = '\0';
    if (hour != NULL) *hour = atoi(dtSBuff);

    stream.readBytes(dtSBuff, 2);  // Two digit minute
    dtSBuff[2] = '\0';
    if (minute != NULL) *minute = atoi(dtSBuff);

    stream.readBytes(dtSBuff, 6);  // 6 digit second with subseconds
    dtSBuff[6] = '\0';
    if (second != NULL) *second = atoi(dtSBuff);
    // *secondWithSS = atof(dtSBuff);
    streamSkipUntil(',');  // Throw away the final comma

    *lat = streamGetFloat(',');                       // Latitude
    *lon = streamGetFloat(',');                       // Longitude
    if (alt != NULL) *alt = streamGetFloat(',');      // MSL Altitude
    if (speed != NULL) *speed = streamGetFloat(',');  // Speed Over Ground
    streamSkipUntil(',');                             // Course Over Ground
    streamSkipUntil(',');                             // Fix Mode
    streamSkipUntil(',');                             // Reserved1
    streamSkipUntil(',');  // Horizontal Dilution Of Precision
    streamSkipUntil(',');  // Position Dilution Of Precision
    streamSkipUntil(',');  // Vertical Dilution Of Precision
    streamSkipUntil(',');  // Reserved2
    if (vsat != NULL) *vsat = streamGetInt(',');  // GNSS Satellites in View
    if (usat != NULL) *usat = streamGetInt(',');  // GNSS Satellites Used
    streamSkipUntil(',');                         // GLONASS Satellites Used
    streamSkipUntil(',');                         // Reserved3
    streamSkipUntil(',');                         // C/N0 max
    streamSkipUntil(',');                         // HPA
    streamSkipUntil('\n');                        // VPA

    waitResponse();

    return fix;
  }
};

#endif  // SRC_TINYGSMCLIENTSIM808_H_
