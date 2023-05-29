/*
  *    Written By ManojBhakarPCM (india), 30 My 2021 during lockdown of second wave of corona.
  *    i am not associated with Quintic anyhow, but lover of RadioFrequency field.
  *    and i love all single chip transmitters. because i couldnt make any with transistor only :(
  *
  *    This file has been modified by TEB for use with the PixelRadio Project.
  *    The original QN8027 Arduino Library is imcompatible, but can be downloaded from:
  *    https://github.com/ManojBhakarPCM/Arduino-QN8027-with-Full-RDS-support
  *    Edits by TEB (thomastech), Feb-06-2022, Mar-05-2022
  *    Edits by TEB (thomastech) & dkulp, Jun-13-2022
  */

/* Transmit Mono audio only and stop tranmitting streo channels.
  *    Transmitter first create MPX spectrum in which it mixes all kind of data
  *    then this MPX spectrum is Modulated in Carrier Frequency which is determined by setFrequency() function.
  *    then this modulated frequency is amplified by internal RF PowerAmplifier(PA) and then sent to antenna.
  *    the MPX spectrum of FM channel is-
  *
  |                  ||                                          ||
  |---------------|  ||    /----------\     /------------\       ||
  |	   MONO		|  ||   /  Left      \   /    Right     \   /\ || /\
  |_______________|__||__/______________\_/________________\_/__\||/__\__
  |                                  19KHz				  38KHz					  57KHz
  |                         Stereo Pilot Tone   Saperation Pilot Tone     RDS Data Channel
  |
  |    This spectrum contains all data, which are - mono(left+right) audio channel, left channel , right channel , RDS channel.
  |    all  channels are saprated by pilot tones. which are 19KHz,19x2KHz,19x3KHz.
  |    actually there is only one pilot tone which is 19KHz, rest of that tones are second and third harmonics of this 19KHz pilot tone.
  |    harmonics are always weaker in amplitude than main tone.
  |    left and right side of 57KHz pilot tone are RDS data channel.
  |    in this chip you can set all these things-
  |
  |    Mono channel + LEFT + RIGHT ON/OFF                                    - mute();
  |    19KHz pilot tone width(Bandwidth) with its harmonics - setTxPilotFreqDeviation();
  |    LEFT + RIGHT channel ON/OFF                                       - MonoAudio();
  |    RDS channel ON/OFF									 - RDS();
  |    RDS data Width(Bandwidth)							 - setRDSFreqDeviation();
  |    Entire MPX channel BandWidth						 - setTxFreqDeviation();
  |
  |    Before mixing audio, audio is inputted to Input Buffer. where it can be amplified by which also depend on input impdence which is set by
  |setAudioInpImp()
  |    here they are Emphasized(Amplified) to reduce noise(hiss sound) which will enter in transmission when wave will travel in Air.
  |    high frequency hiss noise is very bad.
  |    to remove that unwanted noise is to reduce high frequency amplification compared to low frequency.(de - emphasis)
  |    but then frequency response will not be flat. so we need to pre amplify the high frequency audio in transmitter so that after
  |de-amplifying in receiver,
  |    it becomes flat. but what is cutoff for frequency for that you can determine which is "high" frequency and which is "low" freq.
  |    well that cutoff is called TC(time constant).
  |    75uS(2.1221KHz) pre emphasis means above this frequency audio will be more amplified then lower to it.
  |    75uS(2.1221KHz) EmphTimeConstant gives more treble while 50uS(3.1831KHz) gives bass boost.
  |
  |    if TimeConstant to Emphasize are opposite to each other in Transmitter and Receiver, Receiver will get either too much treble or too much
  |bass.
  |    so use setPreEmphTime50()  function to match emphasisTimeConstant according to receiver side de-emphasis time constant.
  |    then it is converted to digital audio by a ADC, where it can also be further amplified by setTxDigitalGain()
  |
  |    crystal is only needed for carrier frequency and pilot tone generation. I2C uses SCL wire instead.
  |
  |    many things can be used instead of crystal which can be selected by - setClockSource()
  |    when selected crystal, crystal frequency is selected by                      - setCrystalFreq()
  |    also, Max crystal current can also be adjusted by                                    - setCrystalCurrent()
  |
  |    when using 12MHz crystal with setCrystalFreq(24), it will transmit at half frequency with much more power.(because power reduced with
  |frequency)
  |    with this hack, you can transmit from 38MHz to 54MHz. this happens because carrier freq is generated by multiplying crystal freq.
  |    when you set carrier freq to 96MHz, chip thinks there is 24MHz crystal, so it multiply its freq by 4, but acutaly there is only 12MHz
  |crystal,
  |    so multiplied freq will be only 12x4 = 48 Mhz !! :)
  |
  |    RDS data format:
  |    RDS data is sent in one or more groups. a group is made of 104 bits which structure is-
  |			Block1				|			Block2				|			Block3				|			Block4				|
  |    ---------------------------------------------------------------------------------------------------------------------------------
  |  16bit data |  10bitcheckword	|  16bit data |  10bitcheckword	|  16bit data |  10bitcheckword	|  16bit data |  10bitcheckword	|
  |
  |    you can input 8 bytes in this function.
  |    each two bytes represents 16bit data of each block (8x2=16). 10bitcheckword of each group is send automatically so no need to worry about
  |it.
  |    that means that-
  |    By0 and By1 = 16 bit data of block1
  |    By2 and By3 = 16 bit data of block2
  |    By4 and By5 = 16 bit data of block3
  |    By6 and By7 = 16 bit data of block4
  |    ...............
  |    dont put your data in these bytes directly. it wont appear on receiver because data need to be encoded first in RDS format(which is very
  |simple format though)
  |
  |    there is a space in receiver where you can put characters. that space indicated by GTYPE below. character position on that space is
  |indexed.
  |    that means you have to send index position with characters so that receiver put that characters at given index to construct complete
  |string to show at display.
  |    for example when sending characters  on "radio text" space, we can send 4 characters at once. but there is total 64 characters can be
  |placed on that space.
  |    so we send it in 16 groups. only one group can be send by using function sendRDS() function. so we need to call this function 16 times.
  |    for first 4 characters, we will use index 0000 (0), which means index position 1,2,3,4.
  |    for next 4 characters , we will use index 0001 (1), which indicates index position of character number 5,6,7,8
  |    ......
  |    each group is made of 4 blocks.
  |    first block(first two bytes) are always PI code. which you can set anything(if you dont know what is it). here i used 0x6400.
  |    second block is made of many things- for example for Song/Artist/Album name (which is called radioText GTYPE) format is as follows-
  |    in this example we want to send "ManojBhakar!" as radio text.
  |    GTYPE(4bit)|B0(1bit)|TP(1bit)|PTY(5bit)|screenClear(1bit)|Index(4bit) = total 16 bits = 2 bytes.
  |    GTYPE = 0010 (radio Text)
  |    B0    = 0    (Format A , there is also Format B which is complex)
  |    TP    = 0     (This is not a traffic alert program)
  |    PTY   = 10011(Program Type = Religious Music)
  |    SC    = 0    (dont need to clear screen of receiver)
  |    index = 0000 (put character1 to index 1, character2 to 2, c3 to 3, 4 to 4 etc.)
  |    third block is also 16bit(2 byte) long. here we put our first two characters which we want to place at index 1 and 2
  |    fourth block is also 16 bit long . here we will put our third and fourth character which will be placed at index 3 and 4 on receiver's
  |display.
  |
  |    so entire 8 bytes for sending our first four characters will look like this -
  |    0x64 0x00 0x22 0x60  M  a  n  o
  |
  |    for next 4 characters group will look like this -
  |    0x64 0x00 0x22 0x61  j  B  h  a
  |
  |    and last 4 characters will be sent in this format-
  |    0x64 0x00 0x22 0x62  k  a  r  !
  |
  |    note that when sending station name block1 will be same block2 will be entirely diffrent from last 5 bytes.
  |    station name is called GTYPE A0 which means Group Type 0 in Format A. you can search internet for getting exact format of this group.
  |    what i can tell here is that this grouptype is only capable of sending 2 bytes at once and can send maximum 8 bytes.
  |
  |
  |
  |    value of onOffCtrl variable can be:  ON or OFF.
  */

