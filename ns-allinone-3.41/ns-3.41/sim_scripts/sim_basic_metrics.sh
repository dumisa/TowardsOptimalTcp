#!/bin/bash
cd ..

./ns3 run "examples/tcp/my_sim_basic_metrics --transport_prot="TcpBbr" --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/my_sim_basic_metrics --transport_prot="TcpCubic" --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/my_sim_basic_metrics.cc --transport_prot="TcpQtOptimal" --Rt_mult=1.1 --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/my_sim_basic_metrics.cc --transport_prot="TcpQtOptimal" --Rt_mult=1.2 --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/my_sim_basic_metrics.cc --transport_prot="TcpQtOptimal" --Rt_mult=1.3 --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/my_sim_basic_metrics.cc --transport_prot="TcpQtOptimal" --Rt_mult=1.4 --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/my_sim_basic_metrics.cc --transport_prot="TcpQtOptimal" --Rt_mult=1.5 --sim_name="../../results/basic_metrics" --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"