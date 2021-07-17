/*
 *  Based on Smoothed.h
 *  Store and calculate smoothed values from sensors.
 *  Created by Matt Fryer on 2017-11-17, rewritten by Christian Piper (CAP1Sup) on 2020-5-10
 *  Licensed under LGPL
 */

#pragma once

// Standard naming conventions
#include "stdint.h"

// Import the config
#include "config.h"

// A class used to store and calculate the values to be smoothed.
// T - type of readings (readings[i] - elements of array).
// total_T - type of the total (runningTotal) of the array.
// If the readingsFactor is small and readings is small, T and total_T maybe of the same type without the runningTotal overflow.
// For example, template <int16_t T, int16_t total_T> or template <float T, float total_T>
// If the readingsFactor is large or readings are large, total_T must has larger size than T size.
// For example, template <int16_t T, int32_t total_T> or template <float T, double total_T>
template <typename T, typename total_T>
class MovingAverage {
  private:
    uint16_t readingsFactor = DEFAULT_AVG_FACTOR; // The smoothing factor. In average mode, this is the number of readings to average.
    uint16_t readingsPosition = 0; // Current position in the array
    uint16_t readingsNum = 0; // Number of readings currently being averaged
    T *readings; // Array of readings
                 // readings[readingsFactor]
                 // readings are stored in an array in an unusual sequence to win/remove one subtraction in code
                 // X(0), X(readingsFactor-1), ..., X(2), X(1)

    // ! Dynamically decide which type to use for the total
    total_T runningTotal = 0; // A cache of the total of the array, speeds up getting the average

  public:
    MovingAverage();
    ~MovingAverage(); // Destructor to clean up when class instance killed
    void begin(uint16_t smoothFactor = DEFAULT_AVG_FACTOR);
    void add(T newReading);
    T get();            // Returns the smoothed result in same to type of readings
    double getDouble(); // Returns the smoothed result as a double. Useful if the readings are of an integer type
    float getFloat();   // Returns the smoothed result as a float. Useful if the readings are of an integer type. Usually faster than the getDouble()
    T getLast();
    void clear();
};


// Constructor
template <typename T, typename total_T>
MovingAverage<T, total_T>::MovingAverage () {}


// Destructor
template <typename T, typename total_T>
MovingAverage<T, total_T>::~MovingAverage () { // Destructor
    delete[] readings;
}


// Initialize the array for storing sensor values
template <typename T, typename total_T>
void MovingAverage<T, total_T>::begin (uint16_t smoothFactor) {

    // Store the number of readings in the array
    readingsFactor = smoothFactor;

    // Create the actual array of the required size
    readings = new T[readingsFactor];

    // Initialise all the values in the array to zero
    for (uint16_t thisReading = 0; thisReading < readingsFactor; thisReading++) {
        readings[thisReading] = 0;
    }
}


// Add a value to the array
template <typename T, typename total_T>
void MovingAverage<T, total_T>::add (T newReading) {

    // Keep record of the number of readings being averaged
    // This will count up to the array size then stay at that number
    if(readingsNum < readingsFactor) {
        readingsNum++;
    }
    else {
        // Remove the old value from the running total
        runningTotal -= readings[readingsPosition];
    }

    // Add the new value to the running total
    runningTotal += newReading;

    // Store immediate value in the array
    readings[readingsPosition] = newReading;

    #ifdef USE_POWER_2_FACTOR_AVGING
    readingsPosition--;
    readingsPosition &= readingsFactor - 1;
    #else
    // If at the begin of the array
    if (readingsPosition == 0) {

        // Set position to the end of the array
        readingsPosition = readingsFactor - 1;
    }
    else {
        // Decrement to previous array position
        readingsPosition--;
    }
    #endif
}


// Get the smoothed result in same to type of readings
template <typename T, typename total_T>
T MovingAverage<T, total_T>::get() {
    return (runningTotal / readingsNum);
}


// Get the smoothed result as double type
template <typename T, typename total_T>
double MovingAverage<T, total_T>::getDouble() {
    return (double)runningTotal / readingsNum;
}


// Get the smoothed result as float type
template <typename T, typename total_T>
float MovingAverage<T, total_T>::getFloat() {
    return (float)runningTotal / readingsNum;
}


// Gets the last result stored
template <typename T, typename total_T>
T MovingAverage<T, total_T>::getLast() {

    // Just return the last reading
    if (readingsPosition == 0) {
        return readings[readingsFactor - 1];
    }
    else {
        return readings[readingsPosition + 1];
    }
}


// Clears all stored values
template <typename T, typename total_T>
void MovingAverage<T, total_T>::clear () {

    // Reset the counters
    readingsPosition = 0;
    readingsNum = 0;
    runningTotal = 0;
}