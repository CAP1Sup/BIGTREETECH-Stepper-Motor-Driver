#ifndef TLE5012_H
#define TLE5012_H

// Libraries
#include "Arduino.h"
#include "config.h"
#include <MovingAverage.h>

// Register locations (reading)
#define ENCODER_READ_COMMAND    0x8000 // 8000
#define ENCODER_STATUS_REG     (0x0000U) // Same as base
#define ENCODER_ANGLE_REG      (0x0020U)
#define ENCODER_SPEED_REG      (0x0030U)
#define ENCODER_ANGLE_REV_REG  (0x0040U)
#define ENCODER_TEMP_REG       (0x0050U)

// Register locations (writing)
#define ENCODER_WRITE_COMMAND   0x5000    // 5000
#define ENCODER_ACT_STATUS_REG (0x0010U)  // Activation status

// Calculation constants
#define POW_2_16                    65536     // 2^16
#define POW_2_15                    32768     // 2^15
#define POW_2_7                     128       // 2^7
#define DELETE_BIT_15               0x7FFF    // Used to delete everything except the first 15 bits
#define CHANGE_UINT_TO_INT_15       0x8000    // Used to change unsigned 16 bit integer into signed
#define CHECK_BIT_14                0x4000    // Used to check the 14th bit
#define GET_BIT_14_4                0x7FF0    // Used to check the 14th bit?
#define TEMP_OFFSET                 152.0     // Used to offset the temp reading
#define TEMP_DIV                    2.776     // Used to divide the temperature
#define DELETE_7_BITS               0x01FF    // Used to delete the first 7 bits of a 16 bit integer
#define CHANGE_UNIT_TO_INT_9        0x0200    // Used to change an unsigned 9 bit integer into signed
#define CHECK_BIT_9                 0x0100    // Used to check the 9th bit

// Safety types (for error checking)
#define SAFE_LOW   0x0000  // Error checking off
#define SAFE_HIGH  0x0001  // Error checking on

// Bitmasks for several error codes
#define ENCODER_SYSTEM_ERROR_MASK           0x4000    //!< \brief System error masks for safety words
#define ENCODER_INTERFACE_ERROR_MASK        0x2000    //!< \brief Interface error masks for safety words
#define ENCODER_INV_ANGLE_ERROR_MASK        0x1000    //!< \brief Angle error masks for safety words

// CRC calculation values
#define CRC_POLYNOMIAL  0x1D
#define CRC_SEED        0xFF

/**
 * @brief Error types from safety word
 */
enum errorTypes {
	NO_ERROR               = 0x00,  //!< \brief NO_ERROR = Safety word was OK
	SYSTEM_ERROR           = 0x01,  //!< \brief SYSTEM_ERROR = over/under voltage, VDD negative, GND off, ROM defect
	INTERFACE_ACCESS_ERROR = 0x02,  //!< \brief INTERFACE_ACCESS_ERROR = wrong address or wrong lock
	INVALID_ANGLE_ERROR    = 0x03,  //!< \brief INVALID_ANGLE_ERROR = NO_GMR_A = 1 or NO_GMR_XY = 1
	ANGLE_SPEED_ERROR      = 0x04,  //!< \brief ANGLE_SPEED_ERROR = combined error, angular speed calculation wrong
	CRC_ERROR              = 0xFF   //!< \brief CRC_ERROR = Cyclic Redundancy Check (CRC), which includes the STAT and RESP bits wrong
};

