#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("assg8");

int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  /*--------------Main Routers------------------------------*/
  NodeContainer mainRouters;
  mainRouters.Create (2);

  PointToPointHelper mRp2p;
  mRp2p.SetDeviceAttribute ("DataRate", StringValue ("2Mbps"));
  mRp2p.SetChannelAttribute ("Delay", StringValue ("20ms"));

  NetDeviceContainer mR_devices;
  mR_devices = mRp2p.Install (mainRouters);

  InternetStackHelper stack;
  stack.Install (mainRouters);

  Ipv4AddressHelper address;
  address.SetBase ("1.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (mR_devices);
  /*--------------------------------------------------------*/
  /*--------------CCC Server 1------------------------------*/
  NodeContainer cccServer1;
  cccServer1.Add(mainRouters.Get(0));
  cccServer1.Create(1);

  PointToPointHelper ccc1;
  ccc1.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  ccc1.SetChannelAttribute("Delay", StringValue("20ms"));

  NetDeviceContainer cccS1_devices;
  cccS1_devices = ccc1.Install(cccServer1);

  stack.Install(cccServer1.Get(1));

  address.SetBase("1.1.2.0", "255.255.255.0");

  Ipv4InterfaceContainer cccS1interfaces = address.Assign (cccS1_devices);
  /*--------------------------------------------------------*/
  /*--------------CCC Server 2------------------------------*/
  NodeContainer cccServer2;
  cccServer2.Add(mainRouters.Get(0));
  cccServer2.Create(1);

  PointToPointHelper ccc2;
  ccc2.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  ccc2.SetChannelAttribute("Delay", StringValue("20ms"));

  NetDeviceContainer cccS2_devices;
  cccS2_devices = ccc2.Install(cccServer2);

  stack.Install(cccServer2.Get(1));

  address.SetBase("1.1.3.0", "255.255.255.0");

  Ipv4InterfaceContainer cccS2interfaces = address.Assign (cccS2_devices);
  /*--------------------------------------------------------*/
  /*--------------CNC router--------------------------------*/
  NodeContainer cncRouter;
  cncRouter.Add(mainRouters.Get(1));
  cncRouter.Create(1);

  PointToPointHelper cncUP;
  cncUP.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  cncUP.SetChannelAttribute("Delay", StringValue("20ms"));

  NetDeviceContainer cncRouter_devices;
  cncRouter_devices = cncUP.Install(cncRouter);

  //stack.Install(cncRouter.Get(1));

  //address.SetBase("1.1.4.0", "255.255.255.0");

  //Ipv4InterfaceContainer cncRouterinterfaces = address.Assign (cncRouter_devices);
  /*--------------------------------------------------------*/
  /*--------------CNC router CSMA---------------------------*/
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NodeContainer cncCSMAnodes;
  cncCSMAnodes.Add(cncRouter.Get(1));
  cncCSMAnodes.Create(4);

  NetDeviceContainer cncCSMA_devices;
  cncCSMA_devices = csma.Install(cncCSMAnodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute("ErrorUnit", StringValue("ERROR_UNIT_PACKET"));
  em->SetAttribute ("ErrorRate", DoubleValue (0.5));
  cncCSMA_devices.Get (4)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  stack.Install(cncCSMAnodes);

  address.SetBase("1.1.5.0", "255.255.255.0");

  Ipv4InterfaceContainer cncCSMAinterfaces = address.Assign (cncCSMA_devices);
  address.SetBase("1.1.4.0", "255.255.255.0");

  Ipv4InterfaceContainer cncRouterinterfaces = address.Assign (cncRouter_devices);
  /*--------------------------------------------------------*/
  /*--------------CNC router wifi---------------------------*/
  NodeContainer cncRouterWifi;
  cncRouterWifi.Add(mainRouters.Get(1));
  cncRouterWifi.Create(1);

  PointToPointHelper cncDOWN;
  cncDOWN.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  cncDOWN.SetChannelAttribute("Delay", StringValue("20ms"));

  NetDeviceContainer cncRouterWifi_devices;
  cncRouterWifi_devices = cncDOWN.Install(cncRouterWifi);

  stack.Install(cncRouterWifi.Get(1));

  address.SetBase("1.1.6.0", "255.255.255.0");

  Ipv4InterfaceContainer cncRouterWifiinterfaces = address.Assign (cncRouterWifi_devices);
  /*--------------------------------------------------------*/
  /*-------------------CNC WIFI nodes-----------------------*/
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (3);
  NodeContainer wifiApNode = cncRouterWifi.Get (1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);

  stack.Install(wifiStaNodes);
  address.SetBase("1.1.7.0", "255.255.255.0");

  Ipv4InterfaceContainer cncWifiAPinterfaces = address.Assign (apDevices);
  Ipv4InterfaceContainer cncWifiStainterfaces = address.Assign (staDevices);

  /*--------------------------------------------------------*/

  UdpEchoServerHelper echoServer (69);

  ApplicationContainer serverApps = echoServer.Install (cccServer1.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (100.0));

  UdpEchoClientHelper echoClient (cccS1interfaces.GetAddress (1), 69);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes.Get (2));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (100.0));

  UdpEchoServerHelper echoServer2 (70);

  ApplicationContainer serverApps2 = echoServer2.Install (cccServer2.Get (1));
  serverApps2.Start (Seconds (5.0));
  serverApps2.Stop (Seconds (100.0));

  UdpEchoClientHelper echoClient2 (cccS2interfaces.GetAddress (1), 70);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (5));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = echoClient2.Install (cncCSMAnodes.Get (4));
  clientApps2.Start (Seconds (7.0));
  clientApps2.Stop (Seconds (100.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  AsciiTraceHelper ascii;
  mRp2p.EnableAsciiAll(ascii.CreateFileStream("first.tr"));


  Simulator::Stop (Seconds (200.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
