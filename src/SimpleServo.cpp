#include <SimpleServo.h>
#include <Arduino.h>

bool SimpleServo::_isInitialized = false;

volatile SimpleServo* SimpleServo::servos[MAX_SERVO_COUNT];
volatile uint8_t SimpleServo::index = 0;
volatile uint8_t SimpleServo::count = 0;
volatile uint16_t SimpleServo::sumOfImpulses = 0;
volatile bool SimpleServo::isPeriodEnded = true;

void SimpleServo::init() {

    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << WGM12); // CTC
    TCCR1B |= (1 << CS11); // Prescaler 8

    TIMSK1 |= (1 << OCIE1A); // Interrupt mask, set interrupt on compare
    OCR1A = PERIOD_TICKS; // Default interrupt value

    _isInitialized = true;
}

uint8_t evaluateAngle180(uint8_t angle) {

    if (angle > MAX_SERVO_ANGLE) {

        return  MAX_SERVO_ANGLE;
    }

    return  angle;
}

void SimpleServo::attach(uint8_t pin, uint8_t startAngle) {

    if (!_isInitialized) {

        init();
    }

    if (count < MAX_SERVO_COUNT) {

        _data.pin = pin;
        _data.ticks = map(evaluateAngle180(startAngle), MIN_SERVO_ANGLE, MAX_SERVO_ANGLE,
            MIN_IMPULSE_LENGTH, MAX_IMPULSE_LENGTH) * MICROSECONDS_TO_TICKS_SCALER;
        pinMode(_data.pin, OUTPUT);

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
        return;
    }

    if (isPeriodEnded) {

        isPeriodEnded = false;
        digitalWrite(servos[index]->_data.pin, HIGH);
        sumOfImpulses = servos[index]->_data.ticks;
        OCR1A = servos[index]->_data.ticks;
        index = 1;

        return;
    }

    digitalWrite(servos[index]->_data.pin, LOW);
    index++;

    if (index == count) {

        OCR1A = PERIOD_TICKS - sumOfImpulses;
        index = 0;
        isPeriodEnded = true;

        return;
    }

    sumOfImpulses += servos[index]->_data.ticks;
    OCR1A = servos[index]->_data.ticks;
    digitalWrite(servos[index]->_data.pin, HIGH);
}

ISR(TIMER1_COMPA_vect) {

    SimpleServo::onInterrupt();
}