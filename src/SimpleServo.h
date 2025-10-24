#ifndef SIMPLESERVO_SIMPLESERVO_H
#define SIMPLESERVO_SIMPLESERVO_H

#define MAX_SERVO_COUNT 8
#define MICROSECONDS_TO_TICKS_SCALER 2

// Angle limits
#define MIN_SERVO_ANGLE 0
#define MAX_SERVO_ANGLE 180

// Default start angle
#define DEFAULT_START_ANGLE 90

// Impulse limits
// All time scales in microseconds
#define MIN_IMPULSE_LENGTH 500
#define MAX_IMPULSE_LENGTH 2500

// Impulse period limit
#define PERIOD_TICKS 40000u

#include <stdint.h>

typedef struct {

    volatile uint8_t *pointer_register;
    volatile uint8_t pin_mask;
    volatile uint16_t ticks;
} connection_t;

class SimpleServo {

    public:
        void attach(uint8_t pin);
        void attach(uint8_t pin, uint8_t startAngle);
        void setAngle(uint8_t angle);

        static void onInterrupt();

    private:
        static  bool isInitialized;
        static volatile uint8_t index;
        static volatile uint8_t count;
        static volatile uint16_t sumOfImpulses;
        static volatile bool isPeriodEnded;
        static volatile SimpleServo* servos[];

        static void init();

        connection_t _data{};
};

#endif //SIMPLESERVO_SIMPLESERVO_H