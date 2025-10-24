#include <SimpleServo.h>
#include <Arduino.h>

bool SimpleServo::isInitialized = false;

volatile SimpleServo* SimpleServo::servos[MAX_SERVO_COUNT];
volatile uint8_t SimpleServo::index = 0;
volatile uint8_t SimpleServo::count = 0;
volatile uint16_t SimpleServo::sumOfImpulses = 0;
volatile bool SimpleServo::isPeriodEnded = true;

void SimpleServo::init() {

    cli();

    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << WGM12); // CTC
    TCCR1B |= (1 << CS11); // Prescaler 8

    TIMSK1 |= (1 << OCIE1A); // Interrupt mask, set interrupt on compare
    OCR1A = PERIOD_TICKS; // Default interrupt value

    sei();

    isInitialized = true;
}

uint8_t evaluateAngle180(uint8_t angle) {

    if (angle > MAX_SERVO_ANGLE) {

        return  MAX_SERVO_ANGLE;
    }

    return  angle;
}

void SimpleServo::attach(uint8_t pin, uint8_t startAngle) {

    if (!isInitialized) {

        init();
    }

    if (pin < 0 || pin > 13) {

        return;
    }

    if (count < MAX_SERVO_COUNT) {

        _data.ticks = map(evaluateAngle180(startAngle), MIN_SERVO_ANGLE, MAX_SERVO_ANGLE,
                          MIN_IMPULSE_LENGTH, MAX_IMPULSE_LENGTH) * MICROSECONDS_TO_TICKS_SCALER;
        pinMode(pin, OUTPUT);

        if (pin < 8) {

            _data.pointer_register = &PORTD;
            _data.pin_mask = (1 << pin);
        }
        else {

            _data.pointer_register = &PORTB;
            _data.pin_mask = (1 << (pin - 8));
        }

        cli();
        servos[count] = this;
        count++;
        sei();
    }
}

void SimpleServo::attach(uint8_t pin) {

    attach(pin, DEFAULT_START_ANGLE);
}

void SimpleServo::setAngle(uint8_t angle) {

    uint16_t ticks = map(evaluateAngle180(angle), MIN_SERVO_ANGLE, MAX_SERVO_ANGLE,
        MIN_IMPULSE_LENGTH, MAX_IMPULSE_LENGTH) * MICROSECONDS_TO_TICKS_SCALER;

    cli();

    _data.ticks = ticks;

    sei();
}

void SimpleServo::onInterrupt() {

    if (count == 0) {

        OCR1A = PERIOD_TICKS;
        return;
    }

    if (isPeriodEnded) {

        isPeriodEnded = false;
        *(servos[index]->_data.pointer_register) |= servos[index]->_data.pin_mask;
        sumOfImpulses = servos[index]->_data.ticks;
        OCR1A = servos[index]->_data.ticks;

        return;
    }

    *(servos[index]->_data.pointer_register) &= ~(servos[index]->_data.pin_mask);
    index++;

    if (index == count) {

        OCR1A = PERIOD_TICKS - sumOfImpulses;
        index = 0;
        isPeriodEnded = true;

        return;
    }

    sumOfImpulses += servos[index]->_data.ticks;
    OCR1A = servos[index]->_data.ticks;
    *(servos[index]->_data.pointer_register) |= servos[index]->_data.pin_mask;
}

ISR(TIMER1_COMPA_vect) {

    SimpleServo::onInterrupt();
}