#include "QN8027Radio.h"
#include <Arduino.h>
#include <ArduinoLog.h>

// #define DEBUG_MODE

QN8027Radio::QN8027Radio (int address)
{
    _address = address;
}

QN8027Radio::QN8027Radio ()
{
    _address = QN8027_I2C_ADDR;
}

/* Set Transmitting Frequency From 76 to 108 MHz with decimal point
  *         Example - setFrequency(88.1); , setFrequency(100);
  *     0.01 is added to frequency to round-up KHz value.
  */
void QN8027Radio::setFrequency (float frequency)
{
    uint16_t    frequencyB  = ((frequency + 0.001f) * 100.0f - 7600.0f) / 5.0f;
    uint8_t     frequencyH  = frequencyB >> 8;

    freqH = frequencyH;
    uint8_t frequencyL = frequencyB & 0XFF;

    // freqL = frequencyL;
    write1Byte (SYSTEM_REG, frequencyH);
    write1Byte (CH1_REG,    frequencyL);
}

/* Get Currently Transmitting Frequency with decimal point */
float QN8027Radio::getFrequency ()
{
    uint8_t frequencyH  = read1Byte (SYSTEM_REG) & CH0_MASK;
    uint8_t frequencyL  = read1Byte (CH1_REG);
    float   freqCombine = (float)(((frequencyH << 8) | frequencyL) * 5 + 7600) / 100;

    return freqCombine;
}

