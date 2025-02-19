/** Output
 *
 *
 *  (c)Copyright Tony Clulow  2021    tony.clulow@pentadtech.com
 *
 *  This work is licensed under the:
 *      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *      http://creativecommons.org/licenses/by-nc-sa/4.0/
 *
 *  For commercial use, please contact the original copyright holder(s) to agree licensing terms
 */


 #include "All.h"


/** Read an Output's data from an OutputModule.
 */
void readOutput(uint8_t aNode, uint8_t aPin)
{
    if (isOutputNodePresent(aNode))
    {
        outputNode = aNode;
        outputPin  = aPin;

        if (isDebug(DEBUG_DETAIL))
        {
            Serial.print(millis());
            Serial.print(CHAR_TAB);
            Serial.print(PGMT(M_DEBUG_READ));
            Serial.print(HEX_CHARS[outputNode]);
            Serial.print(HEX_CHARS[outputPin]);
            Serial.println();
        }
    
        Wire.beginTransmission(I2C_OUTPUT_BASE_ID + outputNode);
        Wire.write(COMMS_CMD_READ | outputPin);
        if (   (Wire.endTransmission() == 0)
            && (Wire.requestFrom(I2C_OUTPUT_BASE_ID + outputNode, sizeof(outputDef)) == sizeof(outputDef))
            && (Wire.available() == sizeof(outputDef)))
        {
            // Read the outputDef from the OutputModule.
            outputDef.read();
            
            if (isDebug(DEBUG_DETAIL))
            {
                outputDef.printDef(M_DEBUG_READ, outputPin);
            }
        }
        else
        {
            outputDef.set(OUTPUT_TYPE_NONE, false, OUTPUT_DEFAULT_LO, OUTPUT_DEFAULT_HI, OUTPUT_DEFAULT_PACE, 0);
            systemFail(M_OUTPUT, aNode);
            setOutputNodePresent(aNode, false);
        }

        // Ignore any data that's left
        while (Wire.available())
        {
            Wire.read();
        }
    }
    else
    {
        outputDef.set(OUTPUT_TYPE_NONE, false, OUTPUT_DEFAULT_LO, OUTPUT_DEFAULT_HI, OUTPUT_DEFAULT_PACE, 0);
    }
}


/** Read an Output's data from an OutputModule.
 */
void readOutput(uint8_t aOutputNumber)
{
    readOutput((aOutputNumber >> OUTPUT_NODE_SHIFT) & OUTPUT_NODE_MASK, aOutputNumber & OUTPUT_PIN_MASK);
}


/** Write current Output's data to its OutputModule.
 */
void writeOutput()
{
    if (isDebug(DEBUG_DETAIL))
    {
        Serial.print(millis());
        Serial.print(CHAR_TAB);
        Serial.print(PGMT(M_DEBUG_WRITE));
        Serial.print(HEX_CHARS[outputNode]);
        Serial.print(HEX_CHARS[outputPin]);
        Serial.println();
        outputDef.printDef(M_DEBUG_WRITE, outputPin);
    }

    Wire.beginTransmission(I2C_OUTPUT_BASE_ID + outputNode);
    Wire.write(COMMS_CMD_WRITE | outputPin);
    outputDef.write();
    Wire.endTransmission();
}


/** Persist an Output's data to an OutputModule.
 */
void writeSaveOutput()
{
    if (isDebug(DEBUG_DETAIL))
    {
        Serial.print(millis());
        Serial.print(CHAR_TAB);
        Serial.print(PGMT(M_DEBUG_SAVE));
        Serial.print(HEX_CHARS[outputNode]);
        Serial.print(HEX_CHARS[outputPin]);
        Serial.println();
        outputDef.printDef(M_DEBUG_SAVE, outputPin);
    }

    Wire.beginTransmission(I2C_OUTPUT_BASE_ID + outputNode);
    Wire.write(COMMS_CMD_SAVE | outputPin);
    Wire.endTransmission();
}


/** Write a change of state to the Output module.
 */
