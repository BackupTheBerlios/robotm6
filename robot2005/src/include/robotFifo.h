/**
 * @file robotFifo.h
 *
 * @author Laurent Saint-Marcel
 *
 * Utilitaire permettant de creer une fifo circulaire de taille fixe qui 
 * evite des allocations dynamiques comme le fait la stl. C'est plus sur en
 * terme de thread que la stl... Il y a deja eu des crashs du programme
 * avec la stl
 */

#ifndef __ROBOT_FIFO_H__
#define __ROBOT_FIFO_H__

/**
 * @class RobotFifo
 * Fifo circulaire dont la taille est fixee par K, Elle evite d'allouer 
 * dynamiquement de la memoire. Elle a ete cree suirte a une segFault dans
 * la STL sur les deque utilisees comme buffer pour les cartes uart
 */
template< typename T, int K=50>
class RobotFifo
{
	public:
		RobotFifo();
		~RobotFifo();
		void push_back(T const& v);
		void pop_front();
		T& front();
		bool empty() const;
		int size() const;
		void clear();
		void dump() const;
		int length() const;
	private:
		T array_[K];
		int iBegin_;
		int iEnd_;
};

// --------------------------------------------------------------------------
// RobotFifo<T, K>::clear
// --------------------------------------------------------------------------
template< typename T, int K>
inline void
RobotFifo<T, K>::clear()
{
	iBegin_=0;
        iEnd_=0;    
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::RobotFifo<T, K>
// --------------------------------------------------------------------------
template< typename T, int K>
inline
RobotFifo<T, K>::RobotFifo() :
	iBegin_(0), iEnd_(0)
{
	clear();
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::RobotFifo<T, K>
// --------------------------------------------------------------------------
template< typename T, int K>
inline
RobotFifo<T, K>::~RobotFifo()
{
}


// --------------------------------------------------------------------------
// RobotFifo<T, K>::push_back(T const& v)
// --------------------------------------------------------------------------
template< typename T, int K>
inline void
RobotFifo<T, K>::push_back(T const& v)
{
	if (iBegin_ <0 || iBegin_ >= K) iBegin_=0;
        array_[iBegin_]=v;
	iBegin_=(iBegin_+1)%K;
	if (iBegin_==iEnd_) {
		iEnd_=(iEnd_+1)%K;
	}
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::pop_front()
// --------------------------------------------------------------------------
template< typename T, int K>
inline void
RobotFifo<T, K>::pop_front()
{
    if (iEnd_ != iBegin_) {
        iEnd_=(iEnd_+1)%K;
    }
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::front()
// --------------------------------------------------------------------------
template< typename T, int K>
inline T&
RobotFifo<T, K>::front()
{
    if (iEnd_<0 || iEnd_ >= K) iEnd_=0;
    return array_[iEnd_];
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::empty()
// --------------------------------------------------------------------------
template< typename T, int K>
bool
RobotFifo<T, K>::empty() const
{
    return (iEnd_==iBegin_);
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::size()
// --------------------------------------------------------------------------
template< typename T, int K>
int
RobotFifo<T, K>::size() const
{
	if (iEnd_ <= iBegin_) return (iBegin_-iEnd_);
	else return (iBegin_ + K - iEnd_);
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::length()
// --------------------------------------------------------------------------
template< typename T, int K>
inline int
RobotFifo<T, K>::length() const
{
	return K;
}

// --------------------------------------------------------------------------
// RobotFifo<T, K>::dump() 
// --------------------------------------------------------------------------
template< typename T, int K>
void
RobotFifo<T, K>::dump() const
{
/*	printf("RobotFifo\n"
		   "  Max Size=%d\n"
		   "  iBegin_=%d\n"
		   "  iEnd_=%d\n",
               K, iBegin_, iEnd_);*/
}

#endif // __ROBOT_FIFO_H__