// Main address fields
enum Addr_t {
    REG_STAT         = (0x0000U),    //!< \brief STAT status register
    REG_ACSTAT       = (0x0010U),    //!< \brief ACSTAT activation status register
    REG_AVAL         = (0x0020U),    //!< \brief AVAL angle value register
    REG_ASPD         = (0x0030U),    //!< \brief ASPD angle speed register
    REG_AREV         = (0x0040U),    //!< \brief AREV angle revolution register
    REG_FSYNC        = (0x0050U),    //!< \brief FSYNC frame synchronization register
    REG_MOD_1        = (0x0060U),    //!< \brief MOD_1 interface mode1 register
    REG_SIL          = (0x0070U),    //!< \brief SIL register
    REG_MOD_2        = (0x0080U),    //!< \brief MOD_2 interface mode2 register
    REG_MOD_3        = (0x0090U),    //!< \brief MOD_3 interface mode3 register
    REG_OFFX         = (0x00A0U),    //!< \brief OFFX offset x
    REG_OFFY         = (0x00B0U),    //!< \brief OFFY offset y
    REG_SYNCH        = (0x00C0U),    //!< \brief SYNCH synchronicity
    REG_IFAB         = (0x00D0U),    //!< \brief IFAB register
    REG_MOD_4        = (0x00E0U),    //!< \brief MOD_4 interface mode4 register
    REG_TCO_Y        = (0x00F0U),    //!< \brief TCO_Y temperature coefficient register
    REG_ADC_X        = (0x0100U),    //!< \brief ADC_X ADC X-raw value
    REG_ADC_Y        = (0x0110U),    //!< \brief ADC_Y ADC Y-raw value
    REG_D_MAG        = (0x0140U),    //!< \brief D_MAG angle vector magnitude
    REG_T_RAW        = (0x0150U),    //!< \brief T_RAW temperature sensor raw-value
    REG_IIF_CNT      = (0x0200U),    //!< \brief IIF_CNT IIF counter value
    REG_T25O         = (0x0300U)     //!< \brief T25O temperature 25°c offset value
};


// Register access addresses
enum Access_t {
    REG_ACCESS_R    = (0x01U),      //!< \brief Read access register */
    REG_ACCESS_W    = (0x02U),      //!< \brief Write access register */
    REG_ACCESS_RW   = (0x03U),      //!< \brief Read & write access register */
    REG_ACCESS_U    = (0x04U),      //!< \brief Update register */
    REG_ACCESS_RU   = (0x05U),      //!< \brief Read & update register */
    REG_ACCESS_RWU  = (0x07U),      //!< \brief Read & write & update register */
    REG_ACCESS_RES  = (0x10U)       //!< \brief Reserved access register */
};

// Bitfield structure
typedef struct {
    uint8_t  regAccess;              //!< \brief Bitfield register access */
    uint16_t regAddress;             //!< \brief Bitfiled register address */
    uint16_t mask;                   //!< \brief Bitfield mask */
    uint8_t  position;               //!< \brief Bitfiled position */
    uint8_t  resetValue;             //!< \brief Bitfield register reset value */
    uint8_t  posMap;                 //!< \brief Bitfield position of register in regMap */
} BitField_t;

// Address field structure
typedef struct {
    uint16_t regAddress;            //!< \brief Addressfield register address */
    uint8_t  posMap;                //!< \brief Addressfield register regMap position */
} AddressField_t;

// Address field variable
static const AddressField_t addrFields[] =
{
	{REG_STAT,     1    },    //!< \brief STAT status register
	{REG_ACSTAT,   2    },    //!< \brief ACSTAT activation status register
	{REG_AVAL,     3    },    //!< \brief AVAL angle value register
	{REG_ASPD,     4    },    //!< \brief ASPD angle speed register
	{REG_AREV,     5    },    //!< \brief AREV angle revolution register
	{REG_FSYNC,    6    },    //!< \brief FSYNC frame synchronization register
	{REG_MOD_1,    7    },    //!< \brief MOD_1 interface mode1 register
	{REG_SIL,      8    },    //!< \brief SIL register
	{REG_MOD_2,    9    },    //!< \brief MOD_2 interface mode2 register
	{REG_MOD_3,   10    },    //!< \brief MOD_3 interface mode3 register
	{REG_OFFX,    11    },    //!< \brief OFFX offset x
	{REG_OFFY,    12    },    //!< \brief OFFY offset y
	{REG_SYNCH,   13    },    //!< \brief SYNCH synchronicity
	{REG_IFAB,    14    },    //!< \brief IFAB register
	{REG_MOD_4,   15    },    //!< \brief MOD_4 interface mode4 register
	{REG_TCO_Y,   16    },    //!< \brief TCO_Y temperature coefficient register
	{REG_ADC_X,   17    },    //!< \brief ADC_X ADC X-raw value
	{REG_ADC_Y,   18    },    //!< \brief ADC_Y ADC Y-raw value
	{REG_D_MAG,   19    },    //!< \brief D_MAG angle vector magnitude
	{REG_T_RAW,   20    },    //!< \brief T_RAW temperature sensor raw-value
	{REG_IIF_CNT, 21    },    //!< \brief IIF_CNT IIF counter value
	{REG_T25O,    22    },    //!< \brief T25O temperature 25°c offset value
};