void writeOutputState(uint8_t aNode, uint8_t aPin, boolean aState, uint8_t aDelay)
{
    if (isDebug(DEBUG_BRIEF))
    {
        Serial.print(millis());
        Serial.print(CHAR_TAB);
        Serial.print(PGMT(M_DEBUG_SEND));
        Serial.print(aNode, HEX);
        Serial.print(aPin, HEX);
        Serial.print(PGMT(M_DEBUG_STATE));
        Serial.print(PGMT(aState ? M_HI : M_LO));
        Serial.print(PGMT(M_DEBUG_DELAY_TO));
        Serial.print(aDelay, HEX);
        Serial.println();
    }

    Wire.beginTransmission(I2C_OUTPUT_BASE_ID + aNode);
    Wire.write((aState ? COMMS_CMD_SET_HI : COMMS_CMD_SET_LO) | aPin);
    Wire.write(aDelay);
    Wire.endTransmission();
}


/** Reset current Output. 
 *  And then reload its definition.
 */
void resetOutput()
{
    if (isDebug(DEBUG_DETAIL))
    {
        Serial.print(millis());
        Serial.print(CHAR_TAB);
        Serial.print(PGMT(M_DEBUG_RESET));
        Serial.print(outputNode, HEX);
        Serial.print(outputPin, HEX);
        Serial.println();
        outputDef.printDef(M_DEBUG_RESET, outputPin);
    }

    Wire.beginTransmission(I2C_OUTPUT_BASE_ID + outputNode);
    Wire.write(COMMS_CMD_RESET | outputPin);
    Wire.endTransmission();

    // Reload the Output now it's been reset.
    readOutput(outputNode, outputPin);
}


/** Read the states of the given node's Outputs.
 *  Save in OutputStates.
 */
void readOutputStates(uint8_t aNode)
{
    int states;
    
    Wire.beginTransmission(I2C_OUTPUT_BASE_ID + aNode);
    Wire.write(COMMS_CMD_SYSTEM | COMMS_SYS_STATES);
    if (   (Wire.endTransmission() == 0)
        && (Wire.requestFrom(I2C_OUTPUT_BASE_ID + aNode, OUTPUT_STATE_LEN) == OUTPUT_STATE_LEN)
        && ((states = Wire.read()) >= 0))
    {
        setOutputNodePresent(aNode, true);
        setOutputStates(aNode, states);

        if (isDebug(DEBUG_DETAIL))
        {
            Serial.print(millis());
            Serial.print(CHAR_TAB);
            Serial.print(PGMT(M_DEBUG_STATES));
            Serial.print(aNode, HEX);
            Serial.print(CHAR_SPACE);
            Serial.print(states, HEX);
            Serial.println();
        }
    }
    else
    {
        setOutputNodePresent(aNode, false);
    }
}


/** Gets the states of all the given node's Outputs.
 */
uint8_t getOutputStates(uint8_t aNode)
{
    return outputStates[aNode];
}


/** Sets the states of all the given node's Outputs.
 */
void setOutputStates(uint8_t aNode, uint8_t aStates)
{
    outputStates[aNode] = aStates;
}


/** Gets the state of the given Output's given pin.
 */
boolean getOutputState(uint8_t aNode, uint8_t aPin)
{
    return (outputStates[aNode] & (1 << aPin)) != 0;
}


/** Sets the state of the given node's Output pin.
 */
void setOutputState(uint8_t aNode, uint8_t aPin, boolean aState)
{
    uint8_t mask = 1 << aPin;
    
    if (aState)
    {
        outputStates[aNode] |= mask;
    }
    else
    {
        outputStates[aNode] &= ~mask;
    }
}


/** Record the presence of an OutputNode in the map.
 */
void setOutputNodePresent(uint8_t aNode, boolean aState)
{
    if (aState)
    {
        outputNodes |= ((long)1 << aNode);
    }
    else
    {
        outputNodes &= ~((long)1 << aNode); 
    }
}


/** Is an Output node present?
 *  Look for Output's node in outputNodes.
 */
boolean isOutputNodePresent(uint8_t aNode)
{
    return (aNode < OUTPUT_NODE_MAX) && (outputNodes & ((long)1 << aNode));
}
