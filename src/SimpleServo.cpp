#include <SimpleServo.h>
#include <Arduino.h>

bool SimpleServo::s_is_initialized = false;

volatile SimpleServo* SimpleServo::s_servos[MAX_SERVO_COUNT];
volatile uint8_t SimpleServo::s_index = 0;
volatile uint8_t SimpleServo::s_count = 0;
volatile uint16_t SimpleServo::s_sum_of_impulses = 0;
volatile bool SimpleServo::s_is_period_ended = true;

void SimpleServo::init() {

    cli();

    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << CS11); // Prescaler 8

    TIMSK1 |= (1 << OCIE1A); // Interrupt mask, set interrupt on compare
    OCR1A = TCNT1 + PERIOD_TICKS;

    sei();

    s_is_initialized = true;
}

uint8_t SimpleServo::evaluate_angle180(uint8_t angle) {

    if (angle > MAX_SERVO_ANGLE) {

        return  MAX_SERVO_ANGLE;
    }

    return  angle;
}

void SimpleServo::attach(uint8_t pin, uint8_t startAngle) {

    if (!s_is_initialized) {

        init();
    }

    if (pin < 0 || pin > 13) {

        return;
    }

    if (s_count < MAX_SERVO_COUNT) {

        _data.ticks = map(evaluate_angle180(startAngle), MIN_SERVO_ANGLE, MAX_SERVO_ANGLE,
                          MIN_IMPULSE_LENGTH, MAX_IMPULSE_LENGTH) * MICROSECONDS_TO_TICKS_SCALER;
        pinMode(pin, OUTPUT);

        if (pin < 8) {

            _data.pointer_register = (uint8_t*)&PORTD;
            _data.pin_mask = (1 << pin);
        }
        else {

            _data.pointer_register = (uint8_t*)&PORTB;
            _data.pin_mask = (1 << (pin - 8));
        }

        cli();
        s_servos[s_count] = this;
        s_count++;
        sei();
    }
}

void SimpleServo::attach(uint8_t pin) {

    attach(pin, DEFAULT_START_ANGLE);
}

void SimpleServo::set_angle(uint8_t angle) {

    uint16_t ticks = map(evaluate_angle180(angle), MIN_SERVO_ANGLE, MAX_SERVO_ANGLE,
        MIN_IMPULSE_LENGTH, MAX_IMPULSE_LENGTH) * MICROSECONDS_TO_TICKS_SCALER;

    cli();

    _data.ticks = ticks;

    sei();
}

void SimpleServo::on_interrupt() {

    if (s_count == 0) {

        OCR1A +=  PERIOD_TICKS;
        return;
    }

    if (s_is_period_ended) {

        s_is_period_ended = false;
        *(s_servos[s_index]->_data.pointer_register) |= s_servos[s_index]->_data.pin_mask;
        s_sum_of_impulses = s_servos[s_index]->_data.ticks;
        OCR1A += s_servos[s_index]->_data.ticks;

        return;
    }

    *(s_servos[s_index]->_data.pointer_register) &= ~(s_servos[s_index]->_data.pin_mask);
    s_index++;

    if (s_index == s_count) {

        s_is_period_ended = true;
        s_index = 0;

        if (PERIOD_TICKS - s_sum_of_impulses < MIN_SAFE_GAP)
            on_interrupt();
        else
            OCR1A += (PERIOD_TICKS - s_sum_of_impulses);

        return;
    }

    s_sum_of_impulses += s_servos[s_index]->_data.ticks;
    OCR1A += s_servos[s_index]->_data.ticks;
    *(s_servos[s_index]->_data.pointer_register) |= s_servos[s_index]->_data.pin_mask;
}

ISR(TIMER1_COMPA_vect) {

    SimpleServo::on_interrupt();
}