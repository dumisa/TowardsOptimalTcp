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

#include "tcp-qt-col-fair.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include <algorithm>
#include <random>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("TcpQtColFair");
NS_OBJECT_ENSURE_REGISTERED(TcpQtColFair);

TypeId TcpQtColFair::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpQtColFair")
                            .SetParent<TcpNewReno>()
                            .AddConstructor<TcpQtColFair>()
                            .SetGroupName("Internet")
                .AddAttribute("rttTargetAlpha",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            DoubleValue(1.5),
                            MakeDoubleAccessor(&TcpQtColFair::SetRttTargetMultiplier),
                            MakeDoubleChecker<double>(0.0))
                .AddTraceSource("PredictedBytesInFlight",
                            "The TCP connection's congestion window",
                            MakeTraceSourceAccessor(&TcpQtColFair::m_predictedBytesInFlight),
                            "ns3::TracedValueCallback::Uint32")
                .AddAttribute("RttWindowLength",
                          "Length of RTT windowed filter",
                          TimeValue(Seconds(10)),
                          MakeTimeAccessor(&TcpQtColFair::m_rttPropFilterLen),
                          MakeTimeChecker());
    return tid;
}

TcpQtColFair::TcpQtColFair ()
  : TcpNewReno()
{
    NS_LOG_FUNCTION (this);
}

TcpQtColFair::TcpQtColFair (const TcpQtColFair& sock)
  : TcpNewReno (sock),
    m_rttProp (sock.m_rttProp),
    m_rttPropExpired (sock.m_rttPropExpired),
    m_rttPropFilterLen (sock.m_rttPropFilterLen),
    m_rttPropStamp (sock.m_rttPropStamp),
    m_rttTarget (sock.m_rttTarget),
    m_rttTargetAlpha (sock.m_rttTargetAlpha),
    m_probeRttDuration (sock.m_probeRttDuration),
    m_probeRttPropStamp (sock.m_probeRttPropStamp),
    m_priorInFlight (sock.m_priorInFlight),
    m_cntRtt (sock.m_cntRtt)
{
    NS_LOG_FUNCTION (this);
}

TcpQtColFair::~TcpQtColFair (void)
{
    NS_LOG_FUNCTION (this);
}

void TcpQtColFair::SetRttTargetMultiplier (double rttTargetAlpha)
{
    m_rttTargetAlpha = rttTargetAlpha;
}


void TcpQtColFair::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                     const Time& rtt)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);

    if (rtt.IsZero ())
    {
      return;
    }

    m_rttProp= std::min(m_rttProp, rtt);
    m_rttTarget = m_rttTargetAlpha*m_rttProp;
    //UpdateRttProp(tcb);

    NS_LOG_DEBUG ("Updated m_rttProp = " << m_rttProp);
    
    // Update RTT counter
    m_cntRtt++;
    NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);
}

void
TcpQtColFair::UpdateRttProp(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
   
    if (tcb->m_lastRtt >= Seconds(0) && (tcb->m_lastRtt < m_rttProp || m_rttPropExpired 
                        || m_rttProp == Seconds(0) || m_rttProp == Time::Max ()))
    {
        m_rttProp = tcb->m_lastRtt.Get();
        m_rttTarget = m_rttTargetAlpha*m_rttProp;
        //m_rttPropStamp = Simulator::Now();
    }

}

