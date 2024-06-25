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
                .AddAttribute("Rtarget_mult",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            DoubleValue(1.5),
                            MakeDoubleAccessor(&TcpQtOptimal::SetRtarget_mult),
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
    m_rttProp (Time::Max ()),
    m_rttPropStamp (Simulator::Now ()),
    m_probeRttPropStamp (Simulator::Now ()),
    m_cntRtt(0),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::TcpQtOptimal (const TcpQtOptimal& sock)
  : TcpNewReno (sock),
    m_predictedBytesInFlight (sock.m_predictedBytesInFlight),
    m_baseRtt (sock.m_baseRtt),
    m_rttProp (sock.m_rttProp),
    m_rttPropStamp (sock.m_rttPropStamp),
    m_probeRttPropStamp (sock.m_probeRttPropStamp),
    m_cntRtt (sock.m_cntRtt),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::~TcpQtOptimal (void)
{
    NS_LOG_FUNCTION (this);
}

void TcpQtOptimal::SetRtarget_mult (double Rtarget_mult)
{
    m_Rtarget_mult = Rtarget_mult;
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

    if (rtt <= m_rttProp)
    {
        m_rttProp = rtt;
        m_rttPropStamp = Simulator::Now ();
    }

  NS_LOG_DEBUG ("Updated m_rttProp = " << m_rttProp);

  if (m_baseRtt.GetSeconds () == 0) m_baseRtt = m_rttProp;

  m_baseRtt = std::min (m_baseRtt, rtt);
  NS_LOG_DEBUG ("Updated m_baseRtt = " << m_baseRtt);

  // Update RTT counter
  m_cntRtt++;
  NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);
}

void
TcpQtOptimal::CongestionStateSet(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this << tcb << newState);
}


void
TcpQtOptimal::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    if (Simulator::Now () - m_rttPropStamp >= m_rttPropFilterLen && !m_probeRtt)
    {
        m_probeRtt = true;
        m_probeRttPropStamp = Simulator::Now ();
        m_priorCwnd = tcb->m_cWnd;
        tcb->m_cWnd = 4 * tcb->m_segmentSize;
        m_rttProp = Time::Max ();
    }
    if (Simulator::Now () - m_probeRttPropStamp >= m_probeRttDuration && m_probeRtt)
    {
        m_probeRtt = false;
        m_rttPropStamp =Simulator::Now ();
        tcb->m_cWnd = m_priorCwnd;
    }

    if (!m_probeRtt)
    {
        if (tcb->m_lastAckedSeq >= m_begSndNxt)
        { // as in Vegas, we do QtOptimal cwnd adjustment every RTT.
          NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");    

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
                // Save the current right edge for next ModNewReno cycle
                m_begSndNxt = tcb->m_nextTxSequence;
                double Rmin = m_rttProp.GetSeconds (); //m_rtProp.GetSeconds ();
                double R = tcb->m_lastRtt.Get ().GetSeconds ();
      
                if (R > 1.01*Rmin)
                    tcb->m_ssThresh = std::max (std::min (tcb->m_ssThresh.Get (), 
                      tcb->m_cWnd.Get () - tcb->m_segmentSize), 2 * tcb->m_segmentSize);

                tcb->m_cWnd = ComputeQtCwnd (tcb);
                m_cntRtt = 0;
            }

            // Reset cntRtt & minRtt every RTT
            //m_rttProp = Time::Max();
            
        }
        else if (tcb->m_cWnd < tcb->m_ssThresh)
        {
            //this mimicks max-min resource allocation
            // with if R > Rt
           TcpNewReno::SlowStart(tcb, segmentsAcked);
        }
    }

}

uint32_t
TcpQtOptimal::ComputeQtCwnd (Ptr<TcpSocketState> tcb)
{

        //R^s \simeq R^\text{target}
        double Rmin = m_rttProp.GetSeconds (); //m_rtProp.GetSeconds ();
        double Rt = m_Rtarget_mult*Rmin;

        //measured RTT, PacketsInFlight and CwndInSegments
        double R = tcb->m_lastRtt.Get ().GetSeconds ();
        uint32_t L = tcb->m_bytesInFlight/tcb->m_segmentSize;
        uint32_t W = tcb->GetCwndInSegments (); //cwnd in packets

        // predict desired PacketsInFlight
        uint32_t Lpred = L + W - W*R/Rt;
        if (Lpred == L) Lpred = L + 1;
        Lpred = std::max((int) Lpred, 2);

        //compute desired cwnd
        uint32_t Wnew = W + std::ceil(Rmin*((double) Lpred - (double) L)/R);
        if (Wnew == W) Wnew = W + 1; //required for persistent excitation
        Wnew = std::max((int) Wnew, 2);

        return Wnew * tcb->m_segmentSize;
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