#ifndef SIMPLESERVO_SIMPLESERVO_H
#define SIMPLESERVO_SIMPLESERVO_H

#define MAX_SERVO_COUNT 10
#define MICROSECONDS_TO_TICKS_SCALER 2

// Angle limits
#define MIN_SERVO_ANGLE 0
#define MAX_SERVO_ANGLE 180

// Default start angle
#define DEFAULT_START_ANGLE 90

// Impulse limits
// All time scales in microseconds
#define MIN_IMPULSE_LENGTH 1000
#define MAX_IMPULSE_LENGTH 2000

// Impulse period limit
#define PERIOD_TICKS 40000u
#define MIN_SAFE_GAP 150

#include <stdint.h>

typedef struct ConnectionData{

    volatile uint8_t *pointer_register;
    volatile uint8_t pin_mask;
    volatile uint16_t ticks;
} cond_t;

class SimpleServo {

    public:
        void attach(uint8_t pin);
        void attach(uint8_t pin, uint8_t startAngle);
        void set_angle(uint8_t angle);

        static void on_interrupt();

    private:
        cond_t _data{};

        static  bool s_is_initialized;
        static volatile uint8_t s_index;
        static volatile uint8_t s_count;
        static volatile uint16_t s_sum_of_impulses;
        static volatile bool s_is_period_ended;
        static volatile SimpleServo* s_servos[];

        uint8_t evaluate_angle180(uint8_t angle);

        static void init();

};

#endif //SIMPLESERVO_SIMPLESERVO_H