
#pragma once

#include "Mona/Mona.h"
#include "AMF.h"
#include "AMFReader.h"
#include "FlashWriter.h"

namespace FlashEvents {
	struct OnStatus : Mona::Event<void(const std::string& code, const std::string& description, FlashWriter& writer)> {};
	struct OnMedia : Mona::Event<void(const std::string& peerId, const std::string& stream, Mona::UInt32 time, Mona::PacketReader& packet, double lostRate, bool audio)> {};
	struct OnPlay: Mona::Event<bool(const std::string& streamName, FlashWriter& writer)> {};
	struct OnNewPeer : Mona::Event<void(const std::string& groupId, const std::string& peerId)> {};
	struct OnGroupHandshake : Mona::Event<void(const std::string& groupId, const std::string& key, const std::string& peerId)> {};
	struct OnGroupMedia : Mona::Event<bool(const std::string& streamName, const std::string& data)> {};
};

/**************************************************************
FlashStream is linked to an as3 NetStream
*/
class FlashStream : public virtual Mona::Object,
	public FlashEvents::OnStatus,
	public FlashEvents::OnMedia,
	public FlashEvents::OnPlay,
	public FlashEvents::OnNewPeer,
	public FlashEvents::OnGroupHandshake,
	public FlashEvents::OnGroupMedia {
public:

	FlashStream(Mona::UInt16 id);
	virtual ~FlashStream();

	const Mona::UInt16	id;

	Mona::UInt32	bufferTime(Mona::UInt32 ms);
	Mona::UInt32	bufferTime() const { return _bufferTime; }

	void	disengage(FlashWriter* pWriter=NULL);

	// return flase if writer is closed!
	bool	process(AMF::ContentType type,Mona::UInt32 time,Mona::PacketReader& packet,FlashWriter& writer, double lostRate=0);

	virtual void	flush() { }

	// Send the connect request to the RTMFP server
	virtual void connect(FlashWriter& writer,const std::string& url);

	// Send the createStream request to the RTMFP server
	virtual void createStream(FlashWriter& writer);

	// Send the play request to the RTMFP server
	virtual void play(FlashWriter& writer, const std::string& name, bool amf3=false);

	// Send the publish request to the RTMFP server
	virtual void publish(FlashWriter& writer, const std::string& name);

	// Send the setPeerInfo request to the RTMFP server
	virtual void sendPeerInfo(FlashWriter& writer, Mona::UInt16 port);

	// Send the group connection request to the server
	void sendGroupConnect(FlashWriter& writer, const std::string& groupId);

	// Send the group connection request to the peer
	void sendGroupPeerConnect(FlashWriter& writer, const std::string& netGroup, const Mona::UInt8* key, const std::string& peerId, bool initiator);

	// Record target peer ID for identifying media source (play mode)
	virtual void setPeerId(const std::string& peerId) { _peerId = peerId; }

private:

	virtual void	messageHandler(const std::string& name, AMFReader& message, FlashWriter& writer);
	virtual void	rawHandler(Mona::UInt16 type, Mona::PacketReader& data, FlashWriter& writer);
	virtual void	dataHandler(DataReader& data, double lostRate);
	virtual void	audioHandler(Mona::UInt32 time, Mona::PacketReader& packet, double lostRate);
	virtual void	videoHandler(Mona::UInt32 time,Mona::PacketReader& packet, double lostRate);
	virtual void	memberHandler(const std::string& peerId);

	Mona::UInt32	_bufferTime;
	std::string		_streamName;

	std::string		_peerId; // peer ID (only for p2p play stream)
	std::string		_groupId; // Group ID (only for NetGroup stream)
	std::string		_targetID; // Peer ID of the target

	bool			_message3Sent; // True if NetGroup message 3 has been sent to target peer

	//Mona::UInt32	_timeFrequency; // to retrieve time
};
