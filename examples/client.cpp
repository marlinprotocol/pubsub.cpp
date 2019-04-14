#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <uv.h>
#include <cstring>
#include <algorithm>
#include <marlin/net/Node.hpp>
#include "PubSubNode.hpp"


using namespace marlin::net;
using namespace marlin::stream;
using namespace marlin::pubsub;
using namespace std;


class PubSubClient {
public:
	void did_unsubscribe(PubSubNode<PubSubClient> &, std::string channel) {
		SPDLOG_INFO("Did unsubscribe: {}", channel);
	}

	void did_subscribe(PubSubNode<PubSubClient> &, std::string channel) {
		SPDLOG_INFO("Did subscribe: {}", channel);
	}

	void did_receive_message(PubSubNode<PubSubClient> &, std::unique_ptr<char[]> &&message, uint64_t size, std::string &channel, uint64_t message_id) {
		SPDLOG_INFO("Received message {} on channel {}: {}", message_id, channel, spdlog::to_hex(message.get(), message.get() + size));
	}
};

using NodeType = PubSubNode<PubSubClient>;

int main() {
	spdlog::default_logger()->set_level(spdlog::level::info);

	PubSubClient b_del, b2_del;

	auto addr = SocketAddress::from_string("127.0.0.1:8000");
	auto b = new NodeType(addr);
	b->delegate = &b_del;
	b->start_listening();

	auto addr2 = SocketAddress::from_string("127.0.0.1:8001");
	auto b2 = new NodeType(addr2);
	b2->delegate = &b2_del;
	b2->start_listening();

	SPDLOG_INFO("Start");

	// b->send_MESSAGE(addr2, std::string("Test channel"), data.get(), SIZE);

	b->send_SUBSCRIBE(addr2, std::string("test_channel"));
	// b2->send_SUBSCRIBE(addr2, std::string("test_channel"));

	// b->send_UNSUBSCRIBE(addr2, std::string("test_channel"));

	return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}