/**
 * @file simulatorBase.h
 *
 * @author Laurent Saint-Marcel
 *
 * Types, enum, const de commun a tous les elements du simulateur
 */

#ifndef __SIMULATOR_BASE_H__
#define __SIMULATOR_BASE_H__

// ----------------------------------------------------------------------------
// Type pour les objets simulation
// ----------------------------------------------------------------------------
typedef enum SimuMatchStatus {
    SIMU_STATUS_NEED_RESET=0,
    SIMU_STATUS_WAIT_START,
    SIMU_STATUS_MATCH,
    SIMU_STATUS_END
} SimuMatchStatus;

typedef enum SimuWeight {
    SIMU_WEIGHT_ULTRA_LIGHT=0,
    SIMU_WEIGHT_LIGHT,
    SIMU_WEIGHT_MEDIUM,
    SIMU_WEIGHT_HEAVY,
    SIMU_WEIGHT_ULTRA_HEAVY
} SimuWeight;

typedef enum SimuRequestType {
    SIMU_REQ_SET_NAME=0,
    SIMU_REQ_SET_WEIGHT,
    SIMU_REQ_SET_MODEL,
    SIMU_REQ_SET_MOTOR_COEF,
    SIMU_REQ_GET_STATUS,
    SIMU_REQ_SET_STATUS,
    SIMU_REQ_GET_JACKIN,
    SIMU_REQ_SET_JACKIN,
    SIMU_REQ_GET_EMERGENCY,
    SIMU_REQ_SET_EMERGENCY,
    SIMU_REQ_GET_LCD_BUTTONS,
    SIMU_REQ_SET_LCD_BUTTONS,
    SIMU_REQ_GET_PWM,
    SIMU_REQ_SET_PWM,
    SIMU_REQ_SET_MOTOR_SPEED,
    SIMU_REQ_GET_MOTOR,
    SIMU_REQ_SET_MOTOR,
    SIMU_REQ_GET_ESTIMATE_POS,
    SIMU_REQ_SET_ESTIMATE_POS,
    SIMU_REQ_GET_REAL_POS,
    SIMU_REQ_SET_REAL_POS,
    SIMU_REQ_SET_LCD,
    SIMU_REQ_GET_OBSTACLE_DIST,
    SIMU_REQ_SET_OBSTACLE_DIST,
    SIMU_REQ_GET_COLLISION,
    SIMU_REQ_SET_COLLISION,
    SIMU_REQ_GET_GROUND_DIST,
    SIMU_REQ_SET_GROUND_DIST,
    SIMU_REQ_SET_POS,
    SIMU_REQ_SET_BRICK,
    SIMU_REQ_SET_NORMAL, // !BRICK
};

// ----------------------------------------------------------------------------
// Liste des machines sur lesquels un client cherche le programme du simulateur
// ----------------------------------------------------------------------------
static const char SIMU_HOSTNAME_LOCALHOST[]    ="127.0.0.1";
static const char SIMU_HOSTNAME_PROJET5[]      ="10.0.0.2";
static const char SIMU_HOSTNAME_PORTABLE[]     ="10.0.0.2";

// ----------------------------------------------------------------------------
// Liste des ports sur lesquels un client cherche a se connecter au simulateur
// ----------------------------------------------------------------------------
static const unsigned int SIMU_PORT_MAX_CONNECTIONS = 4;
static const unsigned int SIMU_PORT = 1630;

static const unsigned int SIMU_BUFFER_LENGTH=255;


#endif // __SIMULATOR_BASE_H__
