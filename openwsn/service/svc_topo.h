#ifndef _SVC_TOPOLOGY_4768_
#define _SVC_TOPOLOGY_4768_

typedef struct{
  uint32 id;
  uint8 age;
  uin8 hop;
}TiNetworkNode;

typedef struct{
  uint32 endpoint1;
  uint32 endpoint2;
  uint32 distance;
  uint8 age;
}TiNetworkEdge;

typedef struct{
  TNetworkNode[20] nodes; 
  TNetworkEdge[100] edges; 
}TiNetworkTopology;

void topo_construct
void topo_destroy
void topo_evolve

#endif