/* Read any Readable Register From QN8027 in 8bit integer. uses I2C protocol. */
// Mod by TEB, Dec-30-2021
uint8_t QN8027Radio::read1Byte (uint8_t regAddr)
{
    uint8_t readData = 0xff;

    //    noInterrupts();  // Mod by TEB, Feb-01-2022
    Wire.beginTransmission (QN8027_I2C_ADDR);
    Wire.write (regAddr);
    Wire.endTransmission ();
    Wire.requestFrom (QN8027_I2C_ADDR, 1);

    if (Wire.available ())  // Mod by TEB, Dec-30-2021;
    {
        readData = Wire.read ();
    }

    Wire.endTransmission ();    // ACK read. Mod by TEB, Dec-30-2021. EndTrans must come AFTER Wire.read();
    //    interrupts();

    return readData;
}

/* Write any writable Register of QN8027
  *         regAddr = Address of Register want to write.
  *         comData = data you want to write in that register. comData means command Data.
  */
void QN8027Radio::write1Byte (uint8_t regAddr, uint8_t comData)
{
    //    noInterrupts();  // Mod by TEB, Feb-01-2022
    Wire.beginTransmission (QN8027_I2C_ADDR);
    Wire.   write ( regAddr);
    Wire.   write ( comData);
    Wire.endTransmission ();    // ACK read
    //    interrupts();
}

/* base Function For RDS data sending.
  */
void QN8027Radio::sendRDS (char By0, char By1, char By2, char By3, char By4, char By5, char By6, char By7)
{
    rdsSentStatus = read1Byte (STATUS_REG) & 8;
    write1Byte (RDSD0_REG,  By0);
    write1Byte (RDSD1_REG,  By1);
    write1Byte (RDSD2_REG,  By2);
    write1Byte (RDSD3_REG,  By3);
    write1Byte (RDSD4_REG,  By4);
    write1Byte (RDSD5_REG,  By5);
    write1Byte (RDSD6_REG,  By6);
    write1Byte (RDSD7_REG,  By7);

    if (rdsReady == 4)
    {
        rdsReady = 0;
    }
    else
    {
        rdsReady = 4;
    }

    updateSYSTEM_REG ();
}

