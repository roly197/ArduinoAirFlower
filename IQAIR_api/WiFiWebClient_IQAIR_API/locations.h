int selectedLocation = 0; // This stores the selected location (button select / from 0-2)

const int NumberOfLocations = 3;
const String Location[NumberOfLocations][3] = {
                         {"Enschede","Overijssel","Netherlands"},
                         {"Delhi","Delhi","India"},
                         {"Houston","Texas","USA"}
                         };
                         // City, Region , Country 
                         
const int NumberOfIntervals = 3;                         
const int AirIntervals[NumberOfLocations][8] = {
                          {0,100,20000,20000,6000,2000,6000,2000},
                          {101,200,20000,20000,3000,1000,3000,1000},
                          {201,999,20000,20000,1000,0000,1000,0000}
                          };
                         // MinResult, MaxResult, presetDeflate(ms), presetInflate(ms), inAir(ms), pause1(ms), outAir(ms), pause2(ms)


const bool VacuumValveOfPump = false; //Set to "true" to use valve and "false" to deflate with vacuum pump.
const bool DualAction = false; // Set to true to use dual pump and dual valves. 
// Program | VacuumValveOfPump | DualAction
// --------|-------------------|-----------
//    1    |       true        |   false   (single motor, single valve)
//    2    |       false       |   false   (single motor, vacuum motor)
//    3    |       true        |   true    (dual motor, dual valve)
//   n.a.  |       false       |   true    (cannot use pump2 to inflate and deflate at the same time) 

// From here: do not change variables unless you know what you're doing :-)

const String SearchKey = "aqius"; //
const unsigned long ApiRequestInterval = 1800L; // Interval to request API data (sec). (1800 sec is 30min)  
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
