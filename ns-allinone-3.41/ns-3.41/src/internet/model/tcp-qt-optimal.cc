
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
 * Adapted from TCP Vegas
 *
 */

#include "tcp-qt-optimal.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include <algorithm>
#include <random>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("TcpQtOptimal");
NS_OBJECT_ENSURE_REGISTERED(TcpQtOptimal);

TypeId TcpQtOptimal::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpQtOptimal")
                            .SetParent<TcpNewReno>()
                            .AddConstructor<TcpQtOptimal>()
                            .SetGroupName("Internet")
                .AddAttribute("Rt_mult",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            DoubleValue(1.5),
                            MakeDoubleAccessor(&TcpQtOptimal::SetRt_mult),
                            MakeDoubleChecker<double>(0.0))
                .AddAttribute("fairness_index",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            BooleanValue(false),
                            MakeBooleanAccessor(&TcpQtOptimal::SetFairnessIndex),
                            MakeBooleanChecker ())
                .AddTraceSource("PredictedBytesInFlight",
                            "The TCP connection's congestion window",
                            MakeTraceSourceAccessor(&TcpQtOptimal::m_predictedBytesInFlight),
                            "ns3::TracedValueCallback::Uint32");
    return tid;
}

TcpQtOptimal::TcpQtOptimal ()
  : TcpNewReno(),
    m_predictedBytesInFlight (0),
    m_baseRtt (Time::Max ()),
    m_minRtt (Time::Max ()),
    m_cntRtt(0),
    m_doingQtOptimalNow (true),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::TcpQtOptimal (const TcpQtOptimal& sock)
  : TcpNewReno (sock),
    m_predictedBytesInFlight (sock.m_predictedBytesInFlight),
    m_baseRtt (sock.m_baseRtt),
    m_minRtt (sock.m_minRtt),
    m_cntRtt (sock.m_cntRtt),
    m_doingQtOptimalNow (true),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::~TcpQtOptimal (void)
{
    NS_LOG_FUNCTION (this);
}

void TcpQtOptimal::SetRt_mult (double Rt_mult)
{
    m_Rt_mult = Rt_mult;
}

void TcpQtOptimal::SetFairnessIndex (bool fairness_index)
{   
    m_fairness_index = fairness_index;
}

void
TcpQtOptimal::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                     const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);

  if (rtt.IsZero ())
    {
      return;
    }

  m_minRtt = std::min (m_minRtt, rtt);
  NS_LOG_DEBUG ("Updated m_minRtt = " << m_minRtt);

  if (m_baseRtt.GetSeconds () == 0) m_baseRtt = m_minRtt;

  m_baseRtt = std::min (m_baseRtt, rtt);
  NS_LOG_DEBUG ("Updated m_baseRtt = " << m_baseRtt);

  // Update RTT counter
  m_cntRtt++;
  NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);
}

void
TcpQtOptimal::EnableQtOptimal(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    m_doingQtOptimalNow = true;
    m_begSndNxt = tcb->m_nextTxSequence;
    m_cntRtt = 0;
    //m_minRtt = Time::Max();
}

void
TcpQtOptimal::DisableQtOptimal()
{
    NS_LOG_FUNCTION(this);

    m_doingQtOptimalNow = false;
}

void
TcpQtOptimal::CongestionStateSet(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this << tcb << newState);
    if (newState == TcpSocketState::CA_OPEN)
    {
        EnableQtOptimal(tcb);
    }
    else
    {
        DisableQtOptimal();
    }
}


