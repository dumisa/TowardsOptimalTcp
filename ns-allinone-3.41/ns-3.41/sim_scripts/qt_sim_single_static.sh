#!/bin/bash
cd ..

./ns3 run "examples/tcp/tcp_qt_static_network --transport_prot="TcpBbr" --sim_name="../../results/static" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static_network --transport_prot="TcpCubic" --sim_name="../../results/static" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static_network --transport_prot="TcpQtCol" --sim_name="../../results/static" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static_network --transport_prot="TcpQtCol" --sim_name="../../results/static" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --rttTargetAlpha=1.5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static_network --transport_prot="TcpQtColFair" --sim_name="../../results/static" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static_network --transport_prot="TcpQtColFair" --sim_name="../../results/static" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --rttTargetAlpha=1.5 --sim_duration=100"