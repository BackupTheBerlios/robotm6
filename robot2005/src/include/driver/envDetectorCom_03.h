/**
 * @brief Protocole de communication avec la carte detecteur d'environement
 *
 * @author Laurent Saint-Marcel
 * @date   2003/05/10
 */

#ifndef __ENV_DETECTOR_COM_03_H__
#define __ENV_DETECTOR_COM_03_H__

/** 
 * @brief Requete de ping, la reponse doit etre ENV_DETECTOR_BOARD_ID
 */
#define ENV_DETECTOR_PING      0xAA
/**
 * @brief Reponse a la requete ping
 */
#define ENV_DETECTOR_BOARD_ID  0xA9

/**
 * @brief Mode repos, +3 octets (sans signification)
 */
#define ENV_DETECTOR_ACKNOLEDGE      0x7B
/**
 * @brief Reponse en cas de timeout
 */
#define ENV_DETECTOR_TIMEOUT_ERROR   0x69
/**
 * @brief Mode repos, +3 octets (sans signification)
 */
#define ENV_DETECTOR_MODE_SLEEP      0x00
/** 
 * @brief Mode surveillance, + 3 octets: angle, nombre de mesures pour le 
 * moyennage, seuil 
 */
#define ENV_DETECTOR_MODE_WATCHDOG   0x01
/**
 * @brief Mode balayage, + 3 octets: angle1, angle2, delta 
 */
#define ENV_DETECTOR_MODE_RADAR      0x02
/**
 * @brief Requete de demande du mode, la reponse est composee de 4 octets: mode, 
 * param1, param2, param3
 */
#define ENV_DETECTOR_GET_MODE        0xF0
/**
 * @brief Fait une acquisition sur les capteurs et retourne les distances.
 * Recoit 2 octets, dist droite, dist gauche
 */
#define ENV_DETECTOR_GET_DIST               0xCA
/**
 * @brief Retourne les moyennes capturees en mode surveillance. Recoit 2 octets, 
 * dist droite, dist gauche
 */
#define ENV_DETECTOR_GET_WATCHDOG_MEAN_DIST 0xCB
/**
 * @brief Retourne la distance minimale detectee avec son angle associe.
 * Recoit 4 octets (angle droit, distance droite, angle gauche, distance gauche)
 */
#define ENV_DETECTOR_GET_RADAR_MIN_RIGHT    0xCC
#define ENV_DETECTOR_GET_RADAR_MIN_LEFT     0xCD

#define ENV_DETECTOR_ENABLE_EVENTS          0x63
#define ENV_DETECTOR_DISABLE_EVENTS         0x64
#define ENV_DETECTOR_EVENTS_DIR_BIT         0x80

#endif // __ENV_DETECTOR_COM_03_H__
