#include "CNetworkManager.h"

CNetworkManager::CNetworkManager():
	m_address(),
	m_host()
{
	enet_initialize();
}

CNetworkManager::~CNetworkManager()
{
	if (m_host != nullptr) {
		enet_host_destroy(m_host);
	}

	enet_deinitialize();
}

bool CNetworkManager::Init(std::string& address, int port)
{
	m_address.host = ENET_HOST_ANY;
	m_address.port = port;

	enet_address_set_host(&m_address, address.c_str());

	m_host = enet_host_create(&m_address, 32, 2, 0, 0);
	if (m_host == NULL) {
		return false;
	}

	m_PlayerManager = new CPlayerManager();

	return true;
}

void CNetworkManager::Pulse()
{
	ENetEvent event;

	while (enet_host_service(m_host, &event, 1000) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				event.peer->data = "Client information";
				m_PlayerManager->CreateEntity(0, event.peer);//TODO: Auto allocate networkID
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->data,
				event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			m_PlayerManager->DeleteEntity(0);//TODO: Auto allocate networkID
			event.peer->data = NULL;
		}
	}
}