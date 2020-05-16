/** Switch data.
 */
#ifndef _SWITCH_H
#define _SWITCH_H

// Switch modules.
#define SWITCH_MODULE_SIZE  16  // 16 switches to each module.
#define SWITCH_MODULE_MAX   8   // Maximum modules.
#define SWITCH_MODULE_SHIFT 4   // Shift switch number this amount to get a module number.

// SwitchData saved in EEPROM
#define SWITCH_BASE SERVO_END                                 // EEPROM base of Switch data.
#define SWITCH_SIZE sizeof(SwitchData)                        // Size of SwitchData entry.
#define SWITCH_MAX  (SWITCH_MODULE_SIZE * SWITCH_MODULE_MAX)  // Maximum switches (up to 128).
#define SWITCH_END  (SWITCH_BASE + SWITCH_SIZE * SWITCH_MAX)  // End of Switch EEPROM.


/** Data describing a switch's operation.
 */
struct SwitchData
{
  uint8_t servo  = 0xff;
  uint8_t servo2 = 0xff;
};

// Variables for working with a Switch.
uint16_t   switchModules = 0;   // Bit map of Switch modules present.
uint8_t    switchNumber  = 0;   // Current Switch number.
SwitchData switchData;          // Data describing current Servo.


/** Load a Switch's data from EEPROM.
 */
void loadSwitch(uint8_t aSwitch)
{
  if (aSwitch < SWITCH_MAX)
  {
    switchNumber = aSwitch;
    EEPROM.get(SWITCH_BASE + switchNumber * SWITCH_SIZE, switchData);
  }
}


/** Save a Switch's data to EEPROM.
 *  Data in switchNumber and switchData.
 */
void saveSwitch()
{
  if (switchNumber < SWITCH_MAX)
  {
    EEPROM.put(SWITCH_BASE + switchNumber * SWITCH_SIZE, switchData);
  }
}


/** Record the presence of a SwitchModule in the map.
 */
void setSwitchModulePresent(uint8_t aModule)
{
  switchModules |= (1 << aModule); 
}


/** Is a Switch present?
 *  Look for switch's module in switchModules.
 */
boolean isSwitch(uint8_t aSwitch)
{
  return switchModules & (1 << (aSwitch >> SWITCH_MODULE_SHIFT));
}

#endif
