/**
 * @file shm.h
 *
 * @author Laurent Saint-Marcel
 *
 * Fonctions utiles pour creer une shared memory
 */

#ifndef __ROBOT_SMH_H__
#define __ROBOT_SMH_H__

/** 
 * @namespace RobotShm
 * Fonctions utiles pour creer une shared memory 
 */
namespace RobotShm {
    
    /** 
     * @brief Cree une shared memory 
     *
     * @param shmKeyFile : key of the shm : "/var/tmp/..."
     *        Chaine de caractere qui defini la cle de la shared memory
     * @param mode 0644 or 0666
     *        Mode d'ouverture de la shared memory
     * @param shmSize  sizeOf(myShmStruct)
     *        Taille de la shared memory
     * @param dataPtr  (void*)(myShmStructPtr)
     *        Pointeur vers les donnees de la shared memory
     * @param existingShared is set to true if the shared memory was 
     *        already existing and is set to false if the shared memory 
     *        is created
     * @param createShmIfNotExisting is set to true if existingShared = false 
     *        then the shared memory is created
     *
     * @return 0 en cas de succes, (1..3) en cas d'erreur
     */
    int create(const char* shmKeyFile,
               int mode,
               int shmSize,
               void** dataPtr, 
               bool &existingShared,
               bool createShmIfNotExisting=true);
};

#endif // __SMH_H__