void TcpQtColFair::MinRttChangeDetection(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    double cur_rtt = tcb->m_lastRtt.Get().GetSeconds();
    double cur_cwndInPkts = tcb->m_cWnd/tcb->m_segmentSize;
    double cur_pktsInFlight = tcb->m_bytesInFlight/tcb->m_segmentSize;
    double cur_time = Simulator::Now().GetSeconds();

    double measure1, measure2, measure3;

    //if (cur_time - m_prev_time >= m_rttProp.GetSeconds())
    //{
            if (cur_cwndInPkts/cur_rtt - m_prev_cwndInPkts/m_prev_rtt != 0)
            {
                measure1 = (cur_pktsInFlight - m_prev_pktsInFlight)/
                            (cur_cwndInPkts/cur_rtt - m_prev_cwndInPkts/m_prev_rtt);
            } else if (cur_pktsInFlight - m_prev_pktsInFlight == 0 
                            && cur_cwndInPkts/cur_rtt - m_prev_cwndInPkts/m_prev_rtt == 0)
            {
                measure1 = 1;
            } else {
                measure1 = (cur_pktsInFlight - m_prev_pktsInFlight)/
                            (cur_cwndInPkts/cur_rtt - m_prev_cwndInPkts/m_prev_rtt);
            }
        
            if (cur_time - m_prev_time > 0)
            {
                measure2 = (cur_pktsInFlight - m_prev_pktsInFlight)/(cur_time - m_prev_time);
            }
            else measure2 = 1111;
        
            measure3 = abs(cur_rtt - m_prev_rtt);
        
            std::cout << Simulator::Now().GetSeconds()
                        << " measure1=" << measure1 
                        << " measure2=" << measure2 
                        << " measure3=" << measure3 
                        << std::endl;
            std::cout << Simulator::Now().GetSeconds()
                        << " cur_rtt=" << cur_rtt 
                        << " m_prev_rtt=" << m_prev_rtt
                        << " m_rttProp=" << m_rttProp.GetSeconds() 
                        << " m_rttTarget=" << m_rttTarget .GetSeconds()
                        << " cur_pktsInFlight=" << cur_pktsInFlight
                        << " m_prev_pktsInFlight=" << m_prev_pktsInFlight
                        << std::endl;
        
            //if ((int)cur_time%10 < 1 && cur_time >= 1) getchar();
            m_prev_cwndInPkts = cur_cwndInPkts;
            m_prev_rtt = cur_rtt;
            m_prev_pktsInFlight = cur_pktsInFlight;
            m_prev_time = cur_time;
        //}

}

void TcpQtColFair::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    m_rttPropExpired = Simulator::Now() > (m_rttPropStamp + m_rttPropFilterLen);
    if (m_rttPropExpired  && !m_probeRtt)
    {
        m_probeRtt = true;
        m_probeRttPropStamp = Simulator::Now ();
        m_priorInFlight = tcb->m_bytesInFlight;
        tcb->m_cWnd = 4 * tcb->m_segmentSize;
        m_probeRttDuration = m_rttProp;
        m_rttProp = Time::Max ();
        m_rttPropStamp = Simulator::Now ();
    }

    // if (!m_probeRtt && ((tcb->m_bytesInFlight <= 0.5*m_priorInFlight 
    //                  &&  tcb->m_lastRtt > 2*m_rttTarget) || tcb->m_bytesInFlight <= 4 * tcb->m_segmentSize))
    //  {
    //      m_rttOvershootCnt++;
    //     if (m_rttOvershootCnt > 2)
    //     {
    //         m_rttProp = tcb->m_lastRtt;
    //         m_usePriorInFlight = true;
    //         m_rttOvershootCnt = 0;
    //     }
    // }
    // else 
    // {
    //     m_rttOvershootCnt = 0;
    // }

    UpdateRttProp(tcb);

    if (Simulator::Now () - m_probeRttPropStamp >= m_probeRttDuration 
            && m_probeRtt && tcb->m_bytesInFlight <= 4 * tcb->m_segmentSize)
    {
        m_probeRtt = false;
        m_usePriorInFlight = true;
    }

    if (!m_probeRtt)
    {
        if (tcb->m_lastAckedSeq >= m_begSndNxt)
        { // as in Vegas, we do QtCol cwnd adjustment every RTT.
          NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");    

            /*
             * as in Vegas, we perform QtCol calculations only if we got enough RTT samples to
             * insure that at least 1 of those samples wasn't from a delayed ACK.
             */
            if (m_cntRtt <= 2)
            { // We do not have enough RTT samples, so we should behave like Reno
                NS_LOG_LOGIC(
                    "We do not have enough RTT samples");
                //TcpNewReno::IncreaseWindow(tcb, segmentsAcked);
            }
            else
            {
                // Save the current right edge for next cycle
                m_begSndNxt = tcb->m_nextTxSequence;
                double Rmin = m_rttProp.GetSeconds ();
                double R = tcb->m_lastRtt.Get ().GetSeconds ();
      
                if (R > 1.01*Rmin)
                    tcb->m_ssThresh = std::max (std::min (tcb->m_ssThresh.Get (), 
                      9*tcb->m_cWnd.Get ()/10), 2 * tcb->m_segmentSize);

                tcb->m_cWnd = ComputeCwnd (tcb);
                //m_rttProp = tcb->m_lastRtt;
                m_cntRtt = 0;
            }
        }
        else if (tcb->m_cWnd < tcb->m_ssThresh && tcb->m_lastRtt < m_rttTarget)
        {
            std::cout << "SlowStart" << std::endl;
            TcpNewReno::SlowStart(tcb, segmentsAcked);
            m_begSndNxt = tcb->m_nextTxSequence;
        }

    }

}

