#pragma once

#include <starlight/network/network.h>

#include <enet.h>

static ENetPacketFlag network_send_mode_to_enet_flags(NetworkSendMode mode)
{
	switch (mode)
	{
		case NETWORK_MODE_RELIABLE: return ENET_PACKET_FLAG_RELIABLE;
		case NETWORK_MODE_UNSEQUENCED: return ENET_PACKET_FLAG_UNSEQUENCED;
		case NETWORK_MODE_UNRELIABLE_FRAGMENTED: return ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
		case NETWORK_MODE_UNRELIABLE:
		default: return 0;
    }
}

static inline NetworkPeer* network_peer_from_enet_peer(ENetPeer* p) { return (NetworkPeer*)p; }
static inline ENetPeer* network_peer_to_enet_peer(NetworkPeer* p) { return (ENetPeer*)p; }
