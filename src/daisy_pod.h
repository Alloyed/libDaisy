#pragma once
#ifndef DSY_POD_BSP_H
#define DSY_POD_BSP_H

#include "daisy_seed.h"

namespace daisy
{
/**
    @brief Class that handles initializing all of the hardware specific to the Daisy Patch Board. \n 
    Helper funtions are also in place to provide easy access to built-in controls and peripherals.
    @author Stephen Hensley
    @date November 2019
    @ingroup boards
*/
class DaisyPod
{
  public:
    /** Switches */
    enum Sw
    {
        BUTTON_1,    /** & */
        BUTTON_2,    /** & */
        BUTTON_LAST, /** &  */
    };

    /** Knobs */
    enum Knob
    {
        KNOB_1,    /** &  */
        KNOB_2,    /** & */
        KNOB_LAST, /** & */
    };

    DaisyPod() {}
    ~DaisyPod() {}

    /** Init related stuff. */
    void Init();

    /** Wait for a bit
    \param del Time to wait in ms.
    */
    void DelayMs(size_t del);

    /** Starts the callback
    \cb Interleaved callback function
    */
    void StartAudio(AudioHandle::InterleavingAudioCallback cb);

    /** Starts the callback
    \cb multichannel callback function
    */
    void StartAudio(AudioHandle::AudioCallback cb);

    /**
       Switch callback functions
       \param cb New interleaved callback function.
    */
    void ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb);

    /**
       Switch callback functions
       \param cb New multichannel callback function.
    */
    void ChangeAudioCallback(AudioHandle::AudioCallback cb);

    /** Stops the audio if it is running. */
    void StopAudio();

    /** Updates the Audio Sample Rate, and reinitializes.
     ** Audio must be stopped for this to work.
     */
    void SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate);

    /** Returns the audio sample rate in Hz as a floating point number.
     */
    float AudioSampleRate();

    /** Sets the number of samples processed per channel by the audio callback.
     */
    void SetAudioBlockSize(size_t blocksize);

    /** Returns the number of samples per channel in a block of audio. */
    size_t AudioBlockSize();

    /** Returns the rate in Hz that the Audio callback is called */
    float AudioCallbackRate();

    /**
       Start analog to digital conversion.
     */
    void StartAdc();
	
	/** Stops Transfering data from the ADC */
    void StopAdc();

    /** Call at same rate as analog reads for smooth reading.*/
    void ProcessAnalogControls();

    /** Does what it says */
    inline void ProcessAllControls() { ProcessAnalogControls(); }


    /** & */
    float GetKnobValue(Knob k);

    /** & */
    void DebounceControls();

    /** Reset Leds*/
    void ClearLeds();

    /** Update Leds to set colors*/
    void UpdateLeds();

    /** Public Members */
    DaisySeed     seed;        /**<# */
    Encoder       encoder;     /**< & */
    AnalogControl knob1,       /**< & */
        knob2,                 /**< & */
        *knobs[KNOB_LAST];     /**< & */
    Switch button1,            /**< & */
        button2,               /**< & */
        *buttons[BUTTON_LAST]; /**< & */
    RgbLed led1,               /**< & */
        led2;                  /**< & */

  private:
    void   InitButtons();
    void   InitEncoder();
    void   InitLeds();
    void   InitKnobs();
    float  sample_rate_;
    size_t block_size_;
};

} // namespace daisy
#endif
