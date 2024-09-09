#!/bin/bash
cd ..

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000010 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000010 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000010 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000010 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000020 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000020 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000020 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000020 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000005 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000005 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000005 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000005 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000050 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000050 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000050 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_single_lossy" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000050 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"