// ---------------------------SYSTEM_REG------------------------------------------------------------
/*
  *    Resets all registers(settings) to default.
  */
void QN8027Radio::updateSYSTEM_REG () {write1Byte (SYSTEM_REG, (radioStatus | monoAudio | muteAudio | rdsReady | freqH));}

void QN8027Radio::reset ()
{
    write1Byte (SYSTEM_REG, 0x80);
    delayMicroseconds (100);
    write1Byte (SYSTEM_REG, 0x00);  // Mod by TEB, Jan-29-2022.
}

/* Recalibrates internal RF power amplifier for load antenna attached. this process is automatic and you just need to use this function
  *    only.*/
void QN8027Radio::reCalibrate ()
{
    write1Byte (SYSTEM_REG, 0x40);
    delayMicroseconds (100);
    write1Byte (SYSTEM_REG, 0x00);  // Mod by TEB, Jan-29-2022.
}

/*mutes audio to transmitter output. transmitter will only transmite carrier frequency without audio.
  *    value of onOffCtrl variable can be:  ON or OFF
  *    default is OFF.
  */
void QN8027Radio::mute (uint8_t onOffCtrl)  // also should set PAPower to 20
{
    if (onOffCtrl == ON)
    {
        muteAudio = 8;
    }
    else if (onOffCtrl == OFF)
    {
        muteAudio = 0;
    }

    updateSYSTEM_REG ();
    // write1Byte(SYSTEM_REG,(radioStatus | monoAudio | muteAudio | rdsReady | freqH));
}

// stop mixing Left and Right audio in MPX. means receiver will get only mono audio.
// default is stereo. param can be ON or OFF
void QN8027Radio::MonoAudio (uint8_t onOffCtrl)
{
    if (onOffCtrl == ON)
    {
        monoAudio = 16;
    }
    else if (onOffCtrl == OFF)
    {
        monoAudio = 0;
    }

    updateSYSTEM_REG ();

    // write1Byte(SYSTEM_REG,(radioStatus | monoAudio | muteAudio | rdsReady | freqH));
}

// Turn Transmitter ON or OFF. by default it is off and does not start just by giving voltage to this chip.
// you must call this function to start transmitter.
void QN8027Radio::Switch (uint8_t onOffCtrl)
{
    if (onOffCtrl == ON)
    {
        radioStatus = 32;
    }
    else if (onOffCtrl == OFF)
    {
        radioStatus = 0;
    }

    updateSYSTEM_REG ();
    // write1Byte(SYSTEM_REG,(radioStatus | monoAudio | muteAudio | rdsReady | freqH));
}

// ---------------------------GPLT_REG----------------------------------------------------------
void QN8027Radio::updateGPLT_REG () {write1Byte (GPLT_REG, (preEmphTime | privateMode | PAAutoOffTime | TxPilotFreqDeviation));}

// I really dont know why is this option there. it gave mono audio with narrow CarrierWave bandwidth in my tests.
// you can provide ON or OFF in parameter to this function.
void QN8027Radio::scrambleAudio (uint8_t onOffCtrl)
{
    if (onOffCtrl == ON)
    {
        privateMode = 64;
    }
    else if (onOffCtrl == OFF)
    {
        privateMode = 0;
    }

    // rite1Byte(GPLT_REG,(preEmphTime | privateMode | PAAutoOffTime | TxPilotFreqDeviation));
    updateGPLT_REG ();
}

// ON = Pre-Emphasis Time Constant = 50uS
// OFF= Pre-Emphasis Time Constant = 75uS (Which is default)
void QN8027Radio::setPreEmphTime50 (uint8_t onOffCtrl)
{
    if (onOffCtrl == ON)
    {
        preEmphTime = 0;
    }
    else if (onOffCtrl == OFF)
    {
        preEmphTime = 128;
    }

    // write1Byte(GPLT_REG,(preEmphTime | privateMode | PAAutoOffTime | TxPilotFreqDeviation));
    updateGPLT_REG ();
}

/* Set main pilot frequency (19KHz) Width(Bandwidth).
  *    PGain = 7  means 7% of 75 KHz
  *    PGain = 8  means 8% of 75 KHz
  *    PGain = 9  means 9% of 75 KHz (it is default)
  *    PGain = 10 means 10%of 75 KHz
  *    this will automatically affect second and third harmonics.
  */
void QN8027Radio::setTxPilotFreqDeviation (uint8_t PGain)
{
    TxPilotFreqDeviation = PGain;
    // write1Byte(GPLT_REG,(preEmphTime | privateMode | PAAutoOffTime | TxPilotFreqDeviation));
    updateGPLT_REG ();
}

// Change the PI Code. Be sure to use a valid PI Code.
// See https://picodes.nrscstandards.org/ and https://www.fmsystems-inc.com/rds-pi-code-formula-station-callsigns/
void QN8027Radio::setPiCode (uint16_t piCodeVal) {piCode = piCodeVal;}

// Change the PTY Code. Be sure to use a valid PTY Code.
void QN8027Radio::setPtyCode (uint8_t ptyCodeVal) {ptyCode = ptyCodeVal;}

/*
  *    ON  : RF power Amplifier will be off automatically after 60 second of no audio input at pin 6 and pin 7 (which is default)
  *    OFF : PA will never off. No effect of audio input silence.
  *    NOTE: Sending RDS data and/or using clearAudioPeak() will prevent automatic shut off.
  */
void QN8027Radio::radioNoAudioAutoOFF (uint8_t onOffCtrl)
{
    if (onOffCtrl == ON)
    {
        PAAutoOffTime = 32;
    }
    else if (onOffCtrl == OFF)
    {
        PAAutoOffTime = 48;
    }

    // write1Byte(GPLT_REG,(preEmphTime | privateMode | PAAutoOffTime | TxPilotFreqDeviation));
    updateGPLT_REG ();
}

// ------------------------XTL_REG-------------------------------------------------------------
void QN8027Radio::updateXTL_REG () {write1Byte (XTL_REG, (clockSource | CrystalCurrentuA));}

/*
  *    Type::meaning
  *    0   :: Using XTAL                            between pin1 and pin2
  *    1	:: Inject digital clock         between pin1 and ground.
  *    2	:: single end sin wave          between pin1 and ground.
  *    3	:: differential sin wave        between pin1 and pin2
  */
void QN8027Radio::setClockSource (uint8_t Type)
{
    clockSource = Type << 6;
    updateXTL_REG ();
}

/*
  *    Second option which i dont understand significance of.
  *    maximum current can be 400 uA
  *    you can input percentage of 400 in parameter
  *    for example setCrystalCurrent(50) means 50% of 400 = 200uA
  *    default is 100 micro ampere.
  */
void QN8027Radio::setCrystalCurrent (float percentOfMax)    // current between 0 to 400 uA
{
    CrystalCurrentuA = (uint8_t)((percentOfMax * 64) / 100);
    // write1Byte(XTL_REG,(clockSource | CrystalCurrentuA));
    updateXTL_REG ();
}

// -----------------------VGA_REG--------------------------------------------------------------
void QN8027Radio::updateVGA_REG () {write1Byte (VGA_REG, (crystalFreqMHz | TxInputBufferGain | TxDigitalGain | LRInputImpdKOhm));}

/*
  *    if clock input source is XTAL then you can set which XTAL was used.
  *    Freq::Meaning
  *    12  :: 12 MHz
  *    24  :: 24 MHz (default)
  */
void QN8027Radio::setCrystalFreq (uint8_t Freq)
{
    if (Freq == 24)
    {
        crystalFreqMHz = 128;
    }
    else    // if 12 or wrong value
    {
        crystalFreqMHz = 0;
    }

    updateVGA_REG ();
}