uint32_t TcpQtColFair::ComputeCwnd (Ptr<TcpSocketState> tcb)
{
    //R^s \simeq R^\text{target}
    double Rmin = m_rttProp.GetSeconds ();
    double Rt = m_rttTargetAlpha*Rmin;

    //measured RTT, PacketsInFlight and CwndInSegments
    double R = tcb->m_lastRtt.Get ().GetSeconds ();
    uint32_t L = tcb->m_bytesInFlight/tcb->m_segmentSize;
    uint32_t W = tcb->GetCwndInSegments (); //cwnd in packets

    // predict desired PacketsInFlight
    uint32_t Lpred;
    if (L + W > W*R/Rt)
        Lpred = L + W - W*R/Rt;
    else Lpred = 4;

    if (m_usePriorInFlight)
    {
        Lpred = 0.9*m_priorInFlight/tcb->m_segmentSize;
        m_usePriorInFlight = false;

        std::cout << Simulator::Now().GetSeconds() 
                    << " m_priorInFlight=" << m_priorInFlight
                    << " Lpred=" << Lpred
                    << std::endl;

        //getchar(); 
    }
    
    Lpred = std::max((int) Lpred, 4);
    if (Lpred == L) Lpred = L + 1;

    std::cout << Simulator::Now().GetSeconds()
                << " m_probeRtt=" << m_probeRtt << " m_usePriorInFlight=" << m_usePriorInFlight << std::endl
                << " m_rttProp=" << m_rttProp.GetSeconds()
                << " m_rttTarget=" << m_rttTarget.GetSeconds() << ", " << Rt
                << " m_lastRtt=" << tcb->m_lastRtt.Get().GetSeconds() << std::endl
                << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize 
                << " m_bytesInFlight=" << tcb->m_bytesInFlight/tcb->m_segmentSize
                << " Lpred=" << Lpred 
                << std::endl;

    //compute desired cwnd - minimum cwnd allowed is 4 packets
    uint32_t Wnew = W + std::ceil(Rmin*((double) Lpred - (double) L)/R);
    Wnew = std::max((int) Wnew, 4);
    if (Wnew == W) Wnew = W + 1; //required for persistent excitation

    std::cout << Simulator::Now().GetSeconds()
            << " m_rttProp=" << m_rttProp.GetSeconds()
            << " m_rttTargetAlpha=" << m_rttTargetAlpha
            << " m_rttTarget=" << m_rttTarget.GetSeconds()
            << " m_lastRtt=" << tcb->m_lastRtt.Get().GetSeconds() << std::endl
            << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize 
            << " m_bytesInFlight=" << tcb->m_bytesInFlight/tcb->m_segmentSize
            << " Lpred=" << Lpred 
            << " Wnew=" << Wnew 
            << std::endl << std::endl;
    //getchar();

    return Wnew * tcb->m_segmentSize;
}

std::string
TcpQtColFair::GetName () const
{
  return "TcpQtColFair";
}

Ptr<TcpCongestionOps>
TcpQtColFair::Fork ()
{
  return CopyObject<TcpQtColFair> (this);
}


}