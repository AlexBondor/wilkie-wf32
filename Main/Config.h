// Motor encoders pins definitions
#define LEFT_ENCODER_INT_PIN         2 // INT1 on WF32
#define LEFT_ENCODER_DIGITAL_PIN    28
#define RIGHT_ENCODER_INT_PIN        7 // INT2 on WF32
#define RIGHT_ENCODER_DIGITAL_PIN   33

// Motor pins definitions
//
// InA == InB - fast brake
// InA != InB - forward/backward direction
//
#define LEFT_MOTOR_IN_A_PIN   4 // direction pin
#define LEFT_MOTOR_IN_B_PIN  30 // direction pin
#define LEFT_MOTOR_PWM_PIN    3 // enable pin used for PWM signal
#define RIGHT_MOTOR_IN_A_PIN  5 // direction pin
#define RIGHT_MOTOR_IN_B_PIN 31 // direction pin
#define RIGHT_MOTOR_PWM_PIN   6 // enable pin used for PWM signal

#define VACUUM_PIN 29

/*
 *	Proximity sensors and servo pins definitions
 */
#define SERVO_PIN 14
#define SHARP_1_PIN 15
#define SHARP_2_PIN 16
#define SHARP_3_PIN 17

// Bluetooth module software serial pins
#define BLUETOOTH_RX_PIN 39
#define BLUETOOTH_TX_PIN 40

// Used by bluetooth module to do callbacks when receives data from 
// app
typedef void (*VoidCallbackMethod)();