void
TcpQtOptimal::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    if (!m_doingQtOptimalNow)
    {
        // as in vegas, if QtOptimal is not on, we follow NewReno algorithm
        NS_LOG_LOGIC("QtOptimal is not turned on, we follow NewReno algorithm.");
        TcpNewReno::IncreaseWindow(tcb, segmentsAcked);
        return;
    }

    if (tcb->m_lastAckedSeq >= m_begSndNxt)
    { // as in Vegas, we do QtOptimal cwnd adjustment every RTT.
      NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");
        // Save the current right edge for next ModNewReno cycle

        /*
         * as in Vegas, we perform QtOptimal calculations only if we got enough RTT samples to
         * insure that at least 1 of those samples wasn't from a delayed ACK.
         */
        if (m_cntRtt <= 2)
        { // We do not have enough RTT samples, so we should behave like Reno
            NS_LOG_LOGIC(
                "We do not have enough RTT samples to do QtOptimal, so we behave like NewReno.");
            TcpNewReno::IncreaseWindow(tcb, segmentsAcked);
        }
        else
        {
            m_begSndNxt = tcb->m_nextTxSequence;
  
            //R^s \simeq R^\text{target}
            double Rt = m_Rt_mult*m_minRtt.GetSeconds ();
            double Rmin = m_minRtt.GetSeconds ();

            //measured RTT (average response time)
            double R = tcb->m_lastRtt.Get ().GetSeconds ();
            //std::cout << "Rtt=" << R << " Rt=" << Rt << " Rmin=" << Rmin << std::endl;
            //getchar ();
            // measured average occupancy of the system
            uint32_t L = tcb->m_bytesInFlight/tcb->m_segmentSize;
            uint32_t W = tcb->GetCwndInSegments (); //cwnd in packets
            // predict average occupancy
            uint32_t Lpred = L + W - W*R/Rt;
            if (Lpred == L) Lpred = L + 1;
            Lpred = std::max((int) Lpred, 2);
            m_predictedBytesInFlight = Lpred;

            uint32_t Wnew = W + std::ceil(Rmin*((double) Lpred - (double) L)/R);

            if (Wnew == W) Wnew = W + 1; //required for persistent excitation
            Wnew = std::max((int) Wnew, 2);

            if (m_fairness_index)
            {            
                static std::default_random_engine generator;
                std::uniform_real_distribution<double> distribution(0.0,1.0);
                if (distribution(generator) > 0.90) 
                {
                    //uncomment for multiflow fairness tests
                    Wnew = 40; //static figure for the specific simulation where Bw=10Mbps
                               // and one-way-delay = 100ms
                               // dynamic mechanisms to be implemented in the future
                }
                //std::cout << "fainess = " << m_fairness_index << std::endl;
                //std::getchar ();
            }

            //for debugging
/*              std::cout << std::endl << "time = " << Simulator::Now().GetSeconds() << ": " << std::endl
                  //<< "W0 = " << W0 << std::endl
                  << "L = " << L << std::endl
                  << "W = " << W << std::endl
                  << "RTT = " << R << std::endl
                  << " W*Rtarget/R = " <<  W*R/R << std::endl
                  << "Rtarget*((double) Lpred - (double) L)/R = " 
                  <<  Rt*((double) Lpred - (double) L)/R << std::endl
                  << "m_cntDec = " << m_cntDec << std::endl
                  << "Lpred = " << Lpred << std::endl
                  << "Wnew = " << Wnew << std::endl
                  << "tcb->m_cWnd = " << tcb->m_cWnd << std::endl;
                  //<< "m_maxRTT_prefer = " <<1.2*m_baseRtt.GetSeconds () << std::endl;
                  
            getchar ();*/
            if (R > 1.01*Rmin)
                tcb->m_ssThresh = std::max (std::min (tcb->m_ssThresh.Get (), 
                  tcb->m_cWnd.Get () - tcb->m_segmentSize), 2 * tcb->m_segmentSize);

            tcb->m_cWnd = Wnew * tcb->m_segmentSize;
            m_cntRtt = 0;
        }

        // Reset cntRtt & minRtt every RTT
        //m_minRtt = Time::Max();
        
    }
    else if (tcb->m_cWnd < tcb->m_ssThresh)
    {
        //this mimicks max-min resource allocation
        // with if R > Rt
       TcpNewReno::SlowStart(tcb, segmentsAcked);
    }

}

std::string
TcpQtOptimal::GetName () const
{
  return "TcpQtOptimal";
}

uint32_t
TcpQtOptimal::GetSsThresh (Ptr<const TcpSocketState> tcb,
                       uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << tcb << bytesInFlight);
  //return std::max (std::min (tcb->m_ssThresh.Get (), 
    //              tcb->m_cWnd.Get () - tcb->m_segmentSize), 2 * tcb->m_segmentSize);

  return TcpNewReno::GetSsThresh (tcb, bytesInFlight);
  //return std::max(2 * tcb->m_segmentSize, bytesInFlight / 2);
}

Ptr<TcpCongestionOps>
TcpQtOptimal::Fork ()
{
  return CopyObject<TcpQtOptimal> (this);
}


}