/*
  *    set audio amplification in input buffer. actual gain is also depends on inputImpedence() functions parameter.
  *    actual gain in dB = Gain = [(IBGain+1)*3] - [LRInputImpdKOhm*6]
  *    you can set IBGain value from 0 to 5
  *    default is 3
  */
void QN8027Radio::setTxInputBufferGain (uint8_t IBGain)
{
    if (IBGain > 5)
    {
        IBGain = 5;
    }

    TxInputBufferGain = IBGain << 4;
    updateVGA_REG ();
    // write1Byte(VGA_REG,(crystalFreqMHz | TxInputBufferGain | TxDigitalGain | LRInputImpdKOhm));
}

/* Digital Audio Amplification in decible.
  *    DGain::Meaning
  *    0    :: 0 dB (default)
  *    1	 :: 1 dB
  *    2	 :: 2 dB
  */
void QN8027Radio::setTxDigitalGain (uint8_t DGain)
{
    if (DGain > 2)
    {
        DGain = 2;
    }

    TxDigitalGain = DGain << 2;
    updateVGA_REG ();
    // write1Byte(VGA_REG,(crystalFreqMHz | TxInputBufferGain | TxDigitalGain | LRInputImpdKOhm));
}

void QN8027Radio::setAudioInpImp (uint8_t impdInKOhms)
{
    switch (impdInKOhms)
    {
        case 5:
        {
            LRInputImpdKOhm = 0;
            break;
        }

        case 10:
        {
            LRInputImpdKOhm = 1;
            break;
        }

        case 20:
        {
            LRInputImpdKOhm = 2;
            break;
        }

        case 40:
        {
            LRInputImpdKOhm = 3;
            break;
        }

        default:
        {
            LRInputImpdKOhm = 2;
            break;
        }
    }   // switch

    updateVGA_REG ();
    // write1Byte(VGA_REG,(crystalFreqMHz | TxInputBufferGain | TxDigitalGain | LRInputImpdKOhm));
}

// ---------------------------FDEV_REG------------------------------------------------------
/*
  *    set overall FM channel Bandwidth. less deviation means it will make sharp narrow peak in FM band.
  *    actual deviation in KHz = Fdev * 0.58
  *    Fdev values can be set from 0 to 255.
  *    default is 129 which means 74.82 KHz
  *    maximum bandwidth can be 148 KHz by setting Fdev value to 255
  */
void QN8027Radio::setTxFreqDeviation (uint8_t Fdev) {write1Byte (FDEV_REG, Fdev);}

// ---------------------------RDS_REG-------------------------------------------------------
/* set RDS channel ON or OFF */
void QN8027Radio::RDS (uint8_t onOffCtrl)
{
    if (onOffCtrl == ON)
    {
        RDSEnable = 128;
    }
    else
    {
        RDSEnable = 0;
    }

    write1Byte (RDS_REG, (RDSEnable | RDSFreqDeviationKHz));
}

/* set bandwidth of RDS channel.
  *    actual bandwidth in KHz = RDSFreqDev * 0.35
  *    RDSFreqDev value can be set from 0 to 127
  *    default is 6 which means 2.1 KHz
  *    maximum bandwidth can be 44.45 KHz by setting RDSFreqDev value to 127
  */
void QN8027Radio::setRDSFreqDeviation (uint8_t RDSFreqDev)
{
    RDSFreqDeviationKHz = RDSFreqDev;
    write1Byte (RDS_REG, (RDSEnable | RDSFreqDeviationKHz));
}

