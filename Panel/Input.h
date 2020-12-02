/** Input data.
 */
#ifndef _Input_h
#define _Input_h

// Input nodes.
#define INPUT_NODE_SIZE          16     // 16 inputs to each node.
#define INPUT_NODE_MAX            8     // Maximum nodes.
#define INPUT_NODE_ALL_MASK    0xff     // All input nodes present.
#define INPUT_NODE_MASK        0x07     // 3 bits for 8 nodes.
#define INPUT_NODE_SHIFT          4     // Shift input number this amount to get a node number.
#define INPUT_PIN_MASK         0x0f     // Mask to get input pin within a node.

// Mask for Input options
#define INPUT_OUTPUT_MAX          3     // Number of outputs each input can control.
#define INPUT_DISABLED_MASK    0x80     // The Input's output is disabled.
#define INPUT_OUTPUT_MASK      0x7f     // Mask to get the Input's output without the flag above.

// Input types
#define INPUT_TYPE_MASK        0x03     // Input types take 2 bits each.
#define INPUT_TYPE_MULT           2     // Input type multiplied by 2 bits per pin.
#define INPUT_TYPE_SHIFT          1     // Input type shifted by 1 bit = multiplied by 2.

#define INPUT_TYPE_TOGGLE         0     // A Toggle Input.
#define INPUT_TYPE_ON_OFF         1     // An on/off Input.
#define INPUT_TYPE_ON             2     // An on Input.
#define INPUT_TYPE_OFF            3     // An off Input.
#define INPUT_TYPE_MAX            4     // Limit of Input types.


// Mask for MCP device none or all bits.
#define MCP_ALL_LOW   0x00
#define MCP_ALL_HIGH  0xFF

// MCP message commands.
#define MCP_IODIRA    0x00    // IO direction, High = input.
#define MCP_IODIRB    0x01
#define MCP_IPOLA     0x02    // Polarity, High = GPIO reversed.
#define MCP_IPOLB     0x03
#define MCP_GPINTENA  0x04    // Interupt enabled.
#define MCP_GPINTENB  0x05
#define MCP_DEFVALA   0x06    // Interupt compare value. Used if INTCON set.
#define MCP_DEFVALB   0x07
#define MCP_INTCONA   0x08    // Interup control, High = use DEFVAL, low = use previous value.
#define MCP_INTCONB   0x09
#define MCP_IOCON     0x0A    // Control register. Not used. See datasheet.
#define MCP_IOCON_DUP 0x0B
#define MCP_GPPUA     0x0C    // Pull-ups. High = pull-up resistor enabled.
#define MCP_GPPUB     0x0D
#define MCP_INTFA     0x0E    // Interupt occurred on these pins (read-only).
#define MCP_INTFB     0x0F
#define MCP_INTCAPA   0x10    // Interupt capture. Copy of GPIO when interups occurred. 
#define MCP_INTCAPB   0x11    // Cleared when read (or when GPIO read).
#define MCP_GPIOA     0x12    // GPIO pins.
#define MCP_GPIOB     0x13
#define MCP_OLATA     0x14    // Output latches (connected to GPIO pins).
#define MCP_OLATB     0x15


/** Definition of an Input..
 */
class InputDef
{
    private:
    uint8_t output[INPUT_OUTPUT_MAX];   // The outputs conrolled by this input.


    public:
    
    /** Gets the nth outputNode.
     */
    uint8_t getOutputNode(uint8_t aIndex)
    {
        return (output[aIndex] >> OUTPUT_NODE_SHIFT) & OUTPUT_NODE_MASK;
    }


    /** Sets the nth outputNode.
     */
    void setOutputNode(uint8_t aIndex, uint8_t aOutputNode)
    {
        output[aIndex] = (output[aIndex] & ~(OUTPUT_NODE_MASK << OUTPUT_NODE_SHIFT)) 
                       | ((aOutputNode   &  OUTPUT_NODE_MASK) << OUTPUT_NODE_SHIFT);
    }


    /** Gets the nth outputPin.
     */
    uint8_t getOutputPin(uint8_t aIndex)
    {
        return output[aIndex] & OUTPUT_PIN_MASK;
    }


    /** Sets the nth outputPin.
     */
    void setOutputPin(uint8_t aIndex, uint8_t aOutputPin)
    {
        output[aIndex] = (output[aIndex] & ~OUTPUT_PIN_MASK)
                       | (aOutputPin     &  OUTPUT_PIN_MASK);
    }


    /** Gets the nth outputNumber.
     */
    uint8_t getOutput(uint8_t aIndex)
    {
        return output[aIndex] & INPUT_OUTPUT_MASK;
    }


    /** Sets the nth outputNumber.
     */
    void setOutput(uint8_t aIndex, uint8_t aOutputNumber)
    {
        output[aIndex] = (output[aIndex] & ~INPUT_OUTPUT_MASK)
                       | (aOutputNumber  &  INPUT_OUTPUT_MASK);
    }


    /** Is the nth output disabled?
     */
    boolean isDisabled(uint8_t aIndex)
    {
        return output[aIndex] & INPUT_DISABLED_MASK;
    }


    /** Sets the nth output disabled (or not).
     */
    void setDisabled(uint8_t aIndex, boolean aDisabled)
    {
        output[aIndex] = (output[aIndex] & ~INPUT_DISABLED_MASK)
                       | (aDisabled ? INPUT_DISABLED_MASK : 0);
    }
};


/** Variables for working with an Input.
 */
int        inputNodes  = 0;   // Bit map of Input nodes present.
int        inputNumber = 0;   // Current Input number.
InputDef   inputDef;          // Definition of the current Input.
uint32_t   inputTypes  = 0L;  // The types of the Inputs. 2 bits per pin, 16 pins per node = 32 bits.
int        inputType   = 0;   // Type of the current Input (2 bits, INPUT_TYPE_MASK).


/** Load an Input's data from EEPROM.
 */
void loadInput(int aInput);


/** Load an Input's data from EEPROM.
 */
void loadInput(int aNode, int aInput);


/** Save an Input's data to EEPROM.
 *  Data in inputNumber and inputDef.
 */
void saveInput();


/** Record the presence of an InputNode in the map.
 */
void setInputNodePresent(int aNode);


/** Is an Input node present?
 *  Look for input's node in inputNodes.
 */
boolean isInputNode(int aNode);


/** Is an Input present?
 *  Look for input's node in inputNodes.
 */
boolean isInput(int aInput);


#endif
