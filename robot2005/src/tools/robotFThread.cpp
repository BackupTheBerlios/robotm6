#include "robotFThread.h"

namespace ftThread {
    static ft_scheduler_t scheduler_=NULL;
    // ----------------------------------------------------------------------
    // init
    // ----------------------------------------------------------------------
    // Cree un ft_scheduler a premiere fois qu'on l'appelle 
    // ----------------------------------------------------------------------
    ft_scheduler_t init()
    {
#ifdef USE_FTHREAD
	if (!scheduler_) scheduler_ = ft_scheduler_create();
#endif
	return scheduler_;
    }
    // ----------------------------------------------------------------------
    // getScheduler
    // ----------------------------------------------------------------------
    // Renvoie le scheduler 
    // ----------------------------------------------------------------------
    ft_scheduler_t getScheduler()
    {
	return init();
    }

    // ----------------------------------------------------------------------
    // cleanup
    // ----------------------------------------------------------------------
    // end of main
    // ----------------------------------------------------------------------
    void cleanup()
    {
#ifdef USE_FTHREAD
	ft_exit();
#endif
    }
};
