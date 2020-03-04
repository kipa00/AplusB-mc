#include <cstdio>
#include <queue>
#include <string>
using std::string;

#include "node.hpp"
#include "bytestream.hpp"

using std::queue;

void help(string fn) {
	fprintf(stderr, "Usage: %s [other flags ...]\n", fn.c_str());
	fprintf(stderr, "\n");
	fprintf(stderr, "flags include:\n");
	fprintf(stderr, "  --max-tick (number)\n    limit execution tick to given number of ticks maximum.\n");
	fprintf(stderr, "  --ban-block (wire|torch|repeater|comparator)\n    if given circuit contains the block, rejects the execution.\n");
}

int main(int argc, char *argv[]) {
	int ban_block = 0, tick_limit = -1;
	int parse_status = 0;
	string fn = "";
	for (int i=1; i<argc; ++i) {
		string now_argv = argv[i];
		if (now_argv == "--max-tick") {
			parse_status = 1;
		} else if (now_argv == "--ban-block") {
			parse_status = 2;
		} else {
			if (parse_status == 0) {
				if (fn == "") fn = argv[i];
				else {
					help(argv[0]);
					return -1;
				}
			} else if (parse_status == 1) {
				if (sscanf(now_argv.c_str(), "%d", &tick_limit) != 1 || tick_limit < 0) {
					help(argv[0]);
					return -1;
				}
				parse_status = 0;
			} else if (parse_status == 2) {
				if (now_argv == "wire") ban_block |= 2;
				else if (now_argv == "torch") ban_block |= 4;
				else if (now_argv == "repeater") ban_block |= 120;
				else if (now_argv == "comparator") ban_block |= 384;
				else {
					help(argv[0]);
					return -1;
				}
				parse_status = 0;
			}
		}
	}
	if (parse_status != 0 || fn == "") {
		help(argv[0]);
		return -1;
	}
	bytereader bs(argv[1]);
	vector<node *> graph;
	byte b;
	graph.push_back(nullptr);
	while ((b = bs.read_byte()) != 255) {
		node *n = nullptr;
		if (ban_block & (1 << (b >> 4))) {
			fprintf(stderr, "stopping execution from error: banned block used\n");
			return -2;
		}
		switch (b >> 4) {
			case 1: // redstone wire
				n = new wire;
				break;
			case 2: // redstone torch
				n = new torch;
				break;
			case 3:
			case 4:
			case 5:
			case 6: // repeater
				n = new repeater((b >> 4) - 2);
				break;
			case 7: // comparator
				n = new comparator;
				break;
			case 8: // subtractor
				n = new subtractor;
				break;
			case 9: // lever
				n = new lever;
				break;
			case 10: // lamp
				n = new lamp;
				break;
		}
		switch (b >> 4) {
			case 2: // redstone torch
			case 9: // lever
			case 10: // lamp
				{
					int u;
					while ((u = bs.read_int()) >= 0) {
						n->input.push_back(u);
					}
				}
				break;
			case 1: // redstone wire
			case 3:
			case 4:
			case 5:
			case 6: // repeater
			case 7: // comparator
			case 8: // subtractor
				for (int i=0; i<2; ++i) {
					int u;
					while ((u = bs.read_int()) >= 0) {
						n->input.push_back(u);
					}
					if (i == 0) n->input.push_back(-1);
				}
				break;
		}
		if (!n) {
			fprintf(stderr, "stopping execution from serious error: corrupted file!\n");
			return -2;
		}
		{
			int u;
			while ((u = bs.read_int()) >= 0) {
				n->need_update.push_back(u);
			}
		}
		n->power = b & 15;
		graph.push_back(n);
	}
	vector<int> input[16], output;
	int input_count = 0;
	{
		int u;
		while ((u = bs.read_int()) >= 0) {
			do {
				input[input_count].push_back(u);
			} while ((u = bs.read_int()) >= 0);
			++input_count;
		}
		while ((u = bs.read_int()) >= 0) {
			output.push_back(u);
		}
	}
	queue<int> temp[5], *update[5];
	for (int i=0; i<5; ++i) {
		update[i] = &temp[i];
	}
	for (int i=0; i<input_count; ++i) {
		i64 t;
		scanf("%lld", &t);
		for (const auto &node_idx : input[i]) {
			lever *n = (lever *)graph[node_idx];
			if (n->set_as(t & 1)) {
				for (const int &x : n->need_update) {
					update[graph[x]->delay]->push(x);
				}
			}
			t >>= 1;
		}
	}
	int tick = 0, last_updated_tick = 0;
	while (!update[0]->empty() || !update[1]->empty() || !update[2]->empty() || !update[3]->empty() || !update[4]->empty()) {
		if (tick_limit >= 0 && tick > tick_limit + 4) {
			fprintf(stderr, "stopping execution from error: tick limit exceeded\n");
			return -2;
		}
		while (!update[0]->empty()) {
			const int f = update[0]->front();
			update[0]->pop();
			node *now_node = graph[f];
			int a = -1, b = -1;
			for (const int &other : now_node->input) {
				if (other >= 0) {
					a = std::max(a, graph[other]->power);
				} else {
					b = std::max(a, 0);
					a = 0;
				}
			}
			bool result = false;
			if (b < 0) {
				result = now_node->update(std::max(a, 0));
			} else {
				result = now_node->update(b, a);
			}
			if (result) {
				for (const int &other : now_node->need_update) {
					update[graph[other]->delay]->push(other);
				}
				last_updated_tick = tick;
			}
		}
		++tick;
		{
			auto temp = update[0];
			update[0] = update[1];
			update[1] = update[2];
			update[2] = update[3];
			update[3] = update[4];
			update[4] = temp;
		}
	}
	if (tick_limit >= 0 && last_updated_tick > tick_limit) {
		fprintf(stderr, "stopping execution from error: tick limit exceeded\n");
		return -2;
	}
	u64 t = 0;
	int i;
	for (i=0; i<output.size(); ++i) {
		t |= (graph[output[i]]->power ? 1 : 0) << i;
	}
	printf("%llu\n", t);
	return 0;
}