// --------------------------PAC_REG---------------------------------------------------------
/*
  *    this chip has a clever feature of audio peak detection. which can be used as silence detection or automatic audio Gain control.
  *    it can also be used as drawing input audio graph.
  *    you can read audio by function getAudioInpPeak(), which actually reads peak from STATUS_REG.
  *    then you use this function which actually toggles a bit in PAC_REG.
  *    so peak detection starts from 0 again.
  *    it will record highest input audio amplitude between two toggles of that clear bit.
  *    if you use getAudioInpPeak() function very frequntly then you will get changing. otherwise if you use it again after 1-2 second , audio
  *       peak always be
  *    fixed which is maximum between this time period.
  *    because its peak amplitude of input audio between previous toggle and this toggle.its not a value at exact moment of reading.
  */
void QN8027Radio::clearAudioPeak ()
{
    if (AudioPeakClear == 128)
    {
        AudioPeakClear = 0;
    }
    else
    {
        AudioPeakClear = 128;
    }

    write1Byte (PAC_REG, (AudioPeakClear | PAOutputPower));
}

/*
  *    sets power of internal RF Power Amplifier.
  *    you can set value from 20 to 75 (decimal).
  *    actual power = 0.62 * setX + 71 dBu
  *    maximum power can be 117.5 dBu
  *    minimum power can be 83.4  dBu
  *    default value of setX is 127 which makes no sense(not between 20 and 75). but we assumes that default is max.
  *    althogh setX is 7 bit long, which means you can set values from 0 to 127. but they said not to be valid.
  *    NOTE: Power will not be updated until RF carrier state is toggled Off-On (radioStatus bit).
  */

void QN8027Radio::setTxPower (uint8_t setX)
{
    PAOutputPower = setX & 0x7F;    // Mod by TEB, Jan-31-2022.
    write1Byte (PAC_REG, (AudioPeakClear | PAOutputPower));
}

// ----------------------STATUS_REG ----------------------------------------------------------
/*
  *    FSM status Value Meaning
  *    0	:: Resetting
  *    1   :: ReCalibrating
  *    2	:: IDLE
  *    3	:: Tx Ready
  *    4   :: PA Calibration
  *    5   :: Transmitting
  *    6   :: PA is OFF
  *
  *    Sample FSM Status Lookup Array
  *    String stats[] = { "Resetting", "Recalibrating", "Idle", "TxReady", "PACalib", "Transmitting", "PA Off", "Undefined" };
  */
uint8_t QN8027Radio::getFSMStatus ()
{
    uint8_t tmp = read1Byte (STATUS_REG);

    return tmp & 7;
}

/* get maximum amplitude of input audio since last reading
  *    multiply this value by 45 and you will get amplitude in mili Volts.
  */
uint8_t QN8027Radio::getAudioInpPeak ()
{
    uint8_t tmp = read1Byte (STATUS_REG);

    clearAudioPeak ();

    return tmp >> 4;
}

uint8_t QN8027Radio::getStatus ()
{
    uint8_t tmp = read1Byte (STATUS_REG);

    clearAudioPeak ();

    return tmp;
}

// Read the PI Code.
// See https://picodes.nrscstandards.org/ and https://www.fmsystems-inc.com/rds-pi-code-formula-station-callsigns/
uint16_t QN8027Radio::getPiCode (void) {return piCode;}

// Read the PTY Code, Mod By dkulp, Jun-13-2022
uint8_t QN8027Radio::getPTYCode (void) {return ptyCode;}

// -------------------RDS sending---------------------------------------------------------------
/*
  *    Sends Program Service Name (PSN = Station ID) such as "KZAP FM" to a RDS enabled receiver.
  *    PSN must be maximum 8 byte long String.
  *    PSN shorter than 8 bytes will contain a null termination. This tells the RDS Receiver when to end decoding.
  */