#define MAX_NUM_REG 0x16      //!< \brief defines the value for temporary data to read all readable registers


// Bit fields
enum BitFieldReg_t
{
    REG_STAT_SRST,
    REG_STAT_SWD,
    REG_STAT_SVR,
    REG_STAT_SFUSE,
    REG_STAT_SDSPU,
    REG_STAT_SOV,
    REG_STAT_SXYOL,
    REG_STAT_SMAGOL,
    REG_STAT_RESERVED,
    REG_STAT_SADCT,
    REG_STAT_SROM,
    REG_STAT_NOGMRXY,
    REG_STAT_NOGMRA,
    REG_STAT_SNR,
    REG_STAT_RDST,

    REG_ACSTAT_ASRST,
    REG_ACSTAT_ASWD,
    REG_ACSTAT_ASVR,
    REG_ACSTAT_ASFUSE,
    REG_ACSTAT_ASDSPU,
    REG_ACSTAT_ASOV,
    REG_ACSTAT_ASVECXY,
    REG_ACSTAT_ASVEGMAG,
    REG_ACSTAT_RESERVED1,
    REG_ACSTAT_ASADCT,
    REG_ACSTAT_ASFRST,
    REG_ACSTAT_RESERVED2,

    REG_AVAL_ANGVAL,
    REG_AVAL_RDAV,

    REG_ASPD_ANGSPD,
    REG_ASPD_RDAS,

    REG_AREV_REVOL,
    REG_AREV_FCNT,
    REG_AREV_RDREV,

    REG_FSYNC_TEMPR,
    REG_FSYNC_FSYNC,

    REG_MOD_1_IIFMOD,
    REG_MOD_1_DSPUHOLD,
    REG_MOD_1_RESERVED1,
    REG_MOD_1_CLKSEL,
    REG_MOD_1_RESERVED2,
    REG_MOD_1_FIRMD,

    REG_SIL_ADCTVX,
    REG_SIL_ADCTVY,
    REG_SIL_ADCTVEN,
    REG_SIL_RESERVED1,
    REG_SIL_FUSEREL,
    REG_SIL_RESERVED2,
    REG_SIL_FILTINV,
    REG_SIL_FILTPAR,

    REG_MOD_2_AUTOCAL,
    REG_MOD_2_PREDICT,
    REG_MOD_2_ANGDIR,
    REG_MOD_2_ANGRANGE,
    REG_MOD_2_RESERVED1,

    REG_MOD_3_PADDRV,
    REG_MOD_3_SSCOD,
    REG_MOD_3_SPIKEF,
    REG_MOD_3_ANG_BASE,

    REG_OFFX_RESERVED1,
    REG_OFFX_XOFFSET,

    REG_OFFY_RESERVED1,
    REG_OFFY_YOFFSET,

    REG_SYNCH_RESERVED1,
    REG_SYNCH_SYNCH,

    REG_IFAB_IFADHYST,
    REG_IFAB_IFABOD,
    REG_IFAB_FIRUDR,
    REG_IFAB_ORTHO,

    REG_MOD_4_IFMD,
    REG_MOD_4_RESERVED1,
    REG_MOD_4_IFABRES,
    REG_MOD_4_HSMPLP,
    REG_MOD_4_TCOXT,

    REG_TCO_Y_CRCPAR,
    REG_TCO_Y_SBIST,
    REG_TCO_Y_TCOYT,

    REG_ADC_X_ADCX,

    REG_ADC_Y_ADCY,

    REG_D_MAG_MAG,
    REG_D_MAG_RESERVED1,

    REG_T_RAW_TRAW,
    REG_T_RAW_RESERVED1,
    REG_T_RAW_TTGL,

    REG_IIF_CNT_IIFCNT,
    REG_IIF_CNT_RESERVED1,

