const int NumberOfLocations = 3;
int selectedLocation = 0; // This stores the selected location (button select / from 0-2)
const String Location[NumberOfLocations][3] = {
                         {"Enschede","Overijssel","Netherlands"},
                         {"Delhi","Delhi","India"},
                         {"Houston","Texas","USA"}
                         };
                         // City, Region , Country
const int AirIntervals[NumberOfLocations][7] = {
                          {0,100,20000,6000,2000,6000,2000},
                          {101,200,20000,3000,1000,3000,1000},
                          {201,999,20000,1000,0000,1000,0000}
                          };
                         // MinResult, MaxResult, preset(ms), inAir(ms), pause1(ms), outAir(ms), pause2(ms)
const String SearchKey = "aqius"; //
const unsigned long ApiRequestInterval = 10L; // Interval to request API data (sec)  
const bool debug = true; // Debug information to serial port. In case of problems.

// Arduino Pin assignments
const int pump1 = 9;    // PWM ports from 220 - 254 (only max for now)
const int pump2 = 10;   // PWM ports from 220 - 254 (only max for now)
const int valve1 = 2;   // for now digital pins (open/close)
const int valve2 = 4;   // for now digital pins (open/close)
const int valve3 = 7;   // for now digital pins (open/close)
const int switch1 = 8;  //
const int switch2 = 12; //
const int switch3 = 13; //