void QN8027Radio::sendStationName (String SN)
{
    char    char_array[PSN_SIZE + 1];
    int     str_len;
    int     rds_len;

    if (SN.length () > PSN_SIZE)    // Prevent Buffer Overflow.
    {
        SN = SN.substring (0, PSN_SIZE);
    }

    str_len = SN.length () + 1;
    rds_len = str_len + (str_len % 2);          // Make it a multiple of 2.

    memset (char_array, char(NULL), PSN_SIZE);  // Clear PSN Buffer (fill with nulls). Mod By TEB, Feb-03-2022.
    SN.toCharArray (char_array, str_len);

    for (int i = 0;i < rds_len;i += 2)
    {
        if (i >= PSN_SIZE)
        {
            break;  // Prevent RDS buffer overflow. Mod By TEB, Jan-31-2022.
        }

        uint8_t ptyHi = (ptyCode & 0x18) >> 3;  // top 2 bits of PTY are in bottom 2 bits of byte 3, Mod By dkulp,
                                                // Jun-13-2022
        uint8_t ptyLo = (ptyCode << 5) & 0xE0;  // bottom 3 bits of PTY are in top 3 bits of byte 4, Mod By dkulp,
                                                // Jun-13-2022
        sendRDS (highByte (piCode), lowByte (piCode), ptyHi, ptyLo | (0x08 + (i / 2)), 0xE0, 0xCD, char_array[i], char_array[i + 1]);
        waitForRDSSend ();
    }
}

/*
  *    waits for previous Group send. when previous group will finish sending, this function will return.
  *    Typical Group send time is ~70mS. Equivalent bit rate is ~475 baud.
  */
const uint8_t RDS_SEND_DELAY = 5;
void QN8027Radio::waitForRDSSend ()
{
    uint8_t status  = rdsSentStatus;
    uint8_t timeout = 0;

    do
    {
        status  = read1Byte (STATUS_REG);
        status  = status & 8;

        if (timeout++ > (100 / RDS_SEND_DELAY))  // Allow up to 100mS RDS Send time. Mod by TEB, Dec-27-2021
        {
            Log.errorln (F ("-> Abort: waitForRDSSend() function time-out!"));
            break;
        }

        delay (RDS_SEND_DELAY);         // This wait time allows the RDS buffer contents to be sent.
    } while (status == rdsSentStatus);  // Wait for rdsSentStatus to toggle.

    rdsSentStatus = status;
}

/*Sends Song Artist Album Name. RT must be maximum 64 Byte long*/
// RadioText shorter than 64 bytes will contain a null termination. This tells the RDS Receiver when to end decoding.
/* Execution time is about 1.2mS */
void QN8027Radio::sendRadioText (String RT)
{
    char    char_array[RADIOTEXT_SIZE + 1];
    int     rds_len;
    int     str_len;

    if (RT.length () > RADIOTEXT_SIZE)  // Prevent Buffer Overflow.
    {
        RT = RT.substring (0, RADIOTEXT_SIZE);
    }

    str_len = RT.length () + 1;
    rds_len = str_len + (str_len % 4);                  // Make it a multiple of 4.
    memset (char_array, char(NULL), RADIOTEXT_SIZE);    // Clear RadioText Buffer (fill with nulls). Mod By TEB, Feb-03-2022.
    RT.toCharArray (char_array, str_len);               // Copy RadioText to Buffer.

    for (int i = 0;i < rds_len;i += 4)
    {
        if (i >= RADIOTEXT_SIZE)
        {
            break;  // Prevent RDS buffer overflow. Mod By TEB, Jan-31-2022.
        }

        uint8_t ptyHi = (ptyCode & 0x18) >> 3;  // top 2 bits of PTY are in bottom 2 bits of byte 3, Mod By dkulp,
                                                // Jun-13-2022
        uint8_t ptyLo = (ptyCode << 5) & 0xE0;  // bottom 3 bits of PTY are in top 3 bits of byte 4, Mod By dkulp,
                                                // Jun-13-2022
        sendRDS (
            highByte (piCode),
            lowByte (piCode),
            0x20 | ptyHi,
            ptyLo | (i / 4),
            char_array[i],
            char_array[i + 1],
            char_array[i + 2],
            char_array[i + 3]);
        waitForRDSSend ();
    }
}

// Written By ManojBhakarPCM.
