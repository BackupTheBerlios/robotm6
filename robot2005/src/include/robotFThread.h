#ifndef __ROBOT_FAIR_THREAD_H__
#define __ROBOT_FAIR_THREAD_H__

#include "fthread.h"

namespace ftThread {
    /** @brief Cree un ft_scheduler a premiere fois qu'on l'appelle */
    ft_scheduler_t init();
    /** @brief Renvoie le scheduler */
    ft_scheduler_t getScheduler(); 
    /** end of main */
    void cleanup();
};

#endif // __ROBOT_FAIR_THREAD_H__