    REG_T25O_T250,
    REG_T25O_RESERVED1,
};

// Encoder class
class Encoder {

    public:
        // Functions
        // Constructor
        Encoder();

        // Low level reading functions
        errorTypes readRegister(uint16_t registerAddress, uint16_t &data);
        void readMultipleRegisters(uint16_t registerAddress, uint16_t* data, uint16_t dataLength);
        uint16_t getBitField(BitField_t bitField);

        // Low level writing functions
        void writeToRegister(uint16_t registerAddress, uint16_t data);
        void setBitField(BitField_t bitfield, uint16_t bitFNewValue);

        // Error checking
        errorTypes checkSafety(uint16_t safety, uint16_t command, uint16_t* readreg, uint16_t length);
        uint8_t calcCRC(uint8_t *data, uint8_t length);
        void resetSafety();

        // Fast functions
        // Reads the raw momentary encoder increments value from the angle register (unadjusted)
        uint16_t getRawIncrements();

        // Returns the absolute momentary encoder increments (adjusted) in the range  of +/-335544 rev's of shaft
        increments_t getAbsoluteIncrements();

        // Returns the raw average encoder increments value from the angle register (unadjusted)
        uint16_t getRawIncrementsAvg();

        // Returns the absolute increments of the encoder (adjusted) in the range  of +/-335544 rev's of shaft
        increments_t getAbsoluteIncrementsAvg();

        // Gets the momentary absolute angle of the motor
        double getAbsoluteAngle();

        // High level encoder functions
        // Reads the raw momentary value from the angle of the encoder (adjusted)
        double getRawAngle();

        // Reads the raw average value from the angle of the encoder (adjusted)
        double getRawAngleAvg();

        // Returns a smoothed value of angle of the encoder
        // More expensive than getAngle(), but transitions between 0 and 360 are smoother
        double getSmoothAngle();

        // Reads the momentary value for the angle of the encoder (ranges from 0-360)
        double getAngle();

        // Reads the average value for the angle of the encoder (ranges from 0-360)
        double getAngleAvg();
        double getEstimSpeed();
        int16_t getRawSpeed();
        double getSpeed();
        double getAccel();
        int16_t getRawTemp();
        double getTemp();
        int16_t getRawRev();
        int32_t getRev();
        double getAbsoluteAngleAvg();
        float getAbsoluteAngleAvgFloat();
        void setStepOffset(double offset);
        void setIncrementsOffset(uint16_t offset);
        void zero();

        // Encoder estimation
        #ifdef ENCODER_SPEED_ESTIMATION

            // Checks if the minimum sample time for the speed has been exceeded
            bool sampleTimeExceeded();
        #endif

    private:
        // Variables
        uint32_t lastAngleSampleTime;
        double lastEncoderAngle = 0;

        // Last state of getRawRev()
        int16_t lastRawRev = 0;

        // Revolutions extender variable
        // Total revolutions = (revolutions * 512) + getRawRev()
        int32_t revolutions = 0;

        // Moving average instances
        MovingAverage <float, float> speedAvg;
        MovingAverage <int16_t, int32_t> rawSpeedAvg;
        MovingAverage <float, float> accelAvg;
        MovingAverage <uint16_t, int32_t> incrementAvg;
        MovingAverage <float, float> absAngleAvg;
        MovingAverage <increments_t, increments_t> absIncrementsAvg;
        MovingAverage <int16_t, int16_t> rawTempAvg;

        // The startup angle and rev offsets
        double startupAngleOffset = 0;
        uint16_t startupIncrementsOffset = 0; // Fix AVAL - Angle Value Register
        int16_t startupRevOffset = 0;         // Fix AREV - Angle Revolution Register
        double encoderStepOffset = 0;         // calibration

        // SPI init structure
        SPI_HandleTypeDef spiConfig;

        // Main initialization structure
        GPIO_InitTypeDef GPIO_InitStructure;

        // Storage for the last overtemp time
        #ifdef ENABLE_OVERTEMP_PROTECTION
            uint32_t lastOvertempTime = 0;
        #endif

        // A map of the known registers
        uint16_t regMap[MAX_NUM_REG];              //!< Register map */
};

#endif

