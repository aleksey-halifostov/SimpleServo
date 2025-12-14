#ifndef SIMPLESERVO_SIMPLESERVO_H
#define SIMPLESERVO_SIMPLESERVO_H

#include <stdint.h>

constexpr uint16_t MAX_SERVO_COUNT = 10;
constexpr uint16_t MICROSECONDS_TO_TICKS_SCALER = 2;

// Angle limits
constexpr uint16_t MIN_SERVO_ANGLE = 0;
constexpr uint16_t MAX_SERVO_ANGLE = 180;

// Default start angle
constexpr uint16_t DEFAULT_START_ANGLE = 90;

// Impulse limits
// All time scales in microseconds
constexpr uint16_t MIN_IMPULSE_LENGTH = 1000;
constexpr uint16_t MAX_IMPULSE_LENGTH = 2000;

// Impulse period limit
constexpr uint16_t PERIOD_TICKS = 40000u;
constexpr uint16_t MIN_SAFE_GAP = 150;

typedef struct ConnectionData{

    uint8_t *pointer_register;
    uint8_t pin_mask;
    uint16_t ticks;
} cond_t;

class SimpleServo {

    public:
        void attach(uint8_t pin);
        void attach(uint8_t pin, uint8_t startAngle);
        void set_angle(uint8_t angle);

        static void on_interrupt();

    private:
        volatile cond_t _data{};

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