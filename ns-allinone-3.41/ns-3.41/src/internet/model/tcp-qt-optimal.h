 /*
 * Copyright (c) 2024 Dumisa Ngwenya <dumisa@crocs.co.za; ngwenyad@sentech.co.za>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Developed as part of PhD work on TCP Congestion Control
 * Dumisa W. Ngwenya - University of Pretoria - Department of Electrical, Electronic and Computer Engineering
 * 
 * Adapted from TcpVegas but inherited from TcpNewReno
 *
 */


#ifndef TCP_QTOPTIMAL_H
#define TCP_QTOPTIMAL_H


/**
 * \defgroup tcp-qtoptimal Description of the tcp-qtoptimal
 */

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"
#include <deque>
#include <eigen3/Eigen/Eigen>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>

namespace ns3
{

// Each class should be documented using Doxygen,
// and have an \ingroup tcp-qtoptimal directive

class TcpSocketState;

/**
* \brief tcp-qtoptimal implementation
	 *
	 *
	 * 
	 */
class TcpQtOptimal : public TcpNewReno
{
	public:
	  /**
	   * \brief Get the type ID.
	   * \return the object TypeId
	   */
		static TypeId GetTypeId ();

		TcpQtOptimal ();

	  /**
	   * \brief Copy constructor.
	   * \param sock object to copy.
	   */
		TcpQtOptimal (const TcpQtOptimal& sock);

		~TcpQtOptimal () override;

		std::string GetName () const;

		void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
	                      const Time& rtt);
		    /**
     * \brief Enable/disable Vegas algorithm depending on the congestion state
     *
     * We only start a Vegas cycle when we are in normal congestion state (CA_OPEN state).
     *
     * \param tcb internal congestion state
     * \param newState new congestion state to which the TCP is going to switch
     */
    	void CongestionStateSet(Ptr<TcpSocketState> tcb,
                            const TcpSocketState::TcpCongState_t newState) override;
		void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
		//virtual void IncreaseWindow_option1 (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
		uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, 
								uint32_t bytesInFlight) override;

		virtual Ptr<TcpCongestionOps> Fork () override;

		void SetRt_mult (double Rt_mult);
		void SetFairnessIndex (bool fairness_index);

	  private:
	    /**
	     * \as in Vegas, brief Enable QtOptimal algorithm to start taking QtOptimal samples
	     *
	     * as in Vewgas, QtOptimal algorithm is enabled in the following situations:
	     * 1. at the establishment of a connection
	     * 2. after an RTO
	     * 3. after fast recovery
	     * 4. when an idle connection is restarted
	     *
	     * \param tcb internal congestion state
	     */
	    void EnableQtOptimal(Ptr<TcpSocketState> tcb);

	    /**
	     * \as in Vegas, brief Stop taking QtOptimal samples
	     */
	    void DisableQtOptimal();


	private:
	  Time m_baseRtt;                    //!< Minimum of all ModNewReno RTT measurements seen during connection
	  Time m_minRtt;                     //!< Minimum of all RTT measurements within last RTT
	  uint32_t m_cntRtt;                 //!< Number of RTT measurements during last RTT
	  double m_Rt_mult;
	  bool m_fairness_index;
	  bool m_doingQtOptimalNow;         //!< If true, do Vegas for this RTT
	  SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT
	  

};



} //end namespace ns3

#endif /* TCP_QTOPTIMAL_H